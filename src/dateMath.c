/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
*************************************************************************/

#include "dateMath.h"
#include <math.h>

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME date_floor

   DESCRIPTION  Like the c function floor(), but for dates.

   ARGUMENTS
      IARG  in_jul  the input julian day number
      IARG  in_ms   the input number of milliseconds since midnight
      IARG  zone    the time zone
      OARG  out_jul the calculated julian day number 
      OARG  out_ms  the calculated millisecond number 

   RETURN Returns 1/0 for success/failure.  The routine fails if the input 
   arguments do not correspond to a real date or time zone, or if the 
   pointers for the return values are NULL.

   ALGORITHM This is a C function that works like the C floor() function,
   except that it operates on dates with times by truncating the times
   to midnight in the local time zone.  The calculation is performed by 
   converting from julian day and milliseconds to date, time of day, yearday,
   and weekday using the julian_to_mdy, ms_to_hms, mdy_to_yday, and 
   julian_to_weekday functions; then converting to the local time zone 
   using GMT_to_zone;
   then dropping the time of day back to midnight and converting back to
   GMT, julian days, and milliseconds using the GMT_from_zone,
   julian_from_mdy, and ms_from_hms functions.
   \\
   \\
   The day number corresponds to the number of
   calendar days since January 1, 1960 (which is defined to be day 0).
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
   Leap seconds are allowed, but are not considered, so there is the 
   possibility of being off by a day in the last second of a day with a 
   leap second.
   \\
   \\
   See also: date_ceil

**********************************************************************/
int date_floor( Sint in_jul, Sint in_ms, TZONE_STRUCT *zone,
		       Sint *out_jul, Sint *out_ms )
{
  TIME_DATE_STRUCT td;

  if( !zone || !out_jul || !out_ms )
    return 0;

  td.weekday = julian_to_weekday( in_jul );

  /* calculate month/day/year and convert to local zone */

  if( !julian_to_mdy( in_jul, &td ) ||
      !ms_to_hms( in_ms, &td ) ||
      !mdy_to_yday( &td ) ||
      !GMT_to_zone( &td, zone ))
    return 0;

  /* truncate the time to midnight and convert back */
  td.hour = td.minute = td.second = td.ms = 0;
  if( !GMT_from_zone( &td, zone ) ||
      !julian_from_mdy( td, out_jul ) ||
      !ms_from_hms( td, out_ms ))
    return 0;

  return 1;
}

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME date_ceil

   DESCRIPTION  Like the c function ceil(), but for dates.

   ARGUMENTS
      IARG  in_jul  the input julian day number
      IARG  in_ms   the input number of milliseconds since midnight
      IARG  zone    the time zone
      OARG  out_jul the calculated julian day number 
      OARG  out_ms  the calculated millisecond number 

   RETURN Returns 1/0 for success/failure.  The routine fails if the input 
   arguments do not correspond to a real date or time zone, or if the 
   pointers for the return values are NULL.

   ALGORITHM This is a C function that works like the C ceil() function,
   except that it operates on dates with times by promoting the times
   to midnight on the next day (unless the input was exactly midnight) 
   in the local time zone.  The calculation is performed by 
   converting from julian day and milliseconds to date, time of day, and
   yearday, and weekday using the julian_to_mdy, ms_to_hms, mdy_to_yday, 
   and julian_to_weekday functions; then converting to the local time zone 
   using GMT_to_zone; 
   then promoting the time of day to midnight and converting back to
   GMT, julian days, and milliseconds using the GMT_from_zone,
   julian_from_mdy, and ms_from_hms functions.
   \\
   \\
   The day number corresponds to the number of
   calendar days since January 1, 1960 (which is defined to be day 0).
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
   Leap seconds are allowed, but are not considered, so there is the 
   possibility of being off by a day in the last second of a day with a 
   leap second.
   \\
   \\
   See also: date_floor

**********************************************************************/
int date_ceil( Sint in_jul, Sint in_ms, TZONE_STRUCT *zone,
		       Sint *out_jul, Sint *out_ms )
{
  TIME_DATE_STRUCT td;

  if( !zone || !out_jul || !out_ms )
    return 0;

  td.weekday = julian_to_weekday( in_jul );

  /* calculate month/day/year and convert to local zone */

  if( !julian_to_mdy( in_jul, &td ) ||
      !ms_to_hms( in_ms, &td ) ||
      !mdy_to_yday( &td ) ||
      !GMT_to_zone( &td, zone ))
    return 0;

  /* advance the time to midnight and convert back */
  if( td.hour || td.minute || td.second || td.ms )
    if( !add_offset( &td, MS_PER_DAY / 1000 ))
      return 0;

  td.hour = td.minute = td.second = td.ms = 0;

  if( !GMT_from_zone( &td, zone ) ||
      !julian_from_mdy( td, out_jul ) ||
      !ms_from_hms( td, out_ms ))
    return 0;

  return 1;
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME add_offset

   DESCRIPTION  Add seconds to a time/date structure

   ARGUMENTS
      IOARG  tstruc      the time/date structure to add to
      IARG   secs_to_add the number of seconds to add

   RETURN Returns 1/0 for success/failure.  The routine fails if the input 
   structure is NULL or does not correspond to a real date.

   ALGORITHM This function converts the time structure to julian
   date and milliseconds, using the ms_from_hms and julian_from_mdy 
   functions.  Then it adds the seconds carrying over into the
   days if necessary, and then converts back to the time structure
   using the julian_to_mdy and ms_to_hms functions.  The weekday
   and yearday members of the time/date structure are recalculated, 
   if the julian day changed, using the julian_to_weekday and 
   mdy_to_yday functions.

   EXCEPTIONS 

   NOTE Calendar dates prior to 1920 were different in many countries.
   See the ``Calendar FAQ'' posted regularly to Usenet news groups
   soc.history, sci.astro, sci.answers, soc.answers, and news.answers,
   and to a web site at www.pip.dknet.dk/{\mytilde}c-t/calendar.html for more
   information on the history of calendars around the world.
   \\
   \\
   This function does not know about leap seconds, so days with leap
   seconds may be off by a second. 

**********************************************************************/
int add_offset( TIME_DATE_STRUCT *tstruc, Sint secs_to_add )
{
  Sint jul, ms, tmp;

  /* convert to ms since midnight */

  if( !ms_from_hms( *tstruc, &ms ))
    return 0;

  /* add seconds */
  ms += 1000 * secs_to_add;

  /* adjust day if necessary */
  if(( ms >= MS_PER_DAY ) || ( ms < 0 ))
  {
    /* convert to julian day */
    if( !julian_from_mdy( *tstruc, &jul ))
      return 0;

    tmp = ms % MS_PER_DAY;
    if( tmp < 0 ) tmp += MS_PER_DAY;
    jul += ( ms - tmp )/MS_PER_DAY;
    ms = tmp;

    /* convert back to m/d/y */
    if( !julian_to_mdy( jul, tstruc ))
      return 0;

    /* re-calculate the weekday */
    tstruc->weekday = julian_to_weekday( jul );

    /* re-calculate the yearday */
    if( !mdy_to_yday( tstruc ))
      return 0;
  }

  /* convert back to h/m/s */
  if( !ms_to_hms( ms, tstruc ))
    return 0;

  return 1;
}
