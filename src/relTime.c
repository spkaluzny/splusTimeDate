/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
 *************************************************************************/

/*************************************************************************
 *
 * It contains C code utility functions for relative time addition.
 *  
 * See relTime.h for a more compact listing of the included functions.
 * They were written as auxiliary functions for R code
 * dealing with times and dates.
 *
 *************************************************************************/

#include "relTime.h"


/* enum for rel time codes, for local use only */

typedef enum rt_code
{
  RT_ERROR,
  RT_MS,
  RT_SEC,
  RT_MIN,
  RT_HR,
  RT_DAY,
  RT_WKD,
  RT_BIZ,
  RT_SUN,
  RT_MON,
  RT_TUE,
  RT_WED,
  RT_THU,
  RT_FRI,
  RT_SAT,
  RT_WK,
  RT_TDY,
  RT_MTH,
  RT_QTR,
  RT_YR
} RT_CODE;

/* local function headers -- see doc in function headers below */

static int rtcode_from_str( char *abb );
static int rt_add_one( TIME_DATE_STRUCT *td, int sgn, int align, int num,
		       RT_CODE code, Sint *hol_dates, Sint num_hols );
static int day_matches( Sint julian, RT_CODE code, Sint *hol_dates, 
			Sint num_hols );


/****************************
  Exported functions 
 ****************************/

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME rtime_add

   DESCRIPTION  Add relative times to a time/date. NOTE: YOU SHOULD PROBABLY
   BE USING THE rtime_add_with_zones FUNCTION, NOT THIS FUNCTION!!!!

   ARGUMENTS
      IOARG  td        struct containing the month, day, year, hour, etc.
      IARG   rt_str    the relative time string
      IARG   hol_dates dates of holidays
      IARG   num_hols  number of holidays in hol_dates

   RETURN Returns 1/0 for success/failure.  The routine fails if the input 
   arguments do not correspond to a valid relative time string, 
   or if pointers are NULL.

   ALGORITHM Parses the relative time string into individual fields,
   and calls local function rt_add_one repeatedly to add the specified
   relative time for each field, after calling local function 
   rtcode_from_str to figure out what time period to add.

   Note that the rtime_add_with_zones function does a better job of
   this, assuming that times with time zones are being used. This function
   is only left here in case someone else is using it for some reason,
   like with GMT.

   EXCEPTIONS 

   NOTE  See documentation for relative time class in R for notes
   on the format of relative time strings.

**********************************************************************/
int rtime_add( TIME_DATE_STRUCT *td, char *rt_str, Sint *hol_dates, 
	       Sint num_hols )
{
  int pos, num_ch, i, ret;
  int sgn, align, num;
  char *abb, tmpchr;
  RT_CODE code;
  Sint jul;

  if( (num_hols && !hol_dates) ||
      !td || !rt_str )
    return 0;

  num_ch = strlen( rt_str );
  pos = 0;

  while( pos < num_ch )
  {
    /* get past white space */
    while(( pos < num_ch ) && isspace( rt_str[pos] ))
      pos++;

    if( pos >= num_ch )
      break;

    /* next character (sign) must be + or - */
    if( rt_str[pos] == '+' )
      sgn = 1;
    else if( rt_str[pos] == '-' )
      sgn = -1;
    else
      return 0;

    if( ++pos >= num_ch )
      return 0;
    
    /* next character is optional a for align */
    align = 0;
    if( rt_str[pos] == 'a' )
    {
      align = 1;
      if( ++pos >= num_ch )
	return 0;
    }

    /* next characters are number to add
       find out how many digits there are */
    for( i = 0; pos + i < num_ch; i++ )
      if( !isdigit(rt_str[pos + i]))
	break;
    if(( i < 1 ) || ( pos + i >= num_ch ))
      return 0;
    
    /* read the digits */
    tmpchr = rt_str[ pos + i ];
    rt_str[ pos + i ] = '\0';
	/* S7 CHANGE Changing sscanf back to sscanf for win64 to retain correct return value */
    ret = sscanf( &(rt_str[pos]), "%d", &num );
    rt_str[ pos + i ] = tmpchr;
    if( ret != 1 )
      return 0;
    pos += i;

    /* next characters to white space are the abbreviation code */
    abb = &(rt_str[pos]);
    while(( pos < num_ch ) && !isspace( rt_str[pos] ))
      pos++;
    if( pos > num_ch )
      return 0;
    if( pos < num_ch )
    {
      tmpchr = rt_str[pos];
      rt_str[pos] = '\0';
      code = rtcode_from_str( abb );
      rt_str[pos] = tmpchr;
    } else /* this is end of string */
      code = rtcode_from_str( abb );

    if( !rt_add_one( td, sgn, align, num, code, hol_dates, num_hols ))
      return 0;

    /* that's it, go ahead and continue */
    pos++;
  }

  /* put the weekday and yearday back */
  
  if( !mdy_to_yday( td ) ||
      !julian_from_mdy( *td, &jul ))
    return 0;

  td->weekday = julian_to_weekday( jul );

  return 1;
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME rtime_add_with_zones

   DESCRIPTION  Add relative times to a time/date, doing day, month, etc.
   in the local zone, and hour, minute, and second in GMT.

   ARGUMENTS
      IOARG  td        struct with month, day, year, hour, etc. (GMT)
      IARG   rt_str    the relative time string
      IARG   hol_dates dates of holidays
      IARG   num_hols  number of holidays in hol_dates
      IARG   tzone     time zone to use for conversions

   RETURN Returns 1/0 for success/failure.  The routine fails if the input 
   arguments do not correspond to a valid relative time string, 
   or if pointers are NULL.

   ALGORITHM Parses the relative time string into individual fields,
   and calls local function rt_add_one repeatedly to add the specified
   relative time for each field, after calling local function 
   rtcode_from_str to figure out what time period to add.  Uses
   GMT_to_zone and GMT_from_zone to convert to/from the local zone
   as needed, so that days, weekdays, bizdays, weeks, months, tendays, and 
   years are done in the local zone, and hours, minutes, seconds,
   and milliseconds in GMT.  The reason for this is that the days and larger
   relative times want to preserve the time of day in the local zone, whereas
   the hours, minutes, seconds, and milliseconds relative times really mean
   that you want an that amount of real time to have passed.

   EXCEPTIONS 

   NOTE  See documentation for relative time class in R for notes
   on the format of relative time strings.

**********************************************************************/
int rtime_add_with_zones( TIME_DATE_STRUCT *td, char *rt_str, Sint *hol_dates, 
	       Sint num_hols, TZONE_STRUCT *tzone )
{
  int pos, num_ch, i, ret;
  int sgn, align, num;
  char *abb, tmpchr;
  RT_CODE code;
  Sint jul;
  int in_GMT;
  int need_local;

  in_GMT = 1;

  if( (num_hols && !hol_dates) ||
      !td || !rt_str || !tzone )
    return 0;

  num_ch = strlen( rt_str );
  pos = 0;

  while( pos < num_ch )
  {
    /* get past white space */
    while(( pos < num_ch ) && isspace( rt_str[pos] ))
      pos++;

    if( pos >= num_ch )
      break;

    /* next character (sign) must be + or - */
    if( rt_str[pos] == '+' )
      sgn = 1;
    else if( rt_str[pos] == '-' )
      sgn = -1;
    else
      return 0;

    if( ++pos >= num_ch )
      return 0;
    
    /* next character is optional a for align */
    align = 0;
    if( rt_str[pos] == 'a' )
    {
      align = 1;
      if( ++pos >= num_ch )
	return 0;
    }

    /* next characters are number to add
       find out how many digits there are */
    for( i = 0; pos + i < num_ch; i++ )
      if( !isdigit(rt_str[pos + i]))
	break;
    if(( i < 1 ) || ( pos + i >= num_ch )) {
      return 0;
    }
    
    /* read the digits */
    tmpchr = rt_str[ pos + i ];
    rt_str[ pos + i ] = '\0';
	/* S7 CHANGE Changing sscanf back to sscanf for win64 to retain correct return value */
    ret = sscanf( &(rt_str[pos]), "%d", &num );
    rt_str[ pos + i ] = tmpchr;
    if( ret != 1 )
      return 0;
    pos += i;

    /* next characters to white space are the abbreviation code */
    abb = &(rt_str[pos]);
    while(( pos < num_ch ) && !isspace( rt_str[pos] ))
      pos++;
    if( pos > num_ch )
      return 0;
    if( pos < num_ch )
    {
      tmpchr = rt_str[pos];
      rt_str[pos] = '\0';
      code = rtcode_from_str( abb );
      rt_str[pos] = tmpchr;
    } else /* this is end of string */
      code = rtcode_from_str( abb );

    /* Convert to/from GMT if we're not in right zone */
    need_local = align || (code >= RT_DAY);
    if( in_GMT && need_local ) {
      /* convert to local */
      if( !GMT_to_zone( td, tzone ))
	return 0;
      in_GMT = 0;
    } else if( !in_GMT && !need_local ) {
      /* convert to GMT for hours/min/sec */
      if( !GMT_from_zone( td, tzone ))
	return 0;
      in_GMT = 1;
    }

    if( !rt_add_one( td, sgn, align, num, code, hol_dates, num_hols ))
      return 0;

    /* that's it, go ahead and continue */
    pos++;
  }

  /* Convert back to GMT */
  if( !in_GMT  ) {
    return GMT_from_zone( td, tzone );
  }

  return 1;
}



/****************************
  Internal functions 
 ****************************/

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME rtcode_from_str

   DESCRIPTION  Figure out which relative time code a given
   string abbreviates

   ARGUMENTS
      IOARG  abb   the abbreviation as a string

   RETURN Returns the code.

   ALGORITHM This function compares the given abbreviation to the
   list of standard abbreviations to see which one matches.

   EXCEPTIONS 

   NOTE See documentation for relative time class in R for a list
   of the abbreviations.

**********************************************************************/
static int rtcode_from_str( char *abb )
{
  if( !abb )
    return( RT_ERROR );

  /* check in rough order of expected frequencies for efficiency */
  if( !strcmp( abb, "day" ))
    return( RT_DAY );
  if( !strcmp( abb, "wkd" ))
    return( RT_WKD );
  if( !strcmp( abb, "biz" ))
    return( RT_BIZ );
  if( !strcmp( abb, "mth" ))
    return( RT_MTH );
  if( !strcmp( abb, "yr" ))
    return( RT_YR );
  if( !strcmp( abb, "qtr" ))
    return( RT_QTR );
  if( !strcmp( abb, "hr" ))
    return( RT_HR );
  if( !strcmp( abb, "mon" ))
    return( RT_MON );
  if( !strcmp( abb, "tue" ))
    return( RT_TUE );
  if( !strcmp( abb, "wed" ))
    return( RT_WED );
  if( !strcmp( abb, "thu" ))
    return( RT_THU );
  if( !strcmp( abb, "fri" ))
    return( RT_FRI );
  if( !strcmp( abb, "wk" ))
    return( RT_WK );
  if( !strcmp( abb, "tdy" ))
    return( RT_TDY );
  if( !strcmp( abb, "sat" ))
    return( RT_SAT );
  if( !strcmp( abb, "sun" ))
    return( RT_SUN );
  if( !strcmp( abb, "min" ))
    return( RT_MIN );
  if( !strcmp( abb, "sec" ))
    return( RT_SEC );
  if( !strcmp( abb, "ms" ))
    return( RT_MS );

  return( RT_ERROR );

}

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME rt_add_one

   DESCRIPTION  Add or subtract one relative time to a time/date.

   ARGUMENTS
      IOARG  td        struct containing the month, day, year, hour, etc.
      IARG   sgn       sign (+1 or -1) for addition/subtraction
      IARG   align     if non-zero, allow less than num to suffice to align
      IARG   num       how many to add/subtract
      IARG   code      the time unit code of what to add/subtract
      IARG   hol_dates dates of holidays
      IARG   num_hols  number of holidays in hol_dates

   RETURN Returns 1/0 for success/failure.  The routine fails if the input 
   arguments do not correspond to a valid relative time code,
   or if pointers are NULL.

   ALGORITHM This function adds or subtracts time based on
   the meanings of the codes and alignment.  Note that the weekdays and 
   yeardays members of the time date structure are NOT maintained.

   EXCEPTIONS 

   NOTE See documentation for relative time class in R.

**********************************************************************/
static int rt_add_one( TIME_DATE_STRUCT *td, int sgn, int align, int num,
		       RT_CODE code, Sint *hol_dates, Sint num_hols )
{

  Sint ms, jul, offset; 
  int tmp, i, by_week = 0;

  if( !td || ( num_hols && !hol_dates ))
    return 0;

  /* check validity of num */
  if(( num < 0 ) || (( num == 0 ) && !align ))
    return 0;

  /* figure out which kind of thing we're adding and add them */
  switch( code )
  {
  case RT_HR:

    if( num == 0 )
    {
      /* align to beginning of hour */
      td->minute = 0;
      td->second = 0;
      td->ms = 0;
      return 1;
    }

    if(align && (( num > 23 ) || ( 24 % num )))
      return 0;

    /* fall through to minutes */
    num *= 60;

  /*LINTED: Meant to fall through here */
  case RT_MIN:

    if( num == 0 )
    {
      /* align to beginning of minute */
      td->second = 0;
      td->ms = 0;
      return 1;
    }

    if(align && ( code == RT_MIN ) && (( num > 59 ) || ( 60 % num )))
      return 0;

    /* fall through to seconds */
    num *= 60;

  /*LINTED: Meant to fall through here */
  case RT_SEC:

    if( num == 0 )
    {
      /* align to beginning of second */
      td->ms = 0;
      return 1;
    }

    if(align && ( code == RT_SEC ) && (( num > 59 ) || ( 60 % num )))
      return 0;

    /* fall through to ms */
    num *= 1000;

  /*LINTED: Meant to fall through here */
  case RT_MS:

    /* num == 0 is not allowed for ms, and already caught for others */
    if( !num ) 
      return 0;

    if(align && ( code == RT_MS ) && (( num > 999 ) || ( 1000 % num )))
      return 0;

    /* convert to ms/julian */
    if( !ms_from_hms( *td, &ms ) ||
	!julian_from_mdy( *td, &jul ))
      return 0;

    /* add or subtract offset */
    
    if( align )
    {
      offset = ms % num;
      if( sgn > 0 )
	ms += (num - offset);
      else if( offset > 0 )
	ms -= offset;
      else
	ms -= num;
    } else
      ms += sgn * num;

    if( !adjust_time( &jul, &ms ) ||
	!julian_to_mdy( jul, td ) ||
	!ms_to_hms( ms, td ))
      return 0;

    return 1;

  case RT_WK:

    /* alignment not allowed for weeks */
    if( align )
      return 0;

    /* fall through to days */
    num *= 7;

  /*LINTED: Meant to fall through here */
  case RT_DAY: 

    if( num == 0 )
    {
      /* align to beginning of day */
      td->hour = 0;
      td->minute = 0;
      td->second = 0;
      td->ms = 0;
      return 1;
    }

    if( !align )
    {
      /* convert to ms/julian */
      if( !julian_from_mdy( *td, &jul ))
	return 0;
      jul += sgn * num;

      if( !julian_to_mdy( jul, td ))
	return 0;

      return 1;
    }

    /* aligning to nearest num days within month */
    jul = days_in_month( td->month, td->year );
    if( num >= jul )
      return 0;
    if( td->hour || td->minute || td->second || td->ms )
    {
      /* account for partial days */
      if( sgn < 0 )
	td->day += 1;
      td->hour = 0;
      td->minute = 0;
      td->second = 0;
      td->ms = 0;
    }

    /* special case when we're at the first of the month and going backwards */
    /* code below will work by setting us to the 32nd of the prev month e.g. */
    /* we will always be subtracting at least 1 from the day */

    if(( td->day == 1 ) && (sgn < 0 ))
    {
      td->month -= 1;
      if( td->month < 1 )
      {
	td->month = 12;
	td->year -= 1;
      }
      td->day = (jul = days_in_month( td->month, td->year )) + 1; /* recompute jul, number of days in current month, since current month changed */
    }

    offset = (td->day - 1) % num;
    if( sgn > 0 )
      offset = offset - num;
    else if( offset == 0 )
      offset = num;

    td->day -= offset;
    if( td->day > jul ) /* jul is days in month here */
    {
      /* we've gone past the end of the month */
      /* the first of the next month is always a valid alignment */
      td->day = 1;
      td->month += 1;
      if( td->month > 12 )
      {
	td->month = 1;
	td->year += 1;
      }
    }
    
    return 1;

  case RT_QTR: 

    if( num == 0 )
    {
      /* align to beginning of quarter */
      td->month = ((td->month - 1 )/ 3) * 3 + 1;
      td->day = 1;
      td->hour = 0;
      td->minute = 0;
      td->second = 0;
      td->ms = 0;
      return 1;
    }

    /* fall through to months */
    num *= 3;

  /*LINTED: Meant to fall through here */
  case RT_MTH:

    if( num == 0 )
    {
      /* align to beginning of month */
      td->day = 1;
      td->hour = 0;
      td->minute = 0;
      td->second = 0;
      td->ms = 0;
      return 1;
    }

    if( align && (( num > 11 ) || ( 12 % num )))
      return 0;

    if( align && ( td->hour || td->minute || td->second || td->ms ||
		   ( td->day > 1 )))
    {
      /* account for partial months */
      if( sgn < 0 )
	td->month += 1;
      td->day = 1;
      td->hour = 0;
      td->minute = 0;
      td->second = 0;
      td->ms = 0;
    }

    /* calculate offset to **subtract** */
    if( align )
    {
      offset = (td->month - 1) % num;
      if( sgn > 0 )
	offset = offset - num;
      else if( offset == 0 )
	offset = num;
    } else
      offset = - sgn * num;

    td->month -= offset;
    if(( td->month > 11 ) || (td->month < 1 ))
    {
      offset = td->month % 12;
      if( offset < 1 )
	offset += 12;
      td->year += (td->month - offset) / 12;
      td->month = offset;
    }
    /* check for falling off end of month */
    jul = days_in_month( td->month, td->year );
    if( td->day > jul )
      td->day = jul;

    return 1;

  case RT_YR:

    if( num == 0 )
    {
      /* align to beginning of year */
      td->month = 1;
      td->day = 1;
      td->hour = 0;
      td->minute = 0;
      td->second = 0;
      td->ms = 0;
      return 1;
    }

    if( align && ( td->hour || td->minute || td->second || td->ms ||
		   ( td->day > 1 ) || ( td->month > 1 )))
    {
      /* account for partial years */
      if( sgn < 0 )
	td->year += 1;
      td->month = 1;
      td->day = 1;
      td->hour = 0;
      td->minute = 0;
      td->second = 0;
      td->ms = 0;
    }

    /* calculate offset to **subtract** */
    if( align )
    {
      offset = td->year % num;
      if( sgn > 0 )
	offset = offset - num;
      else if( offset == 0 )
	offset = num;
    } else
      offset = - sgn * num;

    td->year -= offset;
    
    /* check for falling off end of month (for feb29 + year) */

    jul = days_in_month( td->month, td->year );
    if( td->day > jul )
      td->day = jul;

    return 1;

  case RT_SUN:
  /*LINTED: Meant to fall through here */
  case RT_MON:
  /*LINTED: Meant to fall through here */
  case RT_TUE:
  /*LINTED: Meant to fall through here */
  case RT_WED:
  /*LINTED: Meant to fall through here */
  case RT_THU:
  /*LINTED: Meant to fall through here */
  case RT_FRI:
  /*LINTED: Meant to fall through here */
  case RT_SAT:
    by_week = 1;

  /*LINTED: Meant to fall through here */
  case RT_BIZ:
  /*LINTED: Meant to fall through here */
  case RT_WKD:

    /* convert to julian days */
    if( !julian_from_mdy( *td, &jul ))
      return 0;
    if( align && ( !num || td->hour || td->minute || td->second || td->ms ))
    {
      /* account for partial days */
      if( !num )
	sgn = -1;

      if( sgn < 0 )
	jul += 1;
      td->hour = 0;
      td->minute = 0;
      td->second = 0;
      td->ms = 0;
    }

    /* for first addition/subtraction, get to first matching day */
    /* always add/subtract at least 1 */
    tmp = 0;
    while( !tmp )
    {
      jul += sgn;
      tmp = day_matches( jul, code, hol_dates, num_hols );
      if( tmp < 0 )
	return 0;
    }

    /* now add the rest of the days */
    if( by_week && num )
      jul += sgn * ( num - 1 ) * 7;
    else
      for( i = 1; i < num; i++ )
      {
	tmp = 0;
	while( !tmp )
	{
	  jul += sgn;
	  tmp = day_matches( jul, code, hol_dates, num_hols );
	  if( tmp < 0 )
	    return 0;
	}
      }

    /* convert back to td structure */
    if( !julian_to_mdy( jul, td ))
      return 0;

    return 1;

  case RT_TDY:
    /* ten-day periods */

    if( num == 0 )
    {
      /* align to beginning of ten-day period */
      td->day = ((td->day - 1)/10) * 10 + 1;
      if( td->day > 21 )
	td->day = 21;
      td->hour = 0;
      td->minute = 0;
      td->second = 0;
      td->ms = 0;
      return 1;
    }

    if( align )
    {
      if(( num != 1) && ( num != 2 ) && ( num != 3 ))
	return 0;
      if( td->hour || td->minute || td->second || td->ms )
      {
	/* account for partial days */
	if( sgn < 0 )
	  td->day += 1;
	td->hour = 0;
	td->minute = 0;
	td->second = 0;
	td->ms = 0;
      }
    }

    /* get to next/prev partial tenday */

    if(( td->day > 1 ) && ( td->day < 11 ))
    {
      if( sgn > 0 )
	td->day = 11;
      else
	td->day = 1;
      if( !align )
	num--;
    } else if(( td->day > 11 ) && ( td->day < 21 ))
    {
      if( sgn > 0 )
	td->day = 21;
      else
	td->day = 11;
      if( !align )
	num--;
    } else if( td->day > 21 )
    {
      if( sgn > 0 )
      {
	td->day = 1;
	td->month += 1;
      }
      else
	td->day = 21;
      if( !align )
	num--;
    }

    /* align or add the rest */
    if( align )
    {
      if((( td->day == 11 ) && ( num == 3 )) ||
	 (( td->day == 21 ) && ( num == 2 )))
	td->day += sgn * 10;
    } else 
      td->day += num * sgn * 10;

    /* and get back to valid day/month */

    jul = td->day % 30;
    if( jul < 1 )
      jul += 30;
    td->month += ( td->day - jul );
    td->day = jul;

    jul = td->month % 12;
    if( jul < 1 )
      jul += 12;
    td->year += ( td->month - jul );
    td->month = jul;

    return 1;

  case RT_ERROR:
  default:
    return 0;
  }

}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME day_matches

   DESCRIPTION  Decide whether a given julian day matches the day code.

   ARGUMENTS
      IARG   julian    the julian day
      IARG   code      the time unit code
      IARG   hol_dates dates of holidays
      IARG   num_hols  number of holidays in hol_dates

   RETURN Returns 1/0 for True/False, or -1 on error.

   ALGORITHM This function finds the weekday by calling julian_to_weekday,
   and compares it for the weekday codes.  For business day code, it
   also checks to see whether the date is a holiday.  For other codes,
   it simply returns True, to avoid endless loops.

   EXCEPTIONS 

   NOTE See documentation for relative time class in R.

**********************************************************************/
static int day_matches( Sint julian, RT_CODE code, Sint *hol_dates, 
			Sint num_hols )
{

  int wkd;
  Sint low, hi, mid; 

  wkd = julian_to_weekday( julian );

  switch( code )
  {
  case RT_SUN:
    return( wkd == 0 );
  case RT_MON:
    return( wkd == 1 );
  case RT_TUE:
    return( wkd == 2 );
  case RT_WED:
    return( wkd == 3 );
  case RT_THU:
    return( wkd == 4 );
  case RT_FRI:
    return( wkd == 5 );
  case RT_SAT:
    return( wkd == 6 );
  case RT_WKD:
    return(( wkd != 6 ) && ( wkd != 0 ));

  case RT_BIZ:
    if(( wkd == 6 ) || ( wkd == 0 ))
      return 0;
    /* check for holidays */
    if( !num_hols )
      return 1;
    if( !hol_dates )
      return -1;
    /* do a binary search to see if this date is there */
    low = 0;
    hi = num_hols - 1;

    while( low <= hi )
    {
      if(( julian < hol_dates[low] ) || ( julian > hol_dates[hi] ))
	return 1;
      if(( julian == hol_dates[low] ) || ( julian == hol_dates[hi] ))
	return 0;
      if( hol_dates[low] > hol_dates[hi] )
	return -1;

      mid = (hi + low) / 2;
      if(( mid == low) || ( mid == hi ))
	break;

      if( julian == hol_dates[mid] )
	return 0;
      if( julian < hol_dates[mid] )
      {
	hi = mid - 1;
	low++;
      }
      else
      {
	low = mid + 1;
	hi--;
      }
    }

    return 1;

  default: 
    return -1;
  }
}
