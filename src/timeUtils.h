/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
 *************************************************************************/

#ifndef TIMELIB_TIMEUTILS_H
#define TIMELIB_TIMEUTILS_H


#ifdef __cplusplus
extern "C" {
#endif

#include <R.h>
#include <Rdefines.h>
#include <Rversion.h>
#include <string.h>

#define JULIAN_YEAR 1960
#define WEEKDAY_START 5
#define MS_PER_DAY 86400000

#define TIME_CLASS_NAME "timeDate"
#define TSPAN_CLASS_NAME "timeSpan"
#define C_ZONE_CLASS_NAME "timeZoneC"
#define R_ZONE_CLASS_NAME "timeZoneR"

/* Sfloat, Sint added for splusTimeDate_2.5.4 as they will be dropped
 * from R soon.
 */
typedef double Sfloat;
typedef int Sint;

/**********************************************************************
 * R-DOCUMENTATION ************************************************
 **********************************************************************
   NAME TIME_DATE_STRUCT

   TYPE  typedef

   DESCRIPTION  This structure includes all the pieces of a calendar
   time and date

   ARGUMENTS
      IARG  month           the calendar month (1-12)
      IARG  day             the calendar day (1-31)
      IARG  year            the calendar year (e.g. 1968)
      IARG  hour            the hour of the day (0-23)
      IARG  minute          the minute of the day (0-59)
      IARG  second          the second of the day (0-59)
      IARG  ms              the millisecond of the day (0-999)
      IARG  weekday         the weekday number (0-6) (0 is Sunday)
      IARG  yearday         the day of the year (1-366)
      IARG  zone            the printable time zone string
      IARG  daylight        1/0 for daylight savings/standard time

   RETURN 

   ALGORITHM 

   EXCEPTIONS 

   NOTE 

**********************************************************************/
typedef struct td_struc
{
  Sint month;
  Sint day;
  Sint year;
  Sint hour;
  Sint minute;
  Sint second;
  Sint ms;
  int  weekday;
  int  yearday;
  char *zone;
  int  daylight;

} TIME_DATE_STRUCT;


/**********************************************************************
 * R-DOCUMENTATION ************************************************
 **********************************************************************
   NAME TIME_OPT_STRUCT

   TYPE  typedef

   DESCRIPTION  This structure holds pointers to and actual data used to 
   convert times to/from strings

   ARGUMENTS
      IARG  month_names     length 12 array of month names
      IARG  month_abbs      length 12 array of month abbreviations
      IARG  day_names       length 7 array of day names
      IARG  day_abbs        length 7 array of day abbreviations
      IARG  am_pm           length 2 array for printing AM/PM
      IARG  century         the current century (e.g. 1900)
      IARG  zone            the default time zone for input

   RETURN 

   ALGORITHM 

   EXCEPTIONS 

   NOTE 

**********************************************************************/
typedef struct topt_struct
{
  char **month_names;
  char **month_abbs;
  char **day_names;
  char **day_abbs;
  char **am_pm;
  Sint  century;
  char *zone;
} TIME_OPT_STRUCT;
  
int checkClass(SEXP x, const char **valid, const int P);
SEXP getListElement(SEXP list, const char *str);

#ifdef __cplusplus
}
#endif

#endif /* TIMELIB_TIMEUTILS_H_ */
