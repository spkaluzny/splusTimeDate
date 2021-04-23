/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
 *************************************************************************/

/*************************************************************************
 * This file contains C code utility functions for R time and time span objects
 * to convert them to/from strings, hours/minutes/seconds/ms,
 * month/day/year, numerics, and weekday numbers.
 *
 * The exported functions here were written to be called with the 
 * .Call interface of R.  They include (see documentation in func headers):

    SEXP time_to_string( SEXP time_vec, SEXP opt_list, 
                              SEXP zone_list );
    SEXP time_from_string( SEXP char_vec, SEXP format_string,
			        SEXP opt_list, SEXP zone_list );
    SEXP tspan_to_string( SEXP time_vec );
    SEXP tspan_from_string( SEXP char_vec, SEXP format_string );
    SEXP time_to_month_day_year( SEXP time_vec, 
                                      SEXP zone_list );
    SEXP time_from_month_day_year( SEXP month_vec, 
                                        SEXP day_vec, 
					SEXP year_vec );
    SEXP time_to_year_day( SEXP time_vec, SEXP zone_list );
    SEXP time_from_month_day_index( SEXP month, SEXP wkday, 
                                         SEXP index, SEXP year_vec );
    SEXP time_easter( SEXP year_vec );
    SEXP time_to_hour_min_sec( SEXP time_vec, SEXP zone_list );
    SEXP time_from_hour_min_sec( SEXP hour_vec, SEXP min_vec, 
	 			      SEXP sec_vec, SEXP ms_vec );
    SEXP time_to_numeric( SEXP time_vec );
    SEXP time_from_numeric( SEXP num_vec, SEXP ret_class );
    SEXP time_to_weekday( SEXP time_vec, SEXP zone_list );
    SEXP time_to_zone( SEXP time_vec, SEXP zone, 
			    SEXP zone_list );

*************************************************************************/

#include "timeFuns.h"
#include "zoneFuns.h"
#include <string.h>

/*********************************************************************
 * R-C  DOCUMENTATION ************************************************
 **********************************************************************
   NAME time_to_string

   DESCRIPTION  Convert a time object to character strings.  
   To be called from R as 
   \\
   {\tt 
    .Call("time_to_string", time.obj, time.opt, zone.list)
   }
   \\
   where TIMECLASS is replaced by the name of the time class.

   ARGUMENTS
      IARG  time_vec  The R time vector object
      IARG  opt_list  A list containing various options (see below)
      IARG  zone_list The list of R time zone objects

   RETURN Returns an R vector of character strings of the same 
   length as the input time vector.
   This function exits with the standard R error syntax if
   there is an error, such as the wrong type of input.

   ALGORITHM The time object is converted to a TIME_DATE_STRUCT using the 
   jms_to_struct function.  The time object's time zone is passed to the
   find_zone function to find the zone information, which is then used to 
   convert from GMT to local time using the GMT_to_zone function.  Then 
   this information and the time object's format
   string are used to convert to character strings, using the
   mdyt_format function.  If needed (depends on the format), the following
   components of the options list are used: month.name for the names of
   the months, month.abb for the month abbreviations, day.name and day.abb
   for the names and abbreviations of the weekdays (Sun - Sat ), 
   am.pm for printing strings for
   AM and PM times, and century (e.g. 1900) for the current century.

   EXCEPTIONS 

   NOTE See also: time_from_string


**********************************************************************/
SEXP time_to_string( SEXP time_vec, SEXP opt_list, SEXP zone_list )
{

  SEXP ret;
  char **new_format, *strbuf;
  Sint *in_days, *in_ms;
  Sint i, lng, string_length;
  int full_size, abb_size;
  TIME_DATE_STRUCT td;
  TIME_OPT_STRUCT  topt;
  TZONE_STRUCT *tzone;

  /* get the desired parts of the time and options objects */

  new_format = (char **) R_alloc(1L, sizeof(char *));
  string_length = time_get_pieces( time_vec, opt_list, &in_days, &in_ms, 
				   &lng, new_format, &td.zone, &topt );
 
  if( !string_length || ( lng && ( !in_days || !in_ms )) || 
      !new_format || !td.zone )
    error("invalid argument in C function time_to_string");

   tzone = find_zone( td.zone, zone_list );
  if( !tzone )
    error("unknown or unreadable time zone in C function time_to_string");

  time_opt_sizes( topt, &abb_size, &full_size );

  /* create return data vector */
  
  PROTECT( ret = NEW_STRING(lng) );
  if( !ret )
    error("problem allocating return vector in c function time_to_string");

  /* For each day/ms combo, convert day to month/day/year and then 
     format day/time into a string */

  strbuf = R_alloc( string_length + 1, sizeof(char) );
  for( i = 0; i < lng; i++ )
  {

    /* special case NA */
    /* and convert to month, day, year, hour, minute, second */
    /* and from that to a string */

    if(  in_days[i]== NA_INTEGER || 
	 in_ms[i]==NA_INTEGER ||
	!jms_to_struct( in_days[i], in_ms[i], &td ) ||
	!GMT_to_zone( &td, tzone ) ||
	!mdyt_format( td, *new_format, topt, strbuf ))
      SET_STRING_ELT(ret, i, NA_STRING);
    else
      SET_STRING_ELT(ret, i, mkChar(strbuf));
  }

  UNPROTECT(3); //1 + 2 from time_get_pieces
  return( ret );
}



/**********************************************************************
 * R-C  DOCUMENTATION ************************************************
 **********************************************************************
   NAME time_from_string

   DESCRIPTION  Read a time object from a character string vector.
   To be called from R as 
   \\
   {\tt 
    .Call("time_from_string", char.vec, format.str, time.opt, zone.list)
   }

   ARGUMENTS
      IARG  char_vec       The input character string vector
      IARG  format_string  The new- or old-style date/time input format string
      IARG  opt_list       A list containing various options (see below)
      IARG  zone_list      The list of time zones

   RETURN Returns an R time object the same 
   length as the input character string vector.
   This function exits with the standard R error syntax if
   there is an error, such as the wrong type of input.

   ALGORITHM The format string is used to read times, dates, and time
   zones from the input character string vector, using the mdyt_input
   function.  The actual time zone information is found using the 
   find_zone function. Then the calendar dates and clock times are converted 
   to GMT using the GMT_from_zone function, in conjunction with the
   julian_to_weekday and mdy_to_yday functions, and then to julian days and 
   milliseconds since midnight, using the julian_from_mdy and ms_from_hms
   functions.  The julian days and milliseconds are put into the 
   returned time object.  If needed (depends on the format), the following
   components of the options list are used: month.name for the names of
   the months, month.abb for the month abbreviations, day.name and day.abb
   for the names and abbreviations of the weekdays (Sun - Sat ), 
   am.pm for printing strings for
   AM and PM times, and century (e.g. 1900) for the current century.
   Also, the ``zone'' component of the options list is used as the 
   default zone of the times in the strings, if the input format
   does not specify to read the zone from the string.

   EXCEPTIONS 

   NOTE See also: time_to_string

**********************************************************************/
SEXP time_from_string( SEXP char_vec, SEXP format_string,
		       SEXP opt_list, SEXP zone_list )
{
  SEXP ret, in_data, col0;
  char **new_format;
  const char *in_format;
  char *pos;
  int len, j;
  Sint *jul_data, *ms_data, lng, i;
  TIME_DATE_STRUCT td;
  TIME_OPT_STRUCT  topt;
  TZONE_STRUCT *tzone;

  new_format = (char **) R_alloc(1L, sizeof(char*));
  jul_data = (Sint *) R_alloc(1L, sizeof(Sint *));
  ms_data = (Sint *) R_alloc(1L, sizeof(Sint *));

  /* extract input data */
  if(!isString(format_string) || (lng = length(format_string)) < 1)
    error("problem extracting data from format_string argument in c function time_from_string");
  if( lng > 1 )
    warning("only the first format string will be used -- ignoring the other %ld elements of second argument to c function time_from_string", lng -1);
  in_format = CHAR(STRING_ELT(format_string, 0));

  if(!isString(char_vec) || (lng = length(char_vec)) < 1)
    error("problem extracting data from input argument in c function time_from_string");
  in_data = char_vec;
  lng = length(in_data);

  *new_format = NULL;
  if( !new_format  ||
      !new_in_format( in_format, new_format )  ) 
    error("invalid format in c function time_from_string");
  

  if( !time_opt_parse( opt_list, &topt ))
    error("bad third argument to c function time_from_string");

  /* create output time object and find pointers for data*/

  PROTECT(ret = time_create_new( lng, &jul_data, &ms_data ));

  jul_data = INTEGER( VECTOR_ELT(GET_SLOT(ret, install("columns")), 0) );
  ms_data = INTEGER( VECTOR_ELT(GET_SLOT(ret, install("columns")), 1) );

  if( !ret || !jul_data || !ms_data )
    error("could not create new time object in c function time_from_string");

  /* go through and convert each string to times */
  for( i = 0; i < lng; i++ )
  {
    /* special case NA */
    /* convert from string to m/d/y/h/min/sec/ms */

    if( ( STRING_ELT(in_data,i) &&
	  !strcmp( CHAR(STRING_ELT(in_data,i)), "NA" )) ||
	!mdyt_input( CHAR(STRING_ELT(in_data,i)), *new_format, topt, &td ))
    { 
      /* error occurred -- put NA into return value */
      jul_data[i] = NA_INTEGER;
      ms_data[i] = NA_INTEGER;
      continue;
    }

    /* find the time zone object from the time zone string */

    td.daylight = 0;

    if( !td.zone )
      td.zone = topt.zone;
    else {

      /* If topt.zone (default time zone) has a slash, and the string
	 had a time zone in it, use that time zone to break ties in
	 ambiguous times around daylight savings boundaries. E.g. if
	 string had "EDT", and the default zone is "EST/EDT", then set
	 the default to daylight.  If there's no info, arbitrarily
	 assume it's not daylight, so we only have to check if td.zone
	 matches the part after the /, and set to daylight if so)
      */

      /* note that pos starts on the slash, and we want to match 1 past it */
      pos = strchr( topt.zone, '/' );
      len = strlen( td.zone );
      if( pos && len && strlen( pos ) == len + 1 ) { 
	td.daylight = 1;
	for( j = 0; j < len; j++ ) td.daylight = td.daylight && ( pos[j+1] == td.zone[j] );
      }
    }


    tzone = find_zone( td.zone, zone_list );
    if(!tzone)
      warning("%s%s\n", "Bad time zone ", td.zone);

    /* convert from m/d/y to julian day for use in weekday function */
    if( !tzone ||
	!julian_from_mdy( td, &(jul_data[i]) ))
    {
      /* error occurred -- put NA into return value */

      jul_data[i] = NA_INTEGER;
      ms_data[i]  = NA_INTEGER;
      continue;
    }

    td.weekday = julian_to_weekday( jul_data[i] );

    /* convert to GMT from local time */
    /* and then to julian and ms from time of day and date */
    if( !mdy_to_yday( &td ) ||
	!GMT_from_zone( &td, tzone ) ||
	!julian_from_mdy( td, &(jul_data[i]) ) ||
	!ms_from_hms( td, &(ms_data[i]) ))
    { 
      /* error occurred -- put NA into return value */

      jul_data[i] = NA_INTEGER;
      ms_data[i] = NA_INTEGER;
    }

  }

  col0 = VECTOR_ELT(GET_SLOT(ret, install("columns")), 0);

  UNPROTECT(1);
  return( ret );  
}


/**********************************************************************
 * R-C  DOCUMENTATION ************************************************
 **********************************************************************
   NAME tspan_to_string

   DESCRIPTION  Convert a time span object to character strings.  
   To be called from R as 
   \\
   {\tt 
    .Call("tspan_to_string", time.span.obj)
   }
   \\
   where TIMESPANCLASS is replaced by the name of the time span class.

   ARGUMENTS
      IARG  time_vec  The R time span object

   RETURN Returns an R vector of character strings of the same 
   length as the input time span vector.
   This function exits with the standard R error syntax if
   there is an error, such as the wrong type of input.

   ALGORITHM The time span object is converted to a string
   using the tspan_format function. 

   EXCEPTIONS 

   NOTE See also: tspan_from_string

**********************************************************************/
SEXP tspan_to_string( SEXP time_vec )
{

  SEXP ret;
  char **ret_data, *in_format, *strbuf;
  Sint *in_days, *in_ms;
  Sint i, lng, string_length; 

  /* get the desired parts of the time and options objects */

  string_length = tspan_get_pieces( time_vec, &in_days, &in_ms, 
				   &lng, &in_format );

  if( !string_length || ( lng && ( !in_days || !in_ms )) || 
      !in_format )
    error("Invalid argument in C function tspan_to_string");

  /* create return data vector */
  PROTECT(ret = NEW_STRING(lng));
  if(!ret)
    error( "Problem allocating return vector in c function tspan_to_string");

  /* For each day/ms combo, convert to a string */

  strbuf = R_alloc( string_length + 1, sizeof(char) );
  for( i = 0; i < lng; i++ )
  {

    /* special case NA */
    /* and convert to string */ 

    if(  in_days[i] == NA_INTEGER || 
	 in_ms[i] == NA_INTEGER ||
	!tspan_format( in_format, in_days[i], in_ms[i], strbuf ))
      SET_STRING_ELT(ret, i, NA_STRING);
    else
      SET_STRING_ELT(ret, i, mkChar(strbuf));
  }
  UNPROTECT(1);
  return( ret );
}



/**********************************************************************
 * R-C  DOCUMENTATION ************************************************
 **********************************************************************
   NAME tspan_from_string

   DESCRIPTION  Read a time span object from a character string vector.
   To be called from R as 
   \\
   {\tt 
    .Call("tspan_from_string", char.vec, format.str)
   }

   ARGUMENTS
      IARG  char_vec       The input character string vector
      IARG  format_string  The time span input format string

   RETURN Returns an R time span vector object the same 
   length as the input character string vector.
   This function exits with the standard R error syntax if
   there is an error, such as the wrong type of input.

   ALGORITHM The format string is used to read the time span
   from the input character string vector, using the tspan_input
   function.  

   EXCEPTIONS 

   NOTE See also: tspan_to_string

**********************************************************************/
SEXP tspan_from_string( SEXP char_vec, 
			SEXP format_string )
{
  SEXP ret;
  const char *in_data;
  const char *in_format;
  Sint i, lng;
  Sint *jul_data, *ms_data;

  if(!isString(format_string) || (lng = length(format_string)) < 1)
    error("problem extracting data from format_string argument in c function tspan_from_string");
  if( lng > 1 )
    warning("only the first format string will be used -- ignoring the other %ld elements of second argument to c function time_from_string", lng -1);
  in_format = CHAR(STRING_ELT(format_string, 0));

  if(!isString(char_vec) || (lng = length(char_vec)) < 1)
    error("problem extracting data from char_vec argument in c function tspan_from_string");

  /* create output time span object and find pointers for data*/

  PROTECT(ret = tspan_create_new( lng, &jul_data, &ms_data ));
  if( !(ret) || !jul_data || !ms_data )
    error( "Could not create new time span object in c function tspan_from_string");

  /* go through and convert each string to time span */
  for( i = 0; i < lng; i++ )
  {
    /* special case NA */
    /* convert from string to julian, ms */

    in_data = CHAR(STRING_ELT(char_vec, i));
    if( ( in_data && !strcmp( in_data, "NA" )) ||
	!tspan_input( in_data, in_format, &(jul_data[i]), &(ms_data[i] )))
    { 
      /* error occurred -- put NA into return value */
      jul_data[i] = NA_INTEGER;
      ms_data[i] = NA_INTEGER;
    }
  }

  UNPROTECT(1);
  return( ret );  
}


/**********************************************************************
 * R-C  DOCUMENTATION ************************************************
 **********************************************************************
   NAME time_to_month_day_year

   DESCRIPTION  Convert an R time object to vectors of months, days, 
   and years.  To be called from R as 
   \\
   {\tt 
    .Call("time_to_month_day_year", time.vec, zone.list)
   }
   where TIMECLASS is replaced by the name of the time class.

   ARGUMENTS
      IARG  time_vec  The R time vector object
      IARG  zone_list The list of R time zone objects

   RETURN Returns a list containing three vectors of integers, corresponding
   to the months, days, and years of the input time vector.
   This function exits with the standard R error syntax if
   there is an error, such as the wrong type of input.

   ALGORITHM The time object is converted to a TIME_DATE_STRUCT
   using the jms_to_struct function.  This structure is then 
   converted to the local time zone using the GMT_to_zone 
   functions, with the zone information found using the find_zone 
   function, and the months, days, and years are put into the 
   returned list.

   EXCEPTIONS 

   NOTE See also: time_from_month_day_year, time_to_hour_min_sec,
   time_to_weekday, time_to_year_day

**********************************************************************/
SEXP time_to_month_day_year( SEXP time_vec, 
			     SEXP zone_list )
{

  SEXP ret, m, d, y;
  Sint *in_days, *in_ms;
  Sint i, lng;
  Sint *month_data, *day_data, *year_data;
  TIME_DATE_STRUCT td;
  TZONE_STRUCT *tzone;

  /* get the desired parts of the time object */

  if( !time_get_pieces( time_vec, NULL, &in_days, &in_ms, &lng, 
			NULL, &(td.zone), NULL ) 
      || !in_days || !in_ms || !td.zone )
    error( "Invalid argument in C function time_to_month_day_year");

  tzone = find_zone( td.zone, zone_list );
  if( !tzone )
    error( "Unknown or unreadable time zone in C function time_to_month_day_year");

  /* create output data array */
  PROTECT(ret = NEW_LIST(3));
  PROTECT(m = NEW_INTEGER(lng));
  PROTECT(d = NEW_INTEGER(lng));
  PROTECT(y = NEW_INTEGER(lng));
  SET_VECTOR_ELT(ret, 0, m);
  SET_VECTOR_ELT(ret, 1, d);
  SET_VECTOR_ELT(ret, 2, y);
  month_data = INTEGER(m);
  day_data = INTEGER(d);
  year_data = INTEGER(y);

  if( !(ret) || !month_data || !day_data || !year_data)
    error( "Problem allocating return list in c function time_to_month_day_year");

  /* For each day, convert to month/day/year */ 

  for( i = 0; i < lng; i++ )
   {
    /* special case NA */
    /* and convert to month, day, year */

    if(  in_days[i] == NA_INTEGER || 
	 in_ms[i] == NA_INTEGER || 
	!jms_to_struct( in_days[i], in_ms[i], &td ) ||
	!GMT_to_zone( &td, tzone ))
    { 
      /* error occurred -- put NA into return value */
      month_data[i] = NA_INTEGER;
      day_data[i] = NA_INTEGER;
      year_data[i] = NA_INTEGER;
    } else {
      month_data[i] = td.month;
      day_data[i] = td.day;
      year_data[i] = td.year;
    }
  }

  UNPROTECT(6); //4+2 from time_get_pieces
  return( ret );
}


/**********************************************************************
 * R-C  DOCUMENTATION ************************************************
 **********************************************************************
   NAME time_from_month_day_year

   DESCRIPTION  Create an R time object from vectors of months, days, 
   and years. To be called from R as 
   \\
   {\tt 
   .Call("time_from_month_day_year", month.vec, day.vec, year.vec)
   }

   ARGUMENTS
      IARG  month_vec  An integer vector of months
      IARG  day_vec    An integer vector of days
      IARG  year_vec   An integer vector of years


   RETURN Returns an R time object whose dates correspond to the input
   and whose times are all midnight.
   This function exits with the standard R error syntax if
   there is an error, such as the wrong type of input.

   ALGORITHM The months, days, and years are converted to the time 
   object's julian day using the julian_from_mdy function; this julian
   day is put into the time object.  Since no times are given, time
   zones are not considered in making the calculation, and the time
   object's times of day are always midnight GMT.

   EXCEPTIONS 

   NOTE See also: time_to_month_day_year, time_from_hour_min_sec,
   time_to_zone, time_from_month_day_index

**********************************************************************/
SEXP time_from_month_day_year( SEXP month_vec, 
			       SEXP day_vec, 
			       SEXP year_vec )
{

  SEXP ret;
  Sint *in_months, *in_days, *in_years, *day_data, *ms_data;
  Sint i, lng, tmplng;
  TIME_DATE_STRUCT td;

  /* extract input data */

  
  if(!day_vec || (lng = length(day_vec))<1L ||
     !month_vec || length(month_vec) != lng ||
     !year_vec || length(year_vec) != lng){
    error( "Problem extracting same-length data in c function time_from_month_day_year"); 
  }

  
  in_days = INTEGER(day_vec);
  in_months = INTEGER(month_vec);
  in_years = INTEGER(year_vec);

  /* create output time object and find pointers for data*/

  PROTECT(ret = time_create_new( lng, &day_data, &ms_data ));
  if( !ret || !day_data || !ms_data )
    error( "Could not create new time object in c function time_from_month_day_year");

  /* for each month/day/year, convert to julian day and put into return obj*/

  for( i = 0; i < lng; i++ )
  {
    ms_data[i] = 0;

    td.month = in_months[i];
    td.day = in_days[i];
    td.year = in_years[i];

    if( in_days[i]== NA_INTEGER || in_months[i]== NA_INTEGER 
	|| in_years[i]==NA_INTEGER ||
	!julian_from_mdy( td,  &(day_data[i])))
    {
      day_data[i] = NA_INTEGER;
      ms_data[i] = NA_INTEGER;
    }
  }
  
  UNPROTECT(1);
  return( ret );
}


/**********************************************************************
 * R-C  DOCUMENTATION ************************************************
 **********************************************************************
   NAME time_to_year_day

   DESCRIPTION  Convert an R time object to vectors of the years and 
   year days (number of days elapsed in each year).  To be called from R as 
   \\
   {\tt 
    .Call("time_to_year_day", time.vec, zone.list)
   }
   where TIMECLASS is replaced by the name of the time class.

   ARGUMENTS
      IARG  time_vec  The R time vector object
      IARG  zone_list The list of R time zone objects

   RETURN Returns a list of two vectors of integers, corresponding
   to the years and year days of the input time vector.
   This function exits with the standard R error syntax if
   there is an error, such as the wrong type of input.

   ALGORITHM The time object is converted to a TIME_DATE_STRUCT using
   the jms_to_struct function. This is adjusted to the local time zone
   using the GMT_to_zone function, with the zone information found 
   using the find_zone function.  Then the day and yearday members 
   of the struct are put into the returned object.

   EXCEPTIONS 

   NOTE See also: time_to_month_day_year

**********************************************************************/
SEXP time_to_year_day( SEXP time_vec, SEXP zone_list )
{

  SEXP ret;
  Sint *in_days, *in_ms;
  Sint i, lng;
  Sint *day_data, *year_data;
  TIME_DATE_STRUCT td;
  TZONE_STRUCT *tzone;

  /* get the desired parts of the time object */

  if( !time_get_pieces( time_vec, NULL, &in_days, &in_ms, &lng, 
			NULL, &(td.zone), NULL ) 
      || !in_days || !in_ms || !td.zone )
    error( "Invalid argument in C function time_to_year_day");

  tzone = find_zone( td.zone, zone_list );
  if( !tzone )
    error( "Unknown or unreadable time zone in C function time_to_year_day");

  /* create output data array */
  PROTECT(ret = NEW_LIST(2));
  SET_VECTOR_ELT(ret, 0, NEW_INTEGER(lng));
  SET_VECTOR_ELT(ret, 1, NEW_INTEGER(lng));
  year_data = INTEGER(VECTOR_ELT(ret, 0));
  day_data = INTEGER(VECTOR_ELT(ret, 1));

  if( !(ret) || !year_data || !day_data)
    error( "Problem allocating return list in c function time_to_year_day");

  /* For each day, convert to month/day/year */ 
  /* and from that to year day */

  for( i = 0; i < lng; i++ )
  {
    /* special case NA */
    /* and convert to month, day, year */

    if(  in_days[i] == NA_INTEGER || 
	 in_ms[i] == NA_INTEGER || 
	!jms_to_struct( in_days[i], in_ms[i], &td ) || 
	!GMT_to_zone( &td, tzone ))
    { 
      /* error occurred -- put NA into return value */
      day_data[i] = NA_INTEGER;
      year_data[i] = NA_INTEGER;
    } else {
      day_data[i] = td.yearday;
      year_data[i] = td.year;
    }
  }

  UNPROTECT(3); //1+2 from time_get_pieces
  return( ret );
}

/**********************************************************************
 * R-C  DOCUMENTATION ************************************************
 **********************************************************************
   NAME time_from_month_day_index

   DESCRIPTION  Create an R time object from a vector of years, along
   with a month, day of week, and index number.
   Weekdays run from 0 for Sunday to 6 for Saturday.  Months run from 
   1 to 12.  An index of n indicates to find the nth occurrence of that 
   particular weekday in the month, and -1 says to find the last one.
   To be called from R as 
   \\
   {\tt 
   .Call("time_from_month_day_index", month, weekday, index, year.vec)
   }

   ARGUMENTS
      IARG  month      An integer for the month
      IARG  wkday      An integer for the weekday
      IARG  index      An integer for the index
      IARG  year_vec   An integer vector of years


   RETURN Returns an R time object whose dates correspond to the input
   and whose times are all midnight.  
   This function exits with the standard R error syntax if
   there is an error, such as the wrong type of input, and puts
   NA into the vector for any year in which the specified day does
   not exist.

   ALGORITHM The month, weekday, index, and years are converted to the time
   object's julian day using the julian_from_index function; this julian
   day is put into the time object.  Since no times are given, time
   zones are not considered in making the calculation, and the time
   object's times of day are always midnight GMT.

   EXCEPTIONS 

   NOTE See also: time_from_month_day_year, time_to_zone

**********************************************************************/
SEXP time_from_month_day_index( SEXP month, SEXP wkday, 
				SEXP index, SEXP year_vec )
{
  SEXP ret;
  Sint in_month, in_weekday, in_index, *in_years, *day_data, *ms_data;
  Sint i, lng, other_na;

  /* extract input data */

  if( !wkday || ( lng = length(wkday)) < 1 )
    error( "Problem extracting input data in c function time_from_month_day_index"); 
  if( lng > 1 )
    warning( "Only the first weekday will be used -- ignoring the other %ld elements of second argument to c function time_from_month_day_index", lng - 1);
  in_weekday = *INTEGER(wkday);

  if( !month || ( lng = length(month)) < 1 )
    error( "Problem extracting input data in c function time_from_month_day_index"); 
  if( lng > 1 )
    warning( "Only the first weekday will be used -- ignoring the other %ld elements of second argument to c function time_from_month_day_index", lng - 1);
  in_month = *INTEGER(month);


  if( !index || ( lng = length(index)) < 1 )
    error( "Problem extracting input data in c function time_from_month_day_index"); 
  if( lng > 1 )
    warning( "Only the first weekday will be used -- ignoring the other %ld elements of second argument to c function time_from_month_day_index", lng - 1);
  in_index = *INTEGER(index);

  if( !year_vec )
    error( "Problem extracting input data in c function time_from_month_day_index"); 
  lng = length(year_vec);
  in_years = INTEGER(year_vec);

  /* create output time object and find pointers for data*/

  PROTECT(ret = time_create_new( lng, &day_data, &ms_data ));
  if( !ret || !day_data || !ms_data )
    error( "Could not create new time object in c function time_from_month_day_year");

  /* for each year, convert to julian day and put into return obj*/

  other_na = (  in_month==NA_INTEGER ||
	        in_weekday==NA_INTEGER ||
	        in_index==NA_INTEGER);
  
  for( i = 0; i < lng; i++ )
   {
     ms_data[i] = 0;

     if( other_na ||  in_years[i] == NA_INTEGER ||
	 !julian_from_index( in_month, in_weekday, in_index, 
			     in_years[i],  &(day_data[i])))
       {
	 day_data[i] = NA_INTEGER;
	 ms_data[i] = NA_INTEGER;
       }
   }
  
  UNPROTECT(1);
  return( ret );
}

/**********************************************************************
 * R-C  DOCUMENTATION ************************************************
 **********************************************************************
   NAME time_easter

   DESCRIPTION  Calculate when Easter falls in the given years.
   To be called from R as 
   \\
   {\tt 
   .Call("time_easter", year.vec)
   }

   ARGUMENTS
      IARG  year_vec   An integer vector of years


   RETURN Returns an R time object whose dates correspond to Easter
   in the given years and whose times are all midnight.
   This function exits with the standard R error syntax if
   there is an error, such as the wrong type of input.

   ALGORITHM Easter's date is calculated in the julian_easter
   function, and this is put into the time object.  Since no times are 
   given, time zones are not considered in making the calculation, and 
   the time object's times of day are always midnight GMT.

   EXCEPTIONS 

   NOTE See also: time_to_zone

**********************************************************************/
SEXP time_easter( SEXP year_vec )
{

  SEXP ret;
  Sint *in_years, *day_data, *ms_data;
  Sint i, lng;

  /* extract input data */

  if( !year_vec)
    error( "Problem extracting data in c function time_easter");
  lng = length(year_vec);
  in_years = INTEGER(year_vec);

  /* create output time object and find pointers for data*/

  PROTECT(ret = time_create_new( lng, &day_data, &ms_data ));
  if( !ret || !day_data || !ms_data )
    error( "Could not create new time object in c function time_easter");

  /* for each year, convert to julian day and put into return obj*/

  for( i = 0; i < lng; i++ )
  {
    ms_data[i] = 0;

    if(  in_years[i] == NA_INTEGER ||
	!julian_easter( in_years[i],  &(day_data[i])))
    {
      day_data[i] = NA_INTEGER;
      ms_data[i] = NA_INTEGER;
    }
  }
  
  UNPROTECT(1);
  return( ret );
}

/**********************************************************************
 * R-C  DOCUMENTATION ************************************************
 **********************************************************************
   NAME time_to_hour_min_sec

   DESCRIPTION  Convert an R time object to vectors of time-of-day
   hours, minutes, seconds, and milliseconds.
   To be called from R as 
   \\
   {\tt 
   .Call("time_to_hour_min_sec", time.vec, zone.list)
   }
   where TIMECLASS is replaced by the name of the time class.

   ARGUMENTS
      IARG  time_vec  The R time vector object
      IARG  zone_list The list of R time zone objects

   RETURN Returns a list containing four vectors of integers, corresponding
   to the hours, minutes, seconds, and milliseconds of the input time vector.
   This function exits with the standard R error syntax if
   there is an error, such as the wrong type of input.

   ALGORITHM The time object is converted to a TIME_DATE_STRUCT 
   using the jms_to_struct function. Then the find_zone function 
   is used to find the actual time zone information, which is then used to 
   convert from GMT to local time using the GMT_to_zone function.  The
   hours, minutes, and seconds from the structure are then put into
   the returned list. 

   EXCEPTIONS 

   NOTE See also: time_to_month_day_year, time_from_hour_min_sec

**********************************************************************/
SEXP time_to_hour_min_sec( SEXP time_vec, 
			   SEXP zone_list )
{

  SEXP ret;
  Sint *in_ms, *in_days;
  Sint i, lng;
  Sint *hour_data, *min_data, *sec_data, *ms_data;
  TIME_DATE_STRUCT td;
  TZONE_STRUCT *tzone;

  /* get the desired parts of the time object */

  if( !time_get_pieces( time_vec, NULL, &in_days, &in_ms, &lng, 
			NULL, &(td.zone), NULL ) || !in_ms || !in_days )
    error( "Invalid argument in C function time_to_hour_min_sec");

  tzone = find_zone( td.zone, zone_list );
  if( !tzone )
    error( "Unknown or unreadable time zone in C function time_to_hour_min_sec");

  /* create output data array */
  PROTECT(ret = NEW_LIST(4));
  if( !ret )
    error( "Problem allocating return list in c function time_to_hour_min_sec");
  SET_VECTOR_ELT(ret, 0, PROTECT(NEW_INTEGER(lng)));
  SET_VECTOR_ELT(ret, 1, PROTECT(NEW_INTEGER(lng)));
  SET_VECTOR_ELT(ret, 2, PROTECT(NEW_INTEGER(lng)));
  SET_VECTOR_ELT(ret, 3, PROTECT(NEW_INTEGER(lng)));
  hour_data = INTEGER(VECTOR_ELT(ret, 0));
  min_data = INTEGER(VECTOR_ELT(ret, 1));
  sec_data = INTEGER(VECTOR_ELT(ret, 2));
  ms_data = INTEGER(VECTOR_ELT(ret, 3));

  if(!hour_data || !min_data || !sec_data || !ms_data)
    error( "Problem allocating return list in c function time_to_hour_min_sec");


  /* for each time, convert to hour/min/sec/ms */

  for( i = 0; i < lng; i++ )
  {

    /* special case NA */
    /* and convert */

    if(  in_ms[i]== NA_INTEGER || 
	 in_days[i]== NA_INTEGER || 
	!jms_to_struct( in_days[i], in_ms[i], &td ) ||
	!GMT_to_zone( &td, tzone ))
    { 
      /* error occurred -- put NA into return value */
      hour_data[i] = NA_INTEGER;
      min_data[i] = NA_INTEGER;
      sec_data[i] = NA_INTEGER;
      ms_data[i] = NA_INTEGER;
    } else {
      hour_data[i] = td.hour;
      min_data[i] = td.minute;
      sec_data[i] = td.second;
      ms_data[i] = td.ms;
    }
  }

  UNPROTECT(7); //5+2 from time_get_pieces
  return( ret );
}


/**********************************************************************
 * R-C  DOCUMENTATION ************************************************
 **********************************************************************
   NAME time_from_hour_min_sec

   DESCRIPTION  Convert vectors of time-of-day
   hours, minutes, seconds, and milliseconds to an R time object.
   To be called from R as 
   \\
   {\tt
   .Call("time_from_hour_min_sec", hour.vec, min.vec, sec.vec, ms.vec)
   }

   ARGUMENTS
      IARG  hour_vec  The hours of the day vector
      IARG  min_vec   The minutes of the day vector
      IARG  sec_vec   The seconds of the day vector
      IARG  ms_vec    The milliseconds of the day vector

   RETURN Returns an R time object whose time portion corresponds to
   the input vector, and whose date is the Julian day 0 (1/1/1960).
   This function exits with the standard R error syntax if
   there is an error, such as the wrong type of input.

   ALGORITHM The hours, minutes, seconds, and milliseconds of the days
   are converted to milliseconds since midnight using the 
   ms_from_hms function, and these values are put into the time object.
   Time zones are not considered, since there are no dates (which
   would be needed to figure out daylight savings time).

   EXCEPTIONS 

   NOTE See also: time_from_month_day_year, time_to_hour_min_sec, 
   time_to_zone

**********************************************************************/
SEXP time_from_hour_min_sec( SEXP hour_vec, 
			     SEXP min_vec, 
			     SEXP sec_vec, SEXP ms_vec )
{

  SEXP ret;
  Sint *in_hours, *in_mins, *in_secs, *in_ms, *day_data, *ms_data;
  Sint i, lng, tmplng;
  TIME_DATE_STRUCT td;

  /* extract input data */ 

  in_hours = INTEGER(hour_vec);
  in_mins = INTEGER(min_vec);
  in_secs = INTEGER(sec_vec);
  in_ms = INTEGER(ms_vec);
  lng = length(hour_vec);
  if( !in_hours || !in_mins || length(min_vec)!=lng ||
      !in_secs || length(sec_vec)!=lng ||
      !in_ms || length(ms_vec)!=lng)
    error( "Problem extracting input data in c function time_from_hour_min_sec");

  /* create output time object and find pointers for data*/

  PROTECT(ret = time_create_new( lng, &day_data, &ms_data ));
  if( !ret || !day_data || !ms_data )
    error( "Could not create new time object in c function time_from_hour_min_sec");

  /* for each h/m/s/ms, convert to julian day */

  for( i = 0; i < lng; i++ )
  {
    day_data[i] = 0;
    td.hour = in_hours[i];
    td.minute = in_mins[i];
    td.second = in_secs[i];
    td.ms = in_ms[i];

    if(  in_hours[i] == NA_INTEGER || 
	 in_mins[i] == NA_INTEGER ||
	 in_secs[i] == NA_INTEGER ||
	 in_ms[i] == NA_INTEGER ||
	!ms_from_hms( td, &(ms_data[i])))
    {
      day_data[i] = NA_INTEGER;
      ms_data[i] = NA_INTEGER;
    }
  }
  
  UNPROTECT(1);
  return( ret );
}


/**********************************************************************
 * R-C  DOCUMENTATION ************************************************
 **********************************************************************
   NAME time_to_numeric

   DESCRIPTION  Convert an R time or time span object to a numeric vector.
   To be called from R as 
   \\
   {\tt 
   .Call("time_to_numeric", time.vec ).
   }

   ARGUMENTS
      IARG  time_vec  The R time or time span vector object

   RETURN Returns a numeric vector of the same length as the input
   vector. 

   ALGORITHM  Each time/date or span in the input object is represented 
   by a number in the returned numeric vector whose integer part 
   (or floor for negative numbers) is the Julian day (or number of 
   spanned days for time span).
   A fractional part given by the fraction of the day 
   corresponding to the number of milliseconds, calculated using 
   function ms_to_fraction, is added to the integer part.  
   This is always in GMT for times.

   EXCEPTIONS 

   NOTE See also: time_from_numeric

**********************************************************************/
SEXP time_to_numeric( SEXP time_vec )
{

  SEXP ret;
  double *ret_data;
  Sint *in_days, *in_ms;
  Sint i, lng;

  /* get the desired parts of the time object */

  if( !time_get_pieces( time_vec, NULL, &in_days, &in_ms, &lng, NULL, 
			NULL, NULL ))
    error( "Invalid argument in C function time_to_numeric");

  /* create return data vector */

  PROTECT(ret = NEW_NUMERIC(lng));
  if( !ret)
    error( "Problem allocating return vector in c function time_to_numeric");
  ret_data = REAL(ret);

  /* go through input and convert to numeric */
  for( i = 0; i < lng; i++ )
  {
    /* check for NA and convert */
    if(  in_days[i] == NA_INTEGER || 
	 in_ms[i] == NA_INTEGER)
    {
      ret_data[i] = NA_REAL;
      continue;
    }
    
    if( in_ms[i] >= 0 )
    { 
      if( !ms_to_fraction( in_ms[i], &(ret_data[i]) ))
      {
	ret_data[i] = NA_REAL;
	continue;
      }
    } else {
      /* ms_to_fraction wants ms > 0 */
      if( !ms_to_fraction( -in_ms[i], &(ret_data[i])))
      {
	ret_data[i] = NA_REAL;
	continue;
      }
      ret_data[i] *= -1;
    }
    ret_data[i] += in_days[i];
  }

  UNPROTECT(3); //1+2 from time_get_pieces
  return ret;
}

/**********************************************************************
 * R-C  DOCUMENTATION ************************************************
 **********************************************************************
   NAME time_from_numeric

   DESCRIPTION  Convert a numeric vector to an R time or time span
   object.  To be called from R as 
   \\
   {\tt 
   .Call("time_from_numeric", num.vec, ret.class)
   }

   ARGUMENTS
      IARG  num_vec   The R numeric vector
      IARG  ret_class The class to return, as a character vector

   RETURN Returns an R time or time span object (as requested) 
   of the same length as the input numeric vector. 

   ALGORITHM  Each number in the numeric vector is converted to a time/date
   in the time object by taking the integer part (or floor for negative
   numbers) to be the Julian day, and the fractional part (defined to be the
   number minus the calculated integer part) to be the fraction of the day.
   The fraction is converted to the number of milliseconds using 
   function ms_from_fraction, and this number and the Julian day number
   are put into the time object, after adjusting with adjust_span
   or adjust_time. No special time zones or formats are 
   put on the returned object.

   EXCEPTIONS 

   NOTE See also: time_to_numeric

**********************************************************************/
SEXP time_from_numeric( SEXP num_vec, SEXP ret_class )
{

  SEXP ret;
  double *in_num;
  Sint i, lng;
  int is_span;
  Sint *jul_data, *ms_data;
  const char *in_class;

  /* extract input data */

  if(!isString(ret_class) || ((lng = length(ret_class)) < 1))
    error( "Problem extracting input in c function time_from_numeric");
  in_class = CHAR(STRING_ELT(ret_class, 0));
  if( !in_class || 
      !(in_num = REAL(num_vec)))
    error( "Problem extracting input in c function time_from_numeric");

  lng = length(num_vec);
  /* create output object and find pointers for data*/

  is_span = 1;
  if( !strcmp( in_class, TIME_CLASS_NAME ))
  {
    is_span = 0;
    PROTECT(ret = time_create_new( lng, &jul_data, &ms_data ));
  }
  else if( !strcmp( in_class, TSPAN_CLASS_NAME ))
    PROTECT(ret = tspan_create_new( lng, &jul_data, &ms_data ));
  else
    error( "Unknown class argument in C function time_from_numeric");

  if( !ret || !jul_data || !ms_data )
    error( "Could not create return object in C function time_from_numeric");

  /* go through and convert each number to times */
  for( i = 0; i < lng; i++ )
  {
    /* check for NA and convert: floor becomes julian day, frac becomes ms */
    if( ISNA( in_num[i]) )
    {
      jul_data[i] = NA_INTEGER;
      ms_data[i] = NA_INTEGER;
      continue;
    }
    jul_data[i] = (Sint) floor( in_num[i] );

    if( !ms_from_fraction( in_num[i] - jul_data[i], &(ms_data[i])) ||
	( is_span && !adjust_span( &(jul_data[i]), &(ms_data[i] ))) ||
	( !is_span && !adjust_time( &(jul_data[i]), &(ms_data[i] ))))
    {
      jul_data[i] = NA_INTEGER;
      ms_data[i] = NA_INTEGER;
      continue;
    }
  }

  UNPROTECT(1);
  return ret;
}


/**********************************************************************
 * R-C  DOCUMENTATION ************************************************
 **********************************************************************
   NAME time_to_weekday

   DESCRIPTION  Find the weekday numbers of an R time vector.
   To be called from R as 
   \\
   {\tt 
   .Call("time_to_weekday", time.vec, zone.list)
   }
   where TIMECLASS is replaced by the name of the time class.

   ARGUMENTS
      IARG  time_vec  The R time vector object
      IARG  zone_list The list of R time zone objects

   RETURN Returns an integer vector of the same length as the input time
   vector, containing the weekday numbers of the dates.  0 is Sunday,
   and 6 is Saturday.

   ALGORITHM The time object is converted to a TIME_DATE_STRUCT 
   using the jms_to_struct function. Then the find_zone function 
   is used to find the actual time zone information, which is then used to 
   convert from GMT to local time using the GMT_to_zone function.  The
   weekday number from the struct is then put into the return value.

   EXCEPTIONS 

   NOTE See also: time_to_month_day_year

**********************************************************************/
SEXP time_to_weekday( SEXP time_vec, SEXP zone_list )
{

  SEXP ret;
  Sint *in_days, *in_ms, *ret_data;
  Sint i, lng;
  TIME_DATE_STRUCT td;
  TZONE_STRUCT *tzone;
  
  /* get the desired parts of the time object */

  if( !time_get_pieces( time_vec, NULL, &in_days, &in_ms, &lng, NULL, 
			&(td.zone), NULL ) 
      || !in_days || !in_ms || !td.zone )
    error( "Invalid argument in C function time_toweekday");

  tzone = find_zone( td.zone, zone_list );
  if( !tzone )
    error( "Unknown or unreadable time zone in C function time_to_weekday");

  /* create return data array */
  PROTECT(ret = NEW_INTEGER(lng));
  if( !ret)
    error( "Problem allocating return vector in c function time_to_weekday");
  ret_data = INTEGER(ret);

  /* for each julian day, calculate weekday */

  for( i = 0; i < lng; i++ )
  {
    if(  in_days[i] == NA_INTEGER || 
	 in_ms[i] == NA_INTEGER ||
	!jms_to_struct( in_days[i], in_ms[i], &td ) ||
	!GMT_to_zone( &td, tzone ))
      ret_data[i] = NA_INTEGER;
    else
      ret_data[i] = td.weekday;
  }

  UNPROTECT(3); //1+2 from time_get_pieces
  return( ret );
}



/**********************************************************************
 * R-C  DOCUMENTATION ************************************************
 **********************************************************************
   NAME time_to_zone

   DESCRIPTION  Convert the time zone of a time object, for use after
   time_from_month_day_year and other functions which explicitly do not 
   take zone into account.
   To be called from R as 
   \\
   {\tt 
   .Call("time_to_zone", time_obj, zone, zone.list)
   }
   where TIMECLASS is replaced by the name of the time class.

   ARGUMENTS
      IARG  time_vec  A time vector object
      IARG  zone      Name of the desired time zone
      IARG  zone_list The list of R time zone objects

   RETURN Returns an R time object, converting it to the local zone.

   ALGORITHM This function treats the input as if it had been created
   from months, days, years, hours, minutes, seconds, and milliseconds
   without use of time zones (e.g.\ using time_from_month_day_year or
   reading from a zone-less string with default zone GMT) 
   and afterwards the user wanted to say that
   the given input was really not in GMT but a local time zone. So, it
   converts the input times to TIME_DATE_STRUCT using the 
   jms_to_struct function, converts that 
   from local zone to GMT, using the find_zone function to get the
   zone information and the GMT_from_zone function to do the conversion,
   and then stores the result in a new time object, converting back to
   julian/ms using julian_from_mdy and ms_from_hms.

   EXCEPTIONS 

   NOTE The zone and format slots on the time object are not set
   by this function.
   \\
   \\
   See also: time_from_hour_min_sec, time_from_month_day_year

**********************************************************************/
SEXP time_to_zone( SEXP time_vec, SEXP zone, 
		   SEXP zone_list )
{

  SEXP ret;
  Sint lng, i;
  Sint *in_days, *in_ms, *jul_data, *ms_data;
  const char *zonestr;
  TIME_DATE_STRUCT td;
  TZONE_STRUCT *tzone;

  /* get the desired info from the input */
  if(!isString(zone) || (lng = length(zone) < 1))
    error( "Problem extracting input in c function time_to_zone");
  zonestr = CHAR(STRING_ELT(zone, 0));
  if(!zonestr )
    error( "Problem extracting data in c function time_to_zone"); 

  if( lng > 1 )
    warning( "Only the first time zone will be used -- ignoring the other %ld elements of second argument to c function time_to zone", lng - 1);

  /* convert to RWZone object */
  tzone = find_zone( zonestr, zone_list );
  if( !tzone )
    error( "Unknown or unreadable time zone in C function time_to_zone");

  /* then get the days/ms from the time vector */
  if( !time_get_pieces( time_vec, NULL, &in_days, &in_ms, &lng, 
			NULL, NULL, NULL ) 
      || !in_days || !in_ms )
    error( "invalid argument in C function time_to_zone");

  /* create output time object and find pointers for data*/

  PROTECT(ret = time_create_new( lng, &jul_data, &ms_data ));
  if( !ret || !jul_data || !ms_data )
    error( "could not create new time object in c function time_to_zone");

  /* go through and convert each time to correct zone */
  for( i = 0; i < lng; i++ )
  {
    /* special case NA */

    td.daylight = 0;

    if(  in_days[i] == NA_INTEGER || 
	 in_ms[i] == NA_INTEGER || 
	!jms_to_struct( in_days[i], in_ms[i], &td ) ||
	!GMT_from_zone( &td, tzone ) ||
	!julian_from_mdy( td, &(jul_data[i]) ) ||
	!ms_from_hms( td, &(ms_data[i]) ))
    { 
      /* error occurred -- put NA into return value */
      jul_data[i] = NA_INTEGER;
      ms_data[i] = NA_INTEGER;
    }
  }

  UNPROTECT(3); //1+2 from time_get_pieces
  return( ret );  

}

