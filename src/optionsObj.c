/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
 *************************************************************************/

/*************************************************************************
 *
 * It contains C code utility functions for dealing with R
 * objects containing defaults for printing/reading time objects.
 *
 * See optionsObj.h for a more compact listing of the included functions.
 * They were written as auxiliary functions for R code
 * dealing with times and dates.
 *
 *************************************************************************/


#include "optionsObj.h"

static char GMT_STRING[] = "GMT";

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME time_opt_parse

   DESCRIPTION  Parse an R object containing a time options list.

   ARGUMENTS
      IARG  in_obj      The R time options list
      OARG  out_struct  The parsed list of options

   RETURN Returns 1/0 for success/failure.

   ALGORITHM This function extracts pointers to named components
   of the list, using functions from the Rinternals.h and Rdefines.h header, and puts them 
   (or NULL if they do not exist) into the output structure.  It
   calls the checkClass function to verify the list and each
   component have the correct classes.  The components are: month.name 
   and month.abb, character vectors of length 12 with the names and 
   abbreviations of the months; day.name and day.abb, character
   vectors of length 7 with the names and abbreviations for the days;
   am.pm, a character vector of length 2 usually containing 
   {\tt c( "AM", "PM" )}; century, an integer (e.g. 1900) indicating
   the current century; and zone, a character string with the
   default time zone.  All but zone are used in converting 
   time objects to strings (depending on the format string);
   all but the abbreviations are used in converting strings to
   time objects.

   EXCEPTIONS 

   NOTE See also: time_opt_sizes

**********************************************************************/
int time_opt_parse( SEXP in_obj, TIME_OPT_STRUCT *out_struct )
{

  SEXP tmp_data;
  char **tmp_char;
  Sint *tmp_long;
  int i;

  if( !in_obj || !out_struct )
    return 0;

  out_struct->month_names = out_struct->month_abbs = 
    out_struct->day_names = out_struct->day_abbs = out_struct->am_pm = NULL;
  out_struct->zone = NULL;

  if( !isNewList( in_obj )){
    error("Input options not a list");
    return 0;
  }

  /* find desired pieces of the input object and put pointers
     to them into the output struct */

  tmp_data = getListElement( in_obj, "month.name");
  tmp_char = (char**) R_alloc(12L, sizeof(char*));

  if( tmp_data && ( length( tmp_data ) == 12  ) && 
      isString( tmp_data )){
    for(i=0; i<12; i++){
      tmp_char[i] = R_alloc(length(STRING_ELT(tmp_data, i)), sizeof(char));
      strcpy(tmp_char[i], CHAR(STRING_ELT(tmp_data, i)));
      //tmp_char[i] = acopy_string(CHAR(STRING_ELT(tmp_data, i)));
    }
    out_struct->month_names = tmp_char;
  } else
    out_struct->month_names = NULL;

  tmp_char = (char**) R_alloc(12L, sizeof(char*));
  tmp_data = getListElement( in_obj, "month.abb");
  if(tmp_data && ( length( tmp_data ) == 12  ) && 
      isString( tmp_data )){
    for(i=0; i<12; i++){
      tmp_char[i] = R_alloc(length(STRING_ELT(tmp_data, i)), sizeof(char));
      strcpy(tmp_char[i], CHAR(STRING_ELT(tmp_data, i)));
    }
    out_struct->month_abbs = tmp_char;
  } else
    out_struct->month_abbs = NULL;

  tmp_char = (char**) R_alloc(7L, sizeof(char*));
  tmp_data = getListElement( in_obj, "day.name");
  if(tmp_data && ( length( tmp_data ) == 7  ) && 
      isString( tmp_data )){
    for(i=0; i<7; i++){
      tmp_char[i] = R_alloc(length(STRING_ELT(tmp_data, i)), sizeof(char));
      strcpy(tmp_char[i], CHAR(STRING_ELT(tmp_data, i)));
    }
    out_struct->day_names = tmp_char;
  } else
    out_struct->day_names = NULL;

  tmp_char = (char**) R_alloc(7L, sizeof(char*));
  tmp_data = getListElement( in_obj, "day.abb");
  if(tmp_data && ( length( tmp_data ) == 7  ) && 
      isString( tmp_data )){
    for(i=0; i<7; i++){
      tmp_char[i] = R_alloc(length(STRING_ELT(tmp_data, i)), sizeof(char));
      strcpy(tmp_char[i], CHAR(STRING_ELT(tmp_data, i)));
    }
    out_struct->day_abbs = tmp_char;
  } else
    out_struct->day_abbs = NULL;

  tmp_char = (char**) R_alloc(2L, sizeof(char*));
  tmp_data = getListElement( in_obj, "am.pm");
  if(tmp_data && ( length( tmp_data ) == 2  ) && 
      isString( tmp_data )){
    for(i=0; i<2; i++){
      tmp_char[i] = R_alloc(length(STRING_ELT(tmp_data, i)), sizeof(char));
      strcpy(tmp_char[i], CHAR(STRING_ELT(tmp_data, i)));
    }
    out_struct->am_pm = tmp_char;
  } else
    out_struct->am_pm = NULL;

  tmp_char = (char**) R_alloc(1L, sizeof(char*));
  tmp_data = getListElement( in_obj, "zone");
  if(tmp_data && ( length( tmp_data ) == 1  ) && 
      isString( tmp_data )){
    tmp_char[0] = R_alloc(length(STRING_ELT(tmp_data, 0)), sizeof(char));
    strcpy(tmp_char[0], CHAR(STRING_ELT(tmp_data, 0)));
    out_struct->zone = tmp_char[0];
  } else
    out_struct->zone = GMT_STRING;


  tmp_long = (Sint*) R_alloc(1L, sizeof(Sint));
  PROTECT(tmp_data = (SEXP) AS_INTEGER( getListElement( in_obj, "century") ));
  if(tmp_data &&  length( tmp_data ) > 0  && 
     isInteger( tmp_data )){
    tmp_long = INTEGER(tmp_data);
    out_struct->century = tmp_long[0];
  } else {
    warning("invalid time.century option: setting to 0");
    out_struct->century = 0;
  }
 
  UNPROTECT(1);
  return 1;
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME time_opt_sizes

   DESCRIPTION Find the largest sizes of full names and abbreviations
   in a time options structure

   ARGUMENTS
     IARG  opt_struct  The time options structure
     OARG  abb_size    The largest size of abbreviations
     OARG  full_size   The largest size of full names

   RETURN nothing.

   ALGORITHM This function finds the largest size of full day/month
   names in the options structure and returns it as full_size, 
   and the largest size of day/month abbreviations or AM/PM strings 
   in the options structure, and returns it as
   abb_size.  The sizes are used in calculating the maximum size
   needed to format times as strings. 

   EXCEPTIONS 

   NOTE See also: time_opt_parse, new_out_format

**********************************************************************/
void time_opt_sizes( TIME_OPT_STRUCT opt_struct, int *abb_size, 
		     int *full_size )
{
  int i, tmplen;

  if( !abb_size || !full_size )
    return;

  *abb_size = *full_size = 0;

  /* check sizes of month names */
  if( opt_struct.month_names )
    for( i = 0; i < 12; i++ )
      if( opt_struct.month_names[i] && 
	  ((tmplen = strlen( opt_struct.month_names[i] )) > *full_size ))
	*full_size = tmplen;

  /* check sizes of day names */
  if( opt_struct.day_names )
    for( i = 0; i < 7; i++ )
      if( opt_struct.day_names[i] && 
	  ((tmplen = strlen( opt_struct.day_names[i] )) > *full_size ))
	*full_size = tmplen;

  /* check sizes of month abbrevs */
  if( opt_struct.month_abbs )
    for( i = 0; i < 12; i++ )
      if( opt_struct.month_abbs[i] && 
	  ((tmplen = strlen( opt_struct.month_abbs[i] )) > *abb_size ))
	*abb_size = tmplen;

  /* check sizes of day abbrevs */
  if( opt_struct.day_abbs )
    for( i = 0; i < 7; i++ )
      if( opt_struct.day_abbs[i] && 
	  ((tmplen = strlen( opt_struct.day_abbs[i] )) > *abb_size ))
	*abb_size = tmplen;
  
  /* check sizes of am/pm */
  if( opt_struct.am_pm )
    for( i = 0; i < 2; i++ )
      if( opt_struct.am_pm[i] && 
	  ((tmplen = strlen( opt_struct.am_pm[i] )) > *abb_size ))
	*abb_size = tmplen;

}

