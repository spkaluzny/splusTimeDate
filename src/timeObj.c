/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
 *************************************************************************/

/*************************************************************************
 *
 * It contains C code utility functions for dealing with R
 * time objects.
 *
 * See timeObj.h for a more compact listing of the included functions.
 * They were written as auxiliary functions for R code
 * dealing with times and dates.
 *
 *************************************************************************/

#include "timeObj.h"

/* definitions needed for time class */

static SEXP time_class;
static SEXP tspan_class;

static SEXP columns_slot, format_slot, zone_slot;
static SEXP sp_data_slot, sp_format_slot;
static int time_initialized = 0;

static const char *DEFAULT_OUT_FORMAT = " ";
static const char *DEFAULT_ZONE = "GMT";

/* internal functions */
static void time_init();


/****************************
  Internal functions
 ****************************/

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME time_init

   DESCRIPTION  Initialize the time_class and tspan_class R class 
   variables.

   ARGUMENTS

   RETURN 

   ALGORITHM This is a C function that initializes the R time and
   time span classes for use in C functions.  It initializes 
   the ``time_class'' and ``tspan_class'' variables
   and other variables used internally. 
   
   EXCEPTIONS 

   NOTE

**********************************************************************/
static void time_init()
{

  if( time_initialized )
    return;
  
  /* look up the time classes */
  time_initialized = 1;
  time_class = MAKE_CLASS( TIME_CLASS_NAME );
  tspan_class = MAKE_CLASS( TSPAN_CLASS_NAME );
  /* get the slot offsets */
  columns_slot = install("columns");
  format_slot = install("format");
  zone_slot = install("time.zone");

}

/****************************
  External functions
 ****************************/

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME time_format_pointer

   DESCRIPTION  Get a pointer to the format portion of an R time object.

   ARGUMENTS
      IARG  time_obj  The R time or time span object

   RETURN Returns a pointer to the format slot of the R time object,
   or NULL if there is an error.

   ALGORITHM This function first verifies the input is an R time or
   time span object by calling the checkClass function, and then gets 
   a pointer to the actual portion of the object which contains the format,
   using functions from Rinternals.h and Rdefines.h. If time_init has not previously been called, 
   it is called.

   EXCEPTIONS 

   NOTE See also: time_zone_pointer, time_julian_pointer, time_ms_pointer,
   time_data_pointer

**********************************************************************/
SEXP time_format_pointer( SEXP time_obj )
{
  int cl;
  static const char *classes[] = {
    TIME_CLASS_NAME, TSPAN_CLASS_NAME
  };
  if( !time_initialized )
    time_init();
  
  cl = checkClass( time_obj, classes, 2L );

  if(cl < 0)
    return(NULL);
  else
    return(STRING_ELT(GET_SLOT(time_obj, format_slot),0));

}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME time_zone_pointer

   DESCRIPTION  Get a pointer to the time.zone portion of an R time object.

   ARGUMENTS
      IARG  time_obj  The R time object

   RETURN Returns a pointer to the time zone slot of the R time object,
   or NULL if there is an error.

   ALGORITHM This function first verifies the input is an R time object
   by calling the checkClass function, and then gets a pointer to the
   actual portion of the R time object which contains the time zone,
   using functions from Rinternals.h and Rdefines.h. If time_init has
   not been called, it is called.

   EXCEPTIONS 

   NOTE See also: time_format_pointer, time_julian_pointer, time_ms_pointer,
   time_data_pointer, checkClass

**********************************************************************/
SEXP time_zone_pointer( SEXP time_obj )
{
  int cl;
  static const char *classes[] = {
    TIME_CLASS_NAME
  };

  if( !time_initialized )
    time_init();
  
  cl = checkClass( time_obj, classes, 1L );

  if( cl < 0)
    return NULL; 
  else 
    return(STRING_ELT(GET_SLOT(time_obj, zone_slot), 0));
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME time_data_pointer

   DESCRIPTION  Get a pointer to the data portion of an R time object.

   ARGUMENTS
      IARG  time_obj  The R time or time span object

   RETURN Returns a pointer to the .Data slot of the R time object,
   or NULL if there is an error.

   ALGORITHM This function first verifies the input is an R time or
   time span object by calling the checkClass function, and then gets 
   a pointer to the actual portion of the object which contains the data,
   which is a list containing the julian day vector and the 
   millisecond vector, using functions from Rinternals.h and Rdefines.h. If time_init has 
   not previously been called, it is called.

   EXCEPTIONS 

   NOTE See also: time_format_pointer, time_zone_pointer, 
   time_julian_pointer, time_ms_pointer

**********************************************************************/
SEXP time_data_pointer( SEXP time_obj )
{
  int cl;
  static const char *classes[] = {
    TIME_CLASS_NAME, TSPAN_CLASS_NAME
  };

  if( !time_initialized )
    time_init();
  
  cl = checkClass( time_obj, classes, 2L );

  if( cl < 0)
    return NULL; 
  else 
    return(GET_SLOT(time_obj, install("columns")));
}



/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME time_julian_pointer

   DESCRIPTION  Get a pointer to the julian day portion of an R time object.

   ARGUMENTS
      IARG  time_obj  The R time or time span object

   RETURN Returns a pointer to the julian day vector in the R time object,
   or NULL if there is an error.

   ALGORITHM This function calls time_data_pointer to get the data
   portion of the time or time span object, and then extracts the julian days
   pointer by calling functions from Rinternals.h and Rdefines.h.

   EXCEPTIONS 

   NOTE See also: time_format_pointer, time_zone_pointer, 
   time_ms_pointer, time_data_pointer

**********************************************************************/
SEXP time_julian_pointer( SEXP time_obj )
{

  SEXP data_pointer = time_data_pointer( time_obj );
  if( !data_pointer )
    return NULL;
  /* the julian days currently live in the first element of the data list */
  return( VECTOR_ELT(data_pointer, 0));
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME time_ms_pointer

   DESCRIPTION  Get a pointer to the millisecond portion of an R time object.

   ARGUMENTS
      IARG  time_obj  The R time or time span object

   RETURN Returns a pointer to the millisecond vector in the R time or 
   time span object, or NULL if there is an error.

   ALGORITHM This function calls time_data_pointer to get the data
   portion of the time object, and then extracts the milliseconds
   pointer by calling functions from Rinternals.h and Rdefines.h.

   EXCEPTIONS 

   NOTE See also: time_format_pointer, time_zone_pointer, 
   time_julian_pointer, time_data_pointer

**********************************************************************/
SEXP time_ms_pointer( SEXP time_obj )
{

  SEXP data_pointer = time_data_pointer( time_obj );
  if( !data_pointer )
    return NULL;
  /* the milliseconds  currently live in the 2nd element of the data list */
  return( VECTOR_ELT(data_pointer, 1));

}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME time_get_pieces

   DESCRIPTION  Get a pointers to desired pieces of an R time object; for
   the format, convert it to a new-style format.

   ARGUMENTS
      IARG  time_obj     The R time or time span object
      IARG  opt_obj      The R time options list, or NULL if not needed
      OARG  day_vec      Julian days data, or NULL if not desired
      OARG  ms_vec       milliseconds data, or NULL if not desired
      OARG  vec_length   length of data, or NULL if not desired
      OARG  new_format   new-style-output format, or NULL if not desired
      OARG  zone         the time zone string, or NULL if not desired
      OARG  opt_struct   options structure, or NULL if not desired or needed

   RETURN Returns 0 if an error occurs.  Otherwise, returns 1 if the 
   new_format output argument was NULL, and the string length needed
   to format times using the format if it was not NULL.

   IMPORTANT NOTE: If time_get_pieces returns 1, then it leaves two 
   SEXP objects it created unprotected.  The calling function is
   responsible for unprotecting these two objects.

   ALGORITHM This function returns pointers to the requested data
   of the R time or time span object, as C arrays; the various parts are 
   requested by passing in non-NULL pointers for the corresponding
   output arguments.  The parts are found as follows.  First, if 
   time_init has not been called, it is called.  Then the function
   checks the class of the input object, by calling checkClass. 
   The class must match the correct name of the time or time span class.
   Then the desired pieces are extracted, using the time_julian_pointer,
   time_ms_pointer, time_zone_pointer, and time_format_pointer
   functions, and the vector length is calculated using macros from
   Rinternals.h and Rdefines.h.  The new output format and output length, if requested, 
   are calculated using the new_out_format function, in conjunction
   with the time_opt_sizes function.  The options
   structure is calculated using the time_opt_parse function.

   EXCEPTIONS 

   NOTE    If the output format (and length) or the options structure 
   are desired, then the R options list must be passed in.  Otherwise, 
   it is not used.  The time span object has no time zone.
   \\
   \\
   See also: time_create_new

**********************************************************************/
int time_get_pieces( SEXP time_obj, SEXP opt_obj, 
		     Sint **day_vec, Sint **ms_vec,
		     Sint *vec_length, char **new_format,
		     char **zone, TIME_OPT_STRUCT *opt_struct )
{

  SEXP tmp, tmp1, tmp2;
  const char *old_format, *tmpzone;
  Sint tmplen;
  int zone_size, abb_size, full_size;

  if( !time_initialized )
    time_init();

  if( !time_obj || ( new_format && ( !opt_obj || !opt_struct )) ||
      ( opt_struct && !opt_obj ))
    return 0;

  if( day_vec )
  {
    /* get the julian days data from the object and the length */

    tmp = time_julian_pointer( time_obj );

    if(  (tmplen = length(tmp)) > 0 ){
      PROTECT(tmp1 = AS_INTEGER(tmp));
      *day_vec = INTEGER(tmp1);
    } else PROTECT(tmp1 = NEW_INTEGER(1));

    if( vec_length ) *vec_length = tmplen;
  }

  if( ms_vec )
  {
    /* get the millisecond data from the object and the length */

    tmp = time_ms_pointer( time_obj );

    if(  (tmplen = length(tmp)) > 0 ){
      PROTECT(tmp2 = AS_INTEGER(tmp));
      *ms_vec = INTEGER(tmp2);
    } else PROTECT(tmp1 = NEW_INTEGER(1));

    if( vec_length ) *vec_length = tmplen;

  }

  if( vec_length && !ms_vec && !day_vec )
  {
    /* get the length because we didn't get it above */

    tmp = time_ms_pointer( time_obj );
    if( !tmp ) /* something bad happened */{
      UNPROTECT(2);
      return 0;
    }
    *vec_length = length( tmp );
  }

  if( zone )
  {
    tmp = time_zone_pointer( time_obj );
    if( !tmp ){
      UNPROTECT(2);
      return 0;
    }

    if( length( tmp ) < 1 )
      *zone = (char *) DEFAULT_ZONE;
    else
    {
      tmpzone = CHAR( tmp );
      if( tmpzone ){
	*zone = acopy_string(tmpzone);
      } else{
	UNPROTECT(2);
	return 0;
      }
    }
  }

  if( new_format )
  {
    /* parse the options */

    if( !time_opt_parse( opt_obj, opt_struct ))
      return 0;
    /* create a new-style format string from the format part of obj */
    
    /* calculate the lengths of names */
    time_opt_sizes( *opt_struct, &abb_size, &full_size );

    if( zone && *zone )
      zone_size = strlen( *zone );
    else
    {
      tmp = time_zone_pointer( time_obj );
      if( !tmp ){
	UNPROTECT(2);
      	return 0;
      }

      if( length( tmp ) < 1 )
	zone_size = strlen( DEFAULT_ZONE );
      else
      {
	tmpzone = CHAR( tmp );
	if( tmpzone )
	  zone_size = strlen( tmpzone );
	else{
	  UNPROTECT(2);
	  return 0;
	}
      }
    }

    tmp = time_format_pointer( time_obj );

    if( length( tmp ) < 1 ) {
      tmplen = new_out_format( DEFAULT_OUT_FORMAT, new_format, abb_size, 
			       full_size, zone_size );
      if(!tmplen) UNPROTECT(2);
      return(tmplen );
    }

    old_format = CHAR( tmp );
    if( !old_format || !(old_format)) /* bad character */
      {
	UNPROTECT(2);
	return 0;
      }
    tmplen = new_out_format( old_format, new_format, abb_size, 
			     full_size, zone_size );
    if(!tmplen) UNPROTECT(2);
    return(tmplen );
  }

  if( opt_struct ){ /* parse the time options, if we didn't get a zone */
    tmplen = time_opt_parse( opt_obj, opt_struct );
    if(!tmplen) UNPROTECT(2);
    return(tmplen );
  }
 
  /* nothing else to do, but we haven't returned yet, so just return True */
  return 1;
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME time_create_new

   DESCRIPTION  Create a new R time vector object, and return pointers
   to the requested data.

   ARGUMENTS
      IARG  length       The desired length for the time vector object
      OARG  day_data     Julian days data, or NULL if not desired
      OARG  ms_data      milliseconds data, or NULL if not desired

   RETURN Returns the new time object, or NULL if an error occurs. 

   ALGORITHM This function initializes the time class if time_init has
   not already been called.  Then it calls the NEW macro from Rinternals.h and Rdefines.h to create a 
   new time object, and makes it have the desired length using Rinternals.h and Rdefines.h macros.
   Then it gets pointers to the requested data in the new object, as C 
   arrays, by calling the time_julian_pointer, time_ms_pointer,
   and various Rinternals.h and Rdefines.h functions; the desired parts are requested by passing 
   in non-NULL pointers for the corresponding output arguments.  

   EXCEPTIONS 

   NOTE See also: time_get_pieces

**********************************************************************/
SEXP time_create_new( Sint new_length, Sint **day_data, Sint **ms_data )
{

  SEXP ret, tmp, jd, ms;

  if( !time_initialized )
    time_init();

  /* create a new time object */
  PROTECT( time_class = MAKE_CLASS( TIME_CLASS_NAME )) ;
  PROTECT( ret = NEW_OBJECT(time_class));
  PROTECT( tmp = NEW_LIST(2) );

  /* make the data list parts the right length */
  SET_VECTOR_ELT(tmp, 0, NEW_INTEGER(new_length));
  SET_VECTOR_ELT(tmp, 1, NEW_INTEGER(new_length));
  SET_SLOT(ret, install("columns"), tmp);

  /* get the pointers for return */
  if(day_data) *day_data = INTEGER( VECTOR_ELT(tmp, 0) );
  if(ms_data) *ms_data = INTEGER( VECTOR_ELT(tmp, 1) );

  UNPROTECT(3);
  return ret;

}



/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME tspan_get_pieces

   DESCRIPTION  Get a pointers to desired pieces of an R time span object.

   ARGUMENTS
      IARG  time_obj      The R time span object
      OARG  day_vec       Julian days data, or NULL if not desired
      OARG  ms_vec        milliseconds data, or NULL if not desired
      OARG  vec_length    length of data, or NULL if not desired
      OARG  format_string output format, or NULL if not desired

   RETURN Returns 0 if an error occurs.  Otherwise, returns 1 if the 
   new_format output argument was NULL, and the string length needed
   to format time spans using the format if it was not NULL.

   ALGORITHM This function returns pointers to the requested data
   of the R time span object, as C arrays; the various parts are 
   requested by passing in non-NULL pointers for the corresponding
   output arguments.  The parts are found as follows.  First, if 
   time_init has not been called, it is called.  Then the function
   checks the class of the input object, by calling checkClass. 
   Then the desired pieces are extracted, using the time_julian_pointer,
   time_ms_pointer, and time_format_pointer
   functions, and the vector length is calculated using macros from
   Rinternals.h and Rdefines.h.  The length needed for output is calculated using the 
   tspan_output_length function.

   EXCEPTIONS 

   NOTE See also: tspan_create_new

**********************************************************************/
int tspan_get_pieces( SEXP time_obj, Sint **day_vec, Sint **ms_vec,
		      Sint *vec_length, char **format_string )
{
  int cl;
  SEXP tmp;
  const char *tmp_str;
  Sint tmplen;
  static const char *classes[] = {
    TSPAN_CLASS_NAME
  };

  if( !time_obj )
    return 0;

  if( !time_initialized )
    time_init();

  cl = checkClass( time_obj, classes, 1L );
  if(cl < 0)
    return(0);

  if( day_vec )
  {
    /* get the julian days data from the object and the length */

    tmp = time_julian_pointer( time_obj );
    *day_vec = INTEGER(tmp);

    if( !day_vec || ( (tmplen = length(tmp)) && !(*day_vec )))
      return 0;

    if( vec_length ) *vec_length = tmplen;
  }

  if( ms_vec )
  {
    /* get the millisecond data from the object and the length */

    tmp = time_ms_pointer( time_obj );
    *ms_vec = INTEGER(tmp);

    if( !ms_vec || ( (tmplen = length(tmp)) && !(*ms_vec )))
      return 0;

    if( vec_length ) *vec_length = tmplen;

  }

  if( vec_length && !ms_vec && !day_vec )
  {
    /* get the length because we didn't get it above */

    tmp = time_ms_pointer( time_obj );
    if( !tmp ) /* something bad happened */
      return 0;
    *vec_length = length( tmp );
  }

  if( format_string )
  {

    /* create a new-style format string from the format part of obj */
    
    tmp = time_format_pointer( time_obj );
    if( !tmp ) /* badness! */
      return 0;

    if( length( tmp ) < 1 ) 
      return 0;

    tmp_str = CHAR( tmp );
    if( !tmp_str || !(*tmp_str)) /* bad character */
      return 0;

    *format_string = acopy_string(tmp_str);

    return( tspan_output_length( tmp_str ));
  }

  /* nothing else to do, but we haven't returned yet, so just return True */
  return 1;
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME tspan_create_new

   DESCRIPTION  Create a new R time span vector object, and return pointers
   to the requested data.

   ARGUMENTS
      IARG  length       The desired length for the time span object
      OARG  day_data     Julian days data, or NULL if not desired
      OARG  ms_data      milliseconds data, or NULL if not desired

   RETURN Returns the new time span object, or NULL if an error occurs. 

   ALGORITHM This function initializes the time classes if time_init has
   not already been called.  Then it calls the NEW macro from Rinternals.h and Rdefines.h to create a 
   new time object, and makes it have the desired length using Rinternals.h and Rdefines.h macros.
   Then it gets pointers to the requested data in the new object, as C 
   arrays, by calling the time_julian_pointer, time_ms_pointer,
   and various Rinternals.h and Rdefines.h functions; the desired parts are requested by passing 
   in non-NULL pointers for the corresponding output arguments.  

   EXCEPTIONS 

   NOTE See also: time_get_pieces

**********************************************************************/
SEXP tspan_create_new( Sint new_length, Sint **day_data, Sint **ms_data )
{
  SEXP ret, tmp, jd, ms;
  if( !time_initialized )
    time_init();

  /* create a new time object */

  PROTECT( tspan_class = MAKE_CLASS( TSPAN_CLASS_NAME )) ;
  PROTECT( ret = NEW_OBJECT(tspan_class)) ;
  PROTECT( tmp = NEW_LIST(2) );

  /* make the data list parts the right length */
  SET_VECTOR_ELT(tmp, 0, NEW_INTEGER(new_length));
  SET_VECTOR_ELT(tmp, 1, NEW_INTEGER(new_length));
  SET_SLOT(ret, install("columns"), tmp);

  /* get the pointers for return */
  if(day_data) *day_data = INTEGER( VECTOR_ELT(tmp, 0) );
  if(ms_data) *ms_data = INTEGER( VECTOR_ELT(tmp, 1) );

  UNPROTECT(3);
  return ret;
}
