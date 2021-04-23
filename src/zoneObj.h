/*************************************************************************
 * $Id: //depot/Devel/splus/slocal/timeser/src/TIMEZONE.h#13 $
 *
 * It contains headers for formerly C++, now C code utility functions 
 * for dealing with 
 * dates in "julian" and month/day/year formats, as well as times in 
 * milliseconds since midnight and hour/minute/second/millisecond formats.
 * In particular, these are functions that depend on the time
 * zones, which were implemented using the RogueWave C++ library, but
 * are now implemented in tzone.c.  See
 * TIME.h for straight C functions for times and dates. 
 *  
 * The functions are defined and fully documented with autodoc in files
 * tzone.c and datemath.c.  They were written as auxiliary 
 * functions for S-Plus code dealing with times and dates.
 *
 * The "julian" format for dates is the number of days since January
 * 1, 1960, which is defined to be day 0. The calendar follows the 
 * conventions of the British Empire, which changed from Julian to 
 * Gregorian calendars in September of 1752.  Calendar dates prior to
 * 1920 were different in many countries.  See the "Calendar FAQ" posted
 * regularly to Usenet news groups soc.history, sci.astro, sci.answers,
 * soc.answers, and news.answers, and to a web site at
 * www.pip.dknet.dk/\~c-t/calendar.html for more information on the
 * history of calendars around the world.
 *
 *************************************************************************/
#ifndef TIMELIB_ZONEOBJ_H
#define TIMELIB_ZONEOBJ_H

#include "timeUtils.h"

/**********************************************************************
 * R-DOCUMENTATION ************************************************
 **********************************************************************
   NAME TZONE_CODE

   TYPE  enum

   DESCRIPTION  Code for the type of daylight savings rule -- tells
   what the day and xday members mean for start/end boundary.

   ARGUMENTS
   IARG CODE_MONTHDAY      day is day of month
   IARG CODE_LAST_WEEKDAY  day is weekday number, want last one of month
   IARG CODE_WEEKDAY_GE    day is weekday number, want first one on or after xday
   IARG CODE_WEEKDAY_LE    day is weekday number, want last one on or before xday

   RETURN 

   ALGORITHM 

   EXCEPTIONS 

   NOTE 


**********************************************************************/
typedef enum tzone_code
{
  CODE_MONTHDAY,
  CODE_LAST_WEEKDAY,
  CODE_WEEKDAY_GE,
  CODE_WEEKDAY_LE
} TZONE_CODE;

/**********************************************************************
 * R-DOCUMENTATION ************************************************
 **********************************************************************
   NAME TZONE_RULE_STRUCT

   TYPE  typedef

   DESCRIPTION  This structure encodes a set of rules which tell how
   and when to change to and from daylight savings time. 

   ARGUMENTS
   IARG  prev_rule     pointer to previous TZONE_RULE_STRUCT
   IARG  yearfrom      first year rule is in effect (-1 for min )
   IARG  yearto        last year rule is in effect ( -1 for max )
   IARG  hasdaylight   true if time zone has daylight time these years
   IARG  dsextra       additional seconds offset from GMT for daylight time
   IARG  monthstart    month to start daylight savings (1-12) 
   IARG  codestart     what do daystart and xdaystart mean 
   IARG  daystart      day of month (1-31) or weekday number (0 Sun - 6 Sat)
   IARG  xdaystart     other day for encoding if necessary 
   IARG  timestart     seconds after midnight local standard time to start
   IARG  monthend      month to end daylight savings (1-12) 
   IARG  codeend       what do dayend and xdayend mean 
   IARG  dayend        day of month (1-31) or weekday number (0 Sun - 6 Sat)
   IARG  xdayend       other day for encoding if necessary 
   IARG  timeend       seconds after midnight local standard time to end

   RETURN 

   ALGORITHM 

   EXCEPTIONS 

   NOTE 

**********************************************************************/
typedef struct tzone_rule
{
  struct tzone_rule *prev_rule;

  Sint yearfrom; 
  Sint yearto;   
  Sint hasdaylight;
  Sint dsextra;  

  Sint monthstart; 
  TZONE_CODE codestart;
  Sint daystart;        
  Sint xdaystart;       
  Sint timestart;      

  Sint monthend;        
  TZONE_CODE codeend;  
  Sint dayend;          
  Sint xdayend;         
  Sint timeend;        

} TZONE_RULE_STRUCT;


/**********************************************************************
 * R-DOCUMENTATION ************************************************
 **********************************************************************
   NAME TZONE_STRUCT

   TYPE  typedef

   DESCRIPTION  This structure stores information about a time zone.

   ARGUMENTS
   IARG  offset   seconds offset from GMT without daylight time
   IARG  rule     daylight savings rule for most recent time, NULL if none.

   RETURN 

   ALGORITHM 

   EXCEPTIONS 

   NOTE 

**********************************************************************/
typedef struct tzone_struct
{
  Sint offset;
  TZONE_RULE_STRUCT *rule;
} TZONE_STRUCT;


int find_zone_info( const char *name, SEXP zone_list, void **zone_info, 
		    int *is_R );

#endif /* TIMELIB_ZONEOBJ_H */
