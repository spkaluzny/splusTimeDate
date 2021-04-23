/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
 *************************************************************************/

/*************************************************************************
 * $Id: //depot/Devel/splus/slocal/timeser/src/mdy.c#12 $
 *
 * It contains C code utility functions for dealing with dates in 
 * "julian" and month/day/year formats, as well as times in milliseconds
 * since midnight, fractional day, and hour/minute/second/millisecond formats.
 * There is also a function for calculating weekday number, where 0 is Sunday
 * and 6 is Saturday, and an easter calculator.
 *  
 * See TIME.h for a more compact listing of the included functions.
 * They were written as auxiliary functions for R code
 * dealing with times and dates.
 *
 * The "julian" format for dates is the number of days since January
 * 1, 1960, which is defined to be day 0. The calendar follows the 
 * conventions of the British Empire, which changed from Julian to 
 * Gregorian calendars in September of 1752.  Calendar dates prior to
 * 1920 were different in many countries.  See the "Calendar FAQ" posted
 * regularly to Usenet news groups soc.history, sci.astro, sci.answers,
 * soc.answers, and news.answers, and to a web site at
 * www.pip.dknet.dk/{\mytilde}c-t/calendar.html for more information on the
 * history of calendars around the world.
 *************************************************************************/

#include "mdy.h"
#include <math.h>

/* internal functions used below and defined at bottom of file
*/
static Sint days_in_year( Sint year );
static int  is_leap_year( Sint year );
static Sint LRound( double num );

static Sint month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/****************************
  Exported functions 
 ****************************/

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME julian_from_mdy

   DESCRIPTION  Convert to ``julian'' days from month/day/year, where 
   the julian day number is the number of days since 1/1/1960.

   ARGUMENTS
      IARG  td_input  struct containing the month, day, and year
      OARG  julian  the calculated day number 

   RETURN Returns 1/0 for success/failure.  The routine fails if the input 
   arguments do not correspond to a real date or if the pointer for the 
   return value is NULL.

   ALGORITHM This is a C function that calculates the day number given a
   month, day, and year.  The day number corresponds to the number of
   calendar days since January 1, 1960 (which is defined to be day 0).
   The calendar follows the conventions of the British Empire, which 
   changed from Julian to Gregorian calendars in September of 1752.
   Prior to 8AD, results may be incorrect.
   The calculation is by simple tabulation of the number of days in
   each month and year between the given date and 1/1/1960, through
   calls to the days_in_month and days_in_year functions.

   EXCEPTIONS 

   NOTE Calendar dates prior to 1920 were different in many countries.
   See the ``Calendar FAQ'' posted regularly to Usenet news groups
   soc.history, sci.astro, sci.answers, soc.answers, and news.answers,
   and to a web site at www.pip.dknet.dk/{\mytilde}c-t/calendar.html for more
   information on the history of calendars around the world.
   \\
   \\
   See also: julian_to_mdy, julian_from_index

**********************************************************************/
int julian_from_mdy( TIME_DATE_STRUCT td_input, Sint *julian )
{
  Sint i;

  if( !julian )
    return 0;

  /* check that args are within bounds; in sept 1752 special case it */
  if(( td_input.month < 1 ) || ( td_input.day < 1 ) || 
     ( td_input.month > 12 ) || 
     (( td_input.day > days_in_month( td_input.month, td_input.year )) && 
      !(( td_input.month == 9 ) && ( td_input.year == 1752 ))))
    return 0;

  /* figure out how many days since julian origin */
  /* special case end of September 1752 */

  if(( td_input.year == 1752 ) && ( td_input.month == 9 ))
  {
    if( td_input.day <= 2 )
      *julian = td_input.day - 1;
    else if(( td_input.day < 14 ) || ( td_input.day > 30 )) 
      return 0; /* these days didn't occur */
    else
      *julian =  td_input.day - 12;
  } else
    *julian =  td_input.day - 1;

  for( i = 1; i <  td_input.month; i++ )
    *julian += days_in_month( i,  td_input.year );
  for( i = JULIAN_YEAR; i <  td_input.year; i++ )
    *julian += days_in_year( i );
  for( i = JULIAN_YEAR - 1; i >=  td_input.year; i-- )
    *julian -= days_in_year( i );

  return( 1 );

}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME julian_from_index

   DESCRIPTION  Convert to ``julian'' days from month/weekday/index/year, 
   where the julian day number is the number of days since 1/1/1960,
   and index/weekday indicate e.g. the 3rd Monday of the month.

   ARGUMENTS
      IARG  month   the month, 1-12
      IARG  wkday   the weekday, 0-6 with 0 as Sunday
      IARG  index   index for nth wkday, 1-5 or -1 for last
      IARG  year    the year
      OARG  julian  the calculated day number 

   RETURN Returns 1/0 for success/failure.  The routine fails if the input 
   arguments do not correspond to a real date or if the pointer for the 
   return value is NULL.

   ALGORITHM This is a C function that calculates the day number given a
   month, weekday, index, and year, by making use of the 
   julian_from_mdy and julian_to_weekday functions.
   The day number corresponds to the number of calendar days since 
   January 1, 1960 (which is defined to be day 0). The calendar follows 
   the conventions of the British Empire, which 
   changed from Julian to Gregorian calendars in September of 1752.
   Prior to 8AD, results may be incorrect.

   EXCEPTIONS 

   NOTE Calendar dates prior to 1920 were different in many countries.
   See the ``Calendar FAQ'' posted regularly to Usenet news groups
   soc.history, sci.astro, sci.answers, soc.answers, and news.answers,
   and to a web site at www.pip.dknet.dk/{\mytilde}c-t/calendar.html for more
   information on the history of calendars around the world.
   \\
   \\
   See also: julian_from_mdy

**********************************************************************/
int julian_from_index( Sint month, Sint wkday, Sint index, Sint year, 
		       Sint *julian )
{
  TIME_DATE_STRUCT td;
  Sint toadd;

  /* error checking */
  if( !julian )
    return 0;
  if(( index != -1 ) && (( index < 1 ) || ( index > 5 )))
    return 0;
  if(( wkday > 6 ) || ( wkday < 0 ))
    return 0;
  if(( month > 12 ) || ( month < 1 ))
    return 0;

  if( index > 0 )
  {
    /* For positive values of index, first go to the first possible day */
    td.year = year;
    td.month = month;
    td.day = ( index - 1 ) * 7 + 1;
    if( !julian_from_mdy( td, julian ))
      return 0;

    /* then add to get to the correct weekday */
    toadd = ( wkday - julian_to_weekday( *julian )) % 7;
    if( toadd < 0 ) toadd += 7;
    *julian += toadd;

    /* and verify it's in the correct month (could have crossed) */
    if( !julian_to_mdy( *julian, &td ))
      return 0;
    if( td.month != month )
      return 0;
    return 1;
  } 

  /* OK, we're after the last instance in the month, so start at the
     beginning of the next month */

  td.year = year;
  td.month = month + 1;
  if( td.month > 12 )
  {
    td.month = 1;
    td.year += 1;
  }
  td.day = 1;

  if( !julian_from_mdy( td, julian ))
    return 0;

  /* subtract one day to get to the end of this month */
  *julian -= 1;

  /* then subtract to get to the correct weekday */
  toadd = ( wkday - julian_to_weekday( *julian )) % 7;
  if( toadd > 0 ) toadd -= 7;
  *julian += toadd;

  return 1;
}

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME julian_to_mdy

   DESCRIPTION  Convert ``julian'' days to month/day/year, where 
   the julian day number is the number of days since 1/1/1960.

   ARGUMENTS
      IARG  julian      the julian day number 
      OARG  td_output   struct with month, day, and year

   RETURN Returns 1/0 for success/failure.  The routine fails if any 
   pointer for return values is NULL.

   ALGORITHM This is a C function that calculates month, day, and year 
   given a day number; the time-related, zone, and weekday parts of 
   the output structure 
   are not changed.  The day number corresponds to the number of
   calendar days since January 1, 1960 (which is defined to be day 0).
   The calendar follows the conventions of the British Empire, which 
   changed from Julian to Gregorian calendars in September of 1752.
   Prior to 8AD, results may be incorrect.
   The calculation is by simple tabulation of the number of days in
   each month and year between the given date and 1/1/1960, making
   use of the days_in_year and days_in_month functions.

   EXCEPTIONS 

   NOTE Calendar dates prior to 1920 were different in many countries.
   See the ``Calendar FAQ'' posted regularly to Usenet news groups
   soc.history, sci.astro, sci.answers, soc.answers, and news.answers,
   and to a web site at www.pip.dknet.dk/{\mytilde}c-t/calendar.html for more
   information on the history of calendars around the world.
   \\
   \\
   See also: julian_from_mdy, julian_to_weekday, mdy_to_yday

**********************************************************************/
int julian_to_mdy( Sint julian,  TIME_DATE_STRUCT *td_output )
{
  Sint tmpdays;

  if( !td_output )
    return 0;

  td_output->year = JULIAN_YEAR;
  td_output->day = 1;
  td_output->month = 1;

  /* first do the years */
  while( julian < 0 )
  {
    julian += days_in_year( td_output->year - 1 );
    (td_output->year)--;
  }

  while( (tmpdays = days_in_year( td_output->year )) <= julian )
  {
    julian -= tmpdays;
    (td_output->year)++;
  }

  /* now the months */
  while( (tmpdays = days_in_month( td_output->month, td_output->year )) 
	     <= julian )
  {
    julian -= tmpdays;
    (td_output->month)++;
  }

  td_output->day += julian;

  /* special case september 1752 */
  if((td_output->year == 1752 ) && ( td_output->month == 9 ) && 
     ( td_output->day > 2 ))
    td_output->day += 11;

  return 1;
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME julian_to_weekday

   DESCRIPTION  Calculate the weekday number for a given ``julian'' day,
   where the julian day number is the number of days since 1/1/1960
   and the weekday number is 0 for Sunday, 1 for Monday, and 6 for Saturday.

   ARGUMENTS
      IARG  julian  the julian day number 

   RETURN The return value is the weekday number

   ALGORITHM This is a C function that calculates the weekday number
   for a given julian day number. The day number corresponds to the number 
   of calendar days since January 1, 1960 (which is defined to be day 0,
   and is a Friday).
   The calendar follows the conventions of the British Empire, which 
   changed from Julian to Gregorian calendars in September of 1752.
   The calculation is performed by examining the remainder when the
   day number is divided by 7.

   EXCEPTIONS 

   NOTE   Calendar dates prior to 1920 were different in many countries.
   See the ``Calendar FAQ'' posted regularly to Usenet news groups
   soc.history, sci.astro, sci.answers, soc.answers, and news.answers,
   and to a web site at www.pip.dknet.dk/{\mytilde}c-t/calendar.html for more
   information on the history of calendars around the world.
   \\
   \\
   See also: julian_to_mdy

**********************************************************************/
int julian_to_weekday( Sint julian )
{
  int iret;

  Sint ret = (julian + WEEKDAY_START) % 7;
  if( ret < 0 ) ret += 7;
  /*LINTED: cast OK -- always between 0 and 7 */
  iret = (int) ret;
  return( iret );
}

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME mdy_to_yday

   DESCRIPTION  Convert from month/day/year to day of year.

   ARGUMENTS
      IOARG  td_input  struct containing the month, day, and year

   RETURN Returns 1/0 for success/failure.  The routine fails if the input 
   arguments do not correspond to a real date or if the pointer for the 
   return value is NULL.

   ALGORITHM This is a C function that calculates the day of the
   year given a month, day, and year.  January 1 is day 1, and 
   December 31 is day 365 or 366, depending on leap years.
   The calendar follows the conventions of the British Empire, which 
   changed from Julian to Gregorian calendars in September of 1752.
   Prior to 8AD, results may be incorrect.
   The calculation is by simple tabulation, making use of the 
   days_in_month function.  The result is put into the time/date structure.

   EXCEPTIONS 

   NOTE Calendar dates prior to 1920 were different in many countries.
   See the ``Calendar FAQ'' posted regularly to Usenet news groups
   soc.history, sci.astro, sci.answers, soc.answers, and news.answers,
   and to a web site at www.pip.dknet.dk/{\mytilde}c-t/calendar.html for more
   information on the history of calendars around the world.
   \\
   \\
   See also: julian_to_mdy

**********************************************************************/
int mdy_to_yday( TIME_DATE_STRUCT *td_input )
{
  Sint i;

  if( !td_input )
    return 0;

  /* check that args are within bounds; in sept 1752 special case it */
  if(( td_input->month < 1 ) || ( td_input->day < 1 ) || 
     ( td_input->month > 12 ) || 
     (( td_input->day > days_in_month( td_input->month, td_input->year )) && 
      !(( td_input->month == 9 ) && ( td_input->year == 1752 ))))
    return 0;

  /* figure out how many days since January 1 */
  /* special case end of September 1752 */

  if(( td_input->year == 1752 ) && ( td_input->month == 9 ))
  {
    if( td_input->day <= 2 )
      /*LINTED: Cast OK - day is between 1 and 2 */
      td_input->yearday = (int) td_input->day;
    else if(( td_input->day < 14 ) || ( td_input->day > 30 )) 
      return 0; /* these days didn't occur */
    else
      /*LINTED: Cast OK - day is between 1 and 31 */
      td_input->yearday =  (int) td_input->day - 11;
  } else
    /*LINTED: Cast OK - day is between 1 and 31 */
    td_input->yearday =  (int) td_input->day;

  for( i = 1; i <  td_input->month; i++ )
    td_input->yearday += days_in_month( i,  td_input->year );

  return( 1 );
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME julian_easter

   DESCRIPTION  Calculate the julian day of Easter in the given year,
   where the julian day number is the number of days since 1/1/1960.

   ARGUMENTS
      IARG  year    the year to calculate it (e.g. 1978)
      OARG  julian  the calculated day number 

   RETURN Returns 1/0 for success/failure.  The algorithm fails if
   the pointer for the return value is NULL.

   ALGORITHM This is a C function that calculates easter from
   an algorithm given in a web site called ``Whence Easter,'' at
   http://www.gold.net/{\mytilde}cdwf/misc/easter.html.  The
   given algorithm calculates the number of days since March 22nd
   for Easter, to which we add the ``julian'' day number for March
   22nd in that year, found with the julian_from_mdy function.
   The day number corresponds to the number of calendar days since 
   January 1, 1960 (which is defined to be day 0). The calendar follows 
   the conventions of the British Empire, which 
   changed from Julian to Gregorian calendars in September of 1752.
   Since the Easter calculator is for the Gregorian calendar, its
   answer will be incorrect prior to 1752. 

   EXCEPTIONS 

   NOTE Calendar dates prior to 1920 were different in many countries.
   See the ``Calendar FAQ'' posted regularly to Usenet news groups
   soc.history, sci.astro, sci.answers, soc.answers, and news.answers,
   and to a web site at www.pip.dknet.dk/{\mytilde}c-t/calendar.html for more
   information on the history of calendars around the world.

**********************************************************************/
int julian_easter( Sint year, Sint *julian )
{
  Sint aa, bb, cc, dd, ee, ff, gg, hh, jj, mm, kk;
  TIME_DATE_STRUCT td;

  if( !julian )
    return 0;

  /* I'm just following the algorithm from the web site verbatim,
     and have put in comments from the algorithm there */

  /* The factor of 19 comes from the more or less 235 lunar months
     for 19 years.  Easter is the Sunday after the first full moon
     of Spring, more or less, and Spring starts on March 21 or so */

  aa = year % 19;  /* aa + 1 is the "golden number" */

  bb = year / 100; /* century */
  cc = year % 100; /* year within century */

  dd = bb / 4;     /* number of 400 year cycles */
  ee = bb % 4;     /* place in 400 year cycle */

  ff = cc / 4;     /* number of leap years so far this century */
  gg = cc % 4;     /* number of years since last leap year */

  hh = ( 8 * bb + 13 ) / 25; 
                   /* number of days to shift full moon */

  jj = ( 19 * aa + bb - dd - hh + 15 ) % 30; 
                   /* unadjusted date of Paschal full moon */

  mm = ( aa + 11 * jj ) / 319; 
                   /* 1 to adjust moon back a day and 0 otherwise */

  kk = ( 2 * ee + 2 * ff - gg - jj + mm + 32 ) % 7 ;
                   /* kk + 1 is number of days from Paschal full moon to
		      Easter Sunday */

  /* At this point, jj - mm + kk is the date of Easter Sunday, with
     0 being March 22nd */

  /* so we can calculate the Julian day by finding the date of March 22nd 
     and adding jj - mm + kk */

  td.year = year;
  td.month = 3;
  td.day = 22;
  if( !julian_from_mdy( td, julian ))
    return 0;

  *julian += ( jj - mm + kk );

  return 1;
}

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME ms_from_hms

   DESCRIPTION  Calculate the number of milliseconds since midnight,
   given the time of day as hours/minutes/seconds/milliseconds.

   ARGUMENTS
      IARG  td_input  struct containing the hour, minute, second, ms
      OARG  ms_ret    the returned number of milliseconds

   RETURN The return value is 1/0 for success/failure.  The routine
   fails if the pointer for the return data is NULL or if the input
   arguments are not a valid time.

   ALGORITHM This is a C function that calculates the number of 
   milliseconds since midnight, given the time of day as 
   hours/minutes/seconds/milliseconds.  Calculation is by
   simple arithmetic.

   EXCEPTIONS 

   NOTE Leap seconds are allowed, but this algorithm will not know if 
   a leap second has occurred earlier in the day, so the calculated 
   number of milliseconds since midnight will be off by 1000 after
   any leap second.
   \\
   \\
   See also: ms_to_hms, ms_from_fraction

**********************************************************************/
int ms_from_hms( TIME_DATE_STRUCT td_input, Sint *ms_ret )
{
  /* error checking */

  if( !ms_ret )
    return 0;
  if(( td_input.hour > 23 ) || ( td_input.hour < 0 ))
    return 0;
  if(( td_input.minute < 0 ) || ( td_input.minute >= 60 ))
    return 0;
  /* allow seconds to go to 60 for leap seconds */
  if(( td_input.second < 0 ) || ( td_input.second >= 61 ))
    return 0;
  if(( td_input.ms < 0 ) || ( td_input.ms >= 1000 ))
    return 0;

  /* conversion */

  *ms_ret = 60 * td_input.hour + td_input.minute;
  *ms_ret = 60 * (*ms_ret ) + td_input.second;
  *ms_ret = 1000 * (*ms_ret ) + td_input.ms;

  return 1;
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME ms_to_hms

   DESCRIPTION  Calculate the time of day as 
   hours/minutes/seconds/milliseconds, given the number of milliseconds 
   since midnight. 

   ARGUMENTS
      IARG  ms          number of milliseconds since midnight
      OARG  td_output   struct with hours, minutes, seconds, milliseconds

   RETURN The return value is 1/0 for success/failure.  The routine
   fails if any pointer for return data is NULL or if the input
   argument is not a valid time.

   ALGORITHM   This is a C function that calculates the time of day as 
   hours/minutes/seconds/milliseconds, given the number of milliseconds 
   since midnight.  Calculation is by simple arithmetic.  The date-related
   and time zone parts of the time/date structure are not changed.

   EXCEPTIONS 

   NOTE Leap seconds are allowed, but this algorithm will not know if 
   a leap second has occurred, so the calculated time of day
   will be off by a second if a leap second occurred earlier in the day.
   \\
   \\
   See also: ms_from_hms, ms_to_fraction

**********************************************************************/
int ms_to_hms( Sint ms, TIME_DATE_STRUCT *td_output )
{
  /* error checking */
  if( !td_output )
    return 0;

  /* allow ms to go to MS_PER_DAY + 1 second for leap seconds */
  if(( ms < 0 ) || ( ms >= ( MS_PER_DAY + 1000 )))
    return 0;

  /* conversion */
  td_output->second = ms / 1000;
  td_output->ms = ms - 1000 * (td_output->second);
  td_output->minute = (td_output->second) / 60;
  td_output->second -= (td_output->minute) * 60;
  td_output->hour = (td_output->minute) / 60;
  td_output->minute -= (td_output->hour) * 60;

  /* if hour exceeds 23, this was the last second of a leap second day */
  if( td_output->hour == 24 )
  {
    td_output->hour = 23;
    td_output->minute = 59;
    td_output->second = 60;
  }
    
  return 1;
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME ms_from_fraction

   DESCRIPTION  Convert from fraction of a day to number of milliseconds.

   ARGUMENTS
      IARG  frac   fraction of the day
      OARG  ms     number of milliseconds

   RETURN The return value is 1/0 for success/failure.  The routine
   fails if the pointer for return data is NULL or if the input
   fraction is not between 0 and 1. 

   ALGORITHM   This is a C function that converts between fraction
   of a day to number of milliseconds.  Calculation is by simple
   arithmetic and the result is rounded to the nearest whole number
   of milliseconds.

   EXCEPTIONS 

   NOTE The number of milliseconds is always calculated with respect
   to a normal-length day, not taking into account leap seconds.
   \\
   \\
   See also: ms_from_hms, ms_to_fraction

**********************************************************************/
int ms_from_fraction( double frac, Sint *ms )
{
  if( !ms || ( frac > 1 ) || ( frac < 0 ))
    return 0;
  *ms = LRound( frac * MS_PER_DAY );
  return 1;
}

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME ms_to_fraction

   DESCRIPTION  Convert from number of milliseconds to fraction of a day.

   ARGUMENTS
      IARG  ms     number of milliseconds
      OARG  frac   fraction of the day

   RETURN The return value is 1/0 for success/failure.  The routine
   fails if the pointer for return data is NULL or if the input
   number of milliseconds is negative or exceeds one day.

   ALGORITHM   This is a C function that converts between number of
   milliseconds to fraction of a day by simple division.

   EXCEPTIONS 

   NOTE The fraction of the day is always calculated with respect to
   a normal-length day, not taking into account leap seconds.  On a 
   day with a leap second, the fraction returned will be exactly 1
   for the entire last second of the day.
   \\
   \\
   See also: ms_to_hms, ms_from_fraction

**********************************************************************/
int ms_to_fraction( Sint ms, double *frac )
{
  /* allow leap seconds */
  if(!frac || ( ms < 0 ) || ( ms >= ( MS_PER_DAY + 1000 )))
    return 0;
  *frac = (double) ms / MS_PER_DAY;
  if( *frac > 1 ) *frac = 1;
  return 1;
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME jms_to_struct

   DESCRIPTION  Convert from ``julian'' days and milliseconds since
   midnight to a complete (except zone) time/date structure.

   ARGUMENTS
      IARG  julian    the julian day number 
      IARG  ms        the number of milliseconds
      IARG  td_output time/date structure

   RETURN Returns 1/0 for success/failure.  The routine fails if the input 
   arguments do not correspond to a real date or if the pointer for the 
   return value is NULL.

   ALGORITHM This function calls julian_to_mdy, ms_to_hms, 
   julian_to_weekday, and mdy_to_yday to fill in the structure.

   EXCEPTIONS 

   NOTE 

**********************************************************************/
int jms_to_struct( Sint julian, Sint ms, TIME_DATE_STRUCT *td_output )
{

  if( !td_output ||
      !julian_to_mdy( julian, td_output ) ||
      !mdy_to_yday( td_output ) ||
      !ms_to_hms( ms, td_output ))
    return 0;

  td_output->weekday = julian_to_weekday( julian );

  return( 1 );

}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME adjust_span

   DESCRIPTION  Adjust a time span so that ms part is less than a
   day and the signs of day and ms parts agree

   ARGUMENTS
      IOARG  julian      days in time span
      IOARG  ms          milliseconds in time span

   RETURN Returns 1/0 for success/failure.  The routine fails if the input 
   pointers are NULL.

   ALGORITHM Straightforward arithmetic.

   EXCEPTIONS 

   NOTE 

**********************************************************************/
int adjust_span( Sint *julian, Sint *ms )
{
  Sint tmplng;

  if( !julian || !ms )
    return 0;

  /* first get ms within 1 day span */

  if(( *ms >= MS_PER_DAY ) || ( *ms <= - MS_PER_DAY ))
  {
    tmplng = *ms % MS_PER_DAY;
    *julian += (*ms - tmplng) / MS_PER_DAY;
    *ms = tmplng;
  }

  /* now make signs of day and ms same */
  if(( *julian > 0 ) && (*ms < 0 ))
  {
    *ms += MS_PER_DAY;
    *julian -= 1;
  } else if(( *julian < 0 ) && (*ms > 0 ))
  {
    *ms -= MS_PER_DAY;
    *julian += 1;
  }

  return 1;
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME adjust_time

   DESCRIPTION  Adjust a time so that ms part is less than a
   day and positive. 

   ARGUMENTS
      IOARG  julian      julian days
      IOARG  ms          milliseconds

   RETURN Returns 1/0 for success/failure.  The routine fails if the input 
   pointers are NULL.

   ALGORITHM Straightforward arithmetic.

   EXCEPTIONS 

   NOTE 

**********************************************************************/
int adjust_time( Sint *julian, Sint *ms )
{
  Sint tmplng;

  if( !julian || !ms )
    return 0;

  /* first get ms within 1 day span */

  if(( *ms >= MS_PER_DAY ) || ( *ms < 0 ))
  {
    tmplng = *ms % MS_PER_DAY;
    if( tmplng < 0 ) tmplng += MS_PER_DAY;
    *julian += (*ms - tmplng) / MS_PER_DAY;
    *ms = tmplng;
  }

  return 1;
}

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME days_in_month

   DESCRIPTION  Calculate the actual number of days in the given
   month in the given year, taking into account leap days.

   ARGUMENTS
      IARG  month  the month (1-12)
      IARG  year   the year

   RETURN The return value is the number of days in the given month,
   and 0 for an invalid month.

   ALGORITHM   The number of days is simply returned from a table except
   in February, when the is_leap_year function is called to determine
   whether to return 28 or 29, and in September 1752 which is a special
   case.
   The calendar follows the conventions of the British Empire, which 
   changed from Julian to Gregorian calendars in September of 1752.
   Prior to 8AD, results may be incorrect.

   EXCEPTIONS 

   NOTE Calendar dates prior to 1920 were different in many countries.
   See the ``Calendar FAQ'' posted regularly to Usenet news groups
   soc.history, sci.astro, sci.answers, soc.answers, and news.answers,
   and to a web site at www.pip.dknet.dk/{\mytilde}c-t/calendar.html for more
   information on the history of calendars around the world.
   \\
   \\
   See also: days_in_year

**********************************************************************/
Sint days_in_month( Sint month, Sint year )
{
  /* special case Sept 1752, when 11 days were dropped from
     the calendar in September. */

  if(( year == 1752 ) && ( month == 9 )) return 19;

  /* take care of February */

  if( month == 2 )
  {
     if( is_leap_year( year )) return 29L;
     return 28L;
  }

  if(( month > 12 ) || ( month < 1 ))
    return 0;

  return month_days[ month - 1 ];
}

/****************************
  Internal functions
 ****************************/

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME days_in_year

   DESCRIPTION  Calculate the actual number of days in the year.

   ARGUMENTS
      IARG  year   the year

   RETURN The return value is the number of days in the given year.

   ALGORITHM   The number of days is calculated by calling is_leap_year
   to decide if the year is a leap year or not, and returning 366 for
   leap years and 365 otherwise, except in 1752 which is a special case.
   The calendar follows the conventions of the British Empire, which 
   changed from Julian to Gregorian calendars in September of 1752.
   Prior to 8AD, results may be incorrect.

   EXCEPTIONS 

   NOTE Calendar dates prior to 1920 were different in many countries.
   See the ``Calendar FAQ'' posted regularly to Usenet news groups
   soc.history, sci.astro, sci.answers, soc.answers, and news.answers,
   and to a web site at www.pip.dknet.dk/{\mytilde}c-t/calendar.html for more
   information on the history of calendars around the world.
   \\
   \\
   See also: days_in_month

**********************************************************************/
static Sint days_in_year( Sint year )
{
  /* special case 1752, when 11 days were dropped from the calendar
     in September, and it was a leap year. */

  if( year == 1752 ) return 355L;

  if( is_leap_year( year )) return 366L;

  return 365L;
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME is_leap_year

   DESCRIPTION  Decide whether the given year is a leap year or not.

   ARGUMENTS
      IARG  year   the year

   RETURN The return value is 1 for leap years and 0 for non-leap years.

   ALGORITHM  Leap years under the Gregorian calendar are in years 
   divisible by 4, except for years divisible by 100, which are leap years
   only if they are also divisible by 400.  In the Julian calendar,
   all years divisible by 4 were leap years.  For this function,
   the calendar follows the conventions of the British Empire, which 
   changed from Julian to Gregorian calendars in September of 1752.
   Prior to 8AD, results may be incorrect.

   EXCEPTIONS 

   NOTE 
   Calendar dates prior to 1920 were different in many countries.
   See the ``Calendar FAQ'' posted regularly to Usenet news groups
   soc.history, sci.astro, sci.answers, soc.answers, and news.answers,
   and to a web site at www.pip.dknet.dk/{\mytilde}c-t/calendar.html for more
   information on the history of calendars around the world.
   \\
   \\
   See also: days_in_year, days_in_month

**********************************************************************/
static int is_leap_year( Sint year )
{
  /* if not divisible by 4 it's not a leap year */

  if( year % 4 )
    return 0;

  /* if divisible by 4 and before 1752 it is */
  /* (they didn't do the year 100 thing then) */

  if( year <= 1752 )
    return 1;

  /* if divisible by 4 but not by 100 it is */
  if( year % 100 )
    return 1;
   
  /* if divisible by 100 but not by 400 it isn't */
  if( year % 400 )
    return 0;

  return 1;
}

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME LRound

   DESCRIPTION  Round a double to the nearest Sint

   ARGUMENTS
      IARG  num  The number to be rounded

   RETURN The return value is the rounded number.

   ALGORITHM  The rounded number is calculated from floor( num + 0.5 ).

   EXCEPTIONS 

   NOTE

**********************************************************************/
static Sint LRound( double num )
{
  num = floor( num + 0.5 );
  return ((Sint) num );
}
