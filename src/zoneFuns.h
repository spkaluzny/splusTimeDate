/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
 *************************************************************************/

/*************************************************************************
 *
 * It contains headers for formerly C++, now C code utility functions 
 * for dealing with 
 * dates in "julian" and month/day/year formats, as well as times in 
 * milliseconds since midnight and hour/minute/second/millisecond formats.
 * In particular, these are functions that depend on the time
 * zones, which were implemented using the RogueWave C++ library, but
 * are now implemented in tzone.c.  See
 * timeZone.h for straight C functions for times and dates. 
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
#ifndef TIMELIB_ZONEFUNS_H
#define TIMELIB_ZONEFUNS_H

#include "zoneObj.h"
#include "mdy.h"
#include "dateMath.h"
#include <ctype.h>

int find_zone_info( const char *name, SEXP zone_list,
		    void **zone_info, int *is_R );

/* functions for converting from GMT to/from local zone time */
int GMT_to_zone( TIME_DATE_STRUCT *tstruc, TZONE_STRUCT *tzone );
int GMT_from_zone( TIME_DATE_STRUCT *tstruc, TZONE_STRUCT *tzone );

/* function to find the time zone from the time zone list */
TZONE_STRUCT *find_zone( const char *name, SEXP zone_list );


/***********************
 functions in datemath.c
 ***********************/

/* functions to find the floor/ceiling for dates.
   return true/false for success/failure */
/*
EXTERN_DEF int date_floor( Sint in_jul, Sint in_ms, TZONE_STRUCT *zone,
		       Sint *out_jul, Sint *out_ms );
EXTERN_DEF int date_ceil( Sint in_jul, Sint in_ms, TZONE_STRUCT *zone,
		       Sint *out_jul, Sint *out_ms );
*/
/* add seconds to a time structure, return 1/0 for success/failure */
/*
EXTERN_DEF int add_offset( TIME_DATE_STRUCT *tstruc, Sint secs_to_add );
*/
/******************
  functions in reltime.c
*****************/


/*
 * Function that adds a relative time to a time struct, considering
 * time zones.
 */
/*
EXTERN_DEF int rtime_add_with_zones( TIME_DATE_STRUCT *td, char 
				     *rt_str, Sint *hol_dates, 
				     Sint num_hols, TZONE_STRUCT *tzone );
*/

#endif /* TIMELIB_ZONEFUNS_H */
