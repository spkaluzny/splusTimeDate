/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
 *************************************************************************/

/*************************************************************************
 *
 * It contains C code utility functions for dealing with R
 * time zone objects.
 *
 * See zoneObj.h for a more compact listing of the included functions.
 * They were written as auxiliary functions for R code
 * dealing with times and dates.
 *
 *************************************************************************/

#include "timeUtils.h"
#include "zoneObj.h"

/* definitions needed for time zone classes */

static SEXP name_slot;
static SEXP offset_slot;
static SEXP rules_slot;

static int zone_initialized = 0;
static void zone_init(void);
static int r_zone_to_struct( SEXP obj, void **ret_struct );

/**********************************************************************
 * R-DOCUMENTATION ************************************************
 **********************************************************************
   NAME zone_init

   DESCRIPTION  Initialize the time zone R class variables.

   ARGUMENTS

   RETURN 

   ALGORITHM This is a C function that initializes the R time zone classes
   for use in C functions.  It initializes the ``C_zone_class'' variable
   and other variables used internally. 
   
   EXCEPTIONS 

   NOTE

 **********************************************************************/
static void zone_init()
{

  if( zone_initialized )
    return;
  
  /* look up the time zone classes */
  zone_initialized = 1;

  /* get the slot offsets */
  name_slot = install("name");
  offset_slot = install("offset");
  rules_slot = install("rules");
}

/**********************************************************************
 * R-DOCUMENTATION ************************************************
 **********************************************************************
   NAME find_zone_info

   DESCRIPTION  Find the given zone name's object in the time zone list, 
   and extract information from the zone object.

   ARGUMENTS
      IARG  name         The name to find in the list
      IARG  zone_list    The R time zone list
      OARG  zone_info    Returned information (see below)
      OARG  is_R         1/0 for S/C time zone object

   RETURN Returns 1/0 for success/failure.

   ALGORITHM This function finds the name component of zone_list
   using functions from Rinternals.h and Rdefines.h.  If it is a C time zone object, (as
   determined by checkClass), it sets is_R to 0 and returns a
   char ** pointer to the built-in time zone object name in zone_info
   (extracted using functions from Rinternals.h and Rdefines.h).  if it is an R time zone
   object (as determined by checkClass), it sets is_R to 1 and 
   converts the offset and rules information into a time zone
   struct by calling r_zone_to_struct, and returns a pointer to it.
   If zone_init has not been called, it is called.

   EXCEPTIONS 

   NOTE See also: find_zone, zone_init

**********************************************************************/
int find_zone_info( const char *name, SEXP zone_list, void **zone_info, 
		    int *is_R )
{
  SEXP tmp_data, names;
  const char *tmp_char;
  char *info_char;
  Sint len;
  int ctype;
  static const char *classes[] = {
    C_ZONE_CLASS_NAME
  };

  if( !zone_initialized )
    zone_init();

  if( !name || !zone_list || !zone_info || !is_R )
    return 0;

  if(!isNewList(zone_list))
    return 0;

  names = getAttrib(zone_list, R_NamesSymbol);

  tmp_data = getListElement(zone_list, name);
  if( !tmp_data )
    return 0;

  ctype = checkClass( tmp_data, classes, 1L );

  /* see if it's an R or C time zone */
  if( !ctype )
  {
    *is_R = 1;
    return( r_zone_to_struct( tmp_data, zone_info ));
  }

  /* it's a C object */
  *is_R = 0;

  /* find the name from the zone object */
  tmp_data = GET_SLOT( tmp_data, install("name"));

  if(!isString(tmp_data) || length(tmp_data) < 1 ||
     !(tmp_char = CHAR(STRING_ELT(tmp_data, 0))))
    return 0;

  info_char =  acopy_string(tmp_char);

  *zone_info = (void *) info_char;

  return 1;
}


/**********************************************************************
 * R-DOCUMENTATION ************************************************
 **********************************************************************
   NAME r_zone_to_struct

   DESCRIPTION  Extract information from an R time zone object
   into a TZONE_STRUCT.

   ARGUMENTS
      IARG  obj          The R time zone object
      OARG  ret_struct   Returned structure

   RETURN Returns 1/0 for success/failure.

   ALGORITHM This function extracts the offset and daylight savings
   rules from an R time zone object and puts it into a new C
   time zone struct, and returns in in ret_struct.  
   The structure is allocated with Salloc.
   If zone_init has not been called, it is called.

   EXCEPTIONS 

   NOTE See also: find_zone_info, zone_init

**********************************************************************/
static int r_zone_to_struct( SEXP obj, void **ret_struct )
{
  TZONE_STRUCT *tz;
  TZONE_RULE_STRUCT *prev_rule, *this_rule;

  SEXP sptr, sptr2;
  Sint *dataptr, *yearfroms, *yeartos, *hasdaylights, *dsextras,
    *monthstarts, *codestarts, *daystarts, *xdaystarts, *timestarts,
    *monthends, *codeends, *dayends, *xdayends, *timeends;
  Sint len, tmplen, i;
  static const char *classes[] = {
    R_ZONE_CLASS_NAME    
  };

  if( !zone_initialized )
    zone_init();

  if( !ret_struct || !obj || 
      !checkClass( obj, classes, 1L ))
    return 0;

  /* allocate a time zone struct */

  tz = (TZONE_STRUCT *) R_alloc( 1L, sizeof(TZONE_STRUCT) );
  if( !tz )
    return 0;
  *ret_struct = (void *) tz;

  /* extract the offset */
  /* don't need to protect since its constrained in class def */
  sptr = GET_SLOT( obj, offset_slot);
  if( !sptr || length(sptr)<1 ||
      !(dataptr = INTEGER(sptr)))
    return 0;

  tz->offset = dataptr[0];

  /* extract the rules */

  sptr2 = GET_SLOT( obj, rules_slot);
  if( !sptr2 )
    return 0;

  len = length(sptr2);
  /* see if it's empty -- i.e. no daylight time */
  if( len == 0 )
  {
    tz->rule = NULL;
    return 1;
  } 
  /* sptr2 should be a data.frame */
  if(!isFrame( sptr2 ))
    return 0;

  /* extract the data */
  if( len != 14 )
    return 0;

  yearfroms = INTEGER( PROTECT( (SEXP) AS_INTEGER( VECTOR_ELT( sptr2, 0) ) ));
  yeartos = INTEGER( PROTECT( (SEXP) AS_INTEGER( VECTOR_ELT( sptr2, 1) ) ));
  hasdaylights = INTEGER( PROTECT( (SEXP) AS_INTEGER( VECTOR_ELT( sptr2, 2) ) ));
  dsextras = INTEGER( PROTECT( (SEXP) AS_INTEGER( VECTOR_ELT( sptr2, 3) ) ));
  monthstarts = INTEGER( PROTECT( (SEXP) AS_INTEGER( VECTOR_ELT( sptr2, 4) ) ));
  codestarts = INTEGER( PROTECT( (SEXP) AS_INTEGER( VECTOR_ELT( sptr2, 5) ) ));
  daystarts = INTEGER( PROTECT( (SEXP) AS_INTEGER( VECTOR_ELT( sptr2, 6) ) ));
  xdaystarts = INTEGER( PROTECT( (SEXP) AS_INTEGER( VECTOR_ELT( sptr2, 7) ) ));
  timestarts = INTEGER( PROTECT( (SEXP) AS_INTEGER( VECTOR_ELT( sptr2, 8) ) ));
  monthends = INTEGER( PROTECT( (SEXP) AS_INTEGER( VECTOR_ELT( sptr2, 9) ) ));
  codeends = INTEGER( PROTECT( (SEXP) AS_INTEGER( VECTOR_ELT( sptr2, 10) ) ));
  dayends = INTEGER( PROTECT( (SEXP) AS_INTEGER( VECTOR_ELT( sptr2, 11) ) ));
  xdayends = INTEGER( PROTECT( (SEXP) AS_INTEGER( VECTOR_ELT( sptr2, 12) ) ));
  timeends = INTEGER( PROTECT( (SEXP) AS_INTEGER( VECTOR_ELT( sptr2, 13) ) ));

  if( !yearfroms || !yeartos || !hasdaylights || !dsextras || !monthstarts ||
      !codestarts || !daystarts|| !xdaystarts || !timestarts || !monthends ||
      !codeends || !dayends || !xdayends || !timeends){
    UNPROTECT(14);
    return 0;
  }

  len = length(VECTOR_ELT(sptr2, 0));

  if( len == 0 )
  {
    tz->rule = NULL;
    UNPROTECT(14);
    return 1;
  }

  /* read the rules into rules structs */

  prev_rule = NULL;

  for( i = 0; i < len; i++ )
  {
    this_rule = (TZONE_RULE_STRUCT *) R_alloc( 1, sizeof(TZONE_RULE_STRUCT) );
    this_rule->prev_rule = prev_rule;
    prev_rule = this_rule;

    this_rule->yearfrom =  yearfroms[i];
    this_rule->yearto =  yeartos[i];
    this_rule->hasdaylight =  hasdaylights[i];
    this_rule->dsextra = dsextras[i];
    this_rule->monthstart =  monthstarts[i];
    this_rule->daystart =  daystarts[i];
    this_rule->xdaystart =  xdaystarts[i];
    this_rule->timestart = timestarts[i];
    this_rule->monthend =  monthends[i];
    this_rule->dayend =  dayends[i];
    this_rule->xdayend =  xdayends[i];
    this_rule->timeend = timeends[i];

    switch( codestarts[i] )
    {
    case 1:
      this_rule->codestart = CODE_MONTHDAY;
      break;
    case 2:
      this_rule->codestart = CODE_LAST_WEEKDAY;
      break;
    case 3:
      this_rule->codestart = CODE_WEEKDAY_GE;
      break;
    case 4:
      this_rule->codestart = CODE_WEEKDAY_LE;
      break;
    default:
      UNPROTECT(14); 
      return 0;
    }

    switch( codeends[i] )
    {
    case 1:
      this_rule->codeend = CODE_MONTHDAY;
      break;
    case 2:
      this_rule->codeend = CODE_LAST_WEEKDAY;
      break;
    case 3:
      this_rule->codeend = CODE_WEEKDAY_GE;
      break;
    case 4:
      this_rule->codeend = CODE_WEEKDAY_LE;
      break;
    default: 
      UNPROTECT(14);
      return 0;
    }
  }

  tz->rule = this_rule;
  UNPROTECT(14);
  return 1;
  
}
