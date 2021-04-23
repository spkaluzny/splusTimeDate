/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
 *************************************************************************/

/*************************************************************************
 *
 * It contains C code utility functions for time zones.
 * It was formerly implemented using the RogueWave C++ tools for the
 * time zone class, but is pure C code now.
 *  
 * See zoneFuns.h for a more compact listing of the included functions.
 * They were written as auxiliary functions for R code
 * dealing with times and dates.
 * 
 * References: 

   Most important:
   Daylight rules taken from tzdata code found in
      ftp://elsie.nci.nih.gov/pub/
   This file has been updated to match the following data file version:
      File: tzdata2006g.tar.gz 	150 KB 	5/8/2006 	9:18:00 PM
   This file should be periodically updated to the latest tzcode version.
   The C code for time zones was built to easily use the information
   from the tables of time zones in this ftp archive.

   Good reference on North American time zones and daylight savings
   http://www.astro.virginia.edu/~eww6n/astro/TimeZone.html

*************************************************************************/

#include "zoneFuns.h"
#include <string.h>

/* internal functions -- defined and documented at bottom of file */
static TZONE_STRUCT *built_in_from_name(const char *mixed_name);
static int get_offset( TIME_DATE_STRUCT tstruc, int in_local_time,
		       TZONE_STRUCT *tzone, Sint *offset, int *is_daylight );
static int julian_from_tzcode( TZONE_CODE code, Sint month, Sint day,
			Sint xday, Sint year, Sint *julian );

/* there is also a huge amount of tabular information about time zones
   stored in static variables just before the internal functions */

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME find_zone

   DESCRIPTION  Return the time zone object with the given name.

   ARGUMENTS
      IARG  name        name matching an entry in the zone list
      IARG  zone_list   named list of time zones

   RETURN Returns a pointer to the time zone object with the given name,
   or NULL if not found.

   ALGORITHM Calls function find_zone_info to use R name matching to find 
   the named entry from the R time zone list.  If the entry is a built-in
   C time zone, the built_in_from_name function is used to find a pointer
   to the built-in time zone.  Otherwise, it is an R time zone, which is
   not currently supported and the function will return NULL.

   EXCEPTIONS 

   NOTE See also: GMT_from_zone, GMT_to_zone

**********************************************************************/
TZONE_STRUCT *find_zone( const char *name, SEXP zone_list )
{
  void *zone_info;
  int is_R;


  if( !name || !zone_list )
    return NULL;
  
  /* find the zone in the zone list */
  if( !find_zone_info( name, zone_list, &zone_info, &is_R )){
    warning("Can't find zone info for ", name);
    return NULL;
  }

  if( is_R ) /* it's an R time zone -- returned a zone struct */
    return (TZONE_STRUCT *) zone_info;

  /* otherwise it returned the built-in name, so find the zone ptr */
  return( built_in_from_name( (char *) zone_info ));
}

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME R_get_timezone_data

   DESCRIPTION  For given timezone name, return R objects
      that can be passed to R function timezoneR to recreate
      this timezone.  (Intent is to make updating time zone info
      possible without recompiling C code.)

   ARGUMENTS
      IARG  name        name matching an entry in the zone list
      IARG  zone_list   named list of time zones (output of timeZoneList())

   RETURN Returns a named list with 2 components(offset: integer scalar,
      rules: data.frame) that can be used in do.call("timeZoneR",retval).
      If no such name, return blt_in_NULL.
 */

SEXP R_get_timezone_data(SEXP r_name, SEXP zone_list)
{
  const char *name = CHAR(r_name) ;
  TZONE_STRUCT *tz ;
  TZONE_RULE_STRUCT *tzrule ; Sint nrules ;
  SEXP r_retval, r_offset, r_rules, r_rules_names, r_tmp ;
  SEXP r_yearfrom, r_yearto, r_hasdaylight, r_dsextra, r_monthstart, r_codestart, r_daystart, r_xdaystart, r_timestart, r_monthend, r_codeend, r_dayend, r_xdayend, r_timeend ;
  Sint *yearfrom, *yearto, *hasdaylight, *dsextra, *monthstart, *codestart, *daystart, *xdaystart, *timestart, *monthend, *codeend, *dayend, *xdayend, *timeend ;
  Sint i;
  if (!name || !name[0])
     return R_NilValue;
  tz = find_zone(name, zone_list);
  if (!tz)
     return R_NilValue;
  r_offset = PROTECT(NEW_INTEGER(tz->offset));
  for(tzrule = tz->rule, nrules=0;tzrule;tzrule=tzrule->prev_rule) {
     nrules++ ;
  }
  r_rules = PROTECT(NEW_LIST(14)) ;
  r_rules_names = PROTECT(NEW_CHARACTER(14L)) ;
  i = 0 ;

#undef ADD_ITEM
#define  ADD_ITEM(what) r_##what = PROTECT(NEW_INTEGER(nrules)); what = INTEGER_POINTER(r_##what) ; SET_VECTOR_ELT(r_rules, i, r_##what) ; SET_STRING_ELT(r_rules_names, i,  mkChar(#what)) ; i++
  ADD_ITEM(yearfrom) ;
  ADD_ITEM(yearto) ;
  ADD_ITEM(hasdaylight) ;
  ADD_ITEM(dsextra) ;
  ADD_ITEM(monthstart) ;
  ADD_ITEM(codestart) ;
  ADD_ITEM(daystart) ;
  ADD_ITEM(xdaystart) ;
  ADD_ITEM(timestart) ;
  ADD_ITEM(monthend) ;
  ADD_ITEM(codeend) ;
  ADD_ITEM(dayend) ;
  ADD_ITEM(xdayend) ;
  ADD_ITEM(timeend) ;
#undef ADD_ITEM
  setAttrib(r_rules, R_NamesSymbol, r_rules_names) ;
  r_retval = PROTECT(NEW_LIST(2L));
  SET_ELEMENT(r_retval, 0, r_offset);
  SET_ELEMENT(r_retval, 1, r_rules);
  r_tmp = PROTECT(NEW_CHARACTER(2L)) ;
  SET_STRING_ELT(r_tmp, 0, mkChar("offset"));
  SET_STRING_ELT(r_tmp, 1, mkChar("rules"));
  setAttrib(r_retval, R_NamesSymbol, r_tmp);
  for(tzrule = tz->rule;tzrule;tzrule=tzrule->prev_rule) {
    nrules-- ;
    yearfrom[nrules] = tzrule->yearfrom ;
    yearto[nrules] = tzrule->yearto ;
    hasdaylight[nrules] = tzrule->hasdaylight ;
    dsextra[nrules] = tzrule->dsextra ;
    monthstart[nrules] = tzrule->monthstart ;
    switch(tzrule->codestart){
      case CODE_MONTHDAY:     codestart[nrules] = 1 ; break;
      case CODE_LAST_WEEKDAY: codestart[nrules] = 2 ; break;
      case CODE_WEEKDAY_GE:   codestart[nrules] = 3 ; break;
      case CODE_WEEKDAY_LE:   codestart[nrules] = 4 ; break;
      default:                codestart[nrules] = 666; break;
    }
    daystart[nrules] = tzrule->daystart;
    xdaystart[nrules] = tzrule->xdaystart;
    timestart[nrules] = tzrule->timestart;
    monthend[nrules] = tzrule->monthend;
    switch(tzrule->codeend){
      case CODE_MONTHDAY:     codeend[nrules] = 1 ; break;
      case CODE_LAST_WEEKDAY: codeend[nrules] = 2 ; break;
      case CODE_WEEKDAY_GE:   codeend[nrules] = 3 ; break;
      case CODE_WEEKDAY_LE:   codeend[nrules] = 4 ; break;
      default:                codeend[nrules] = 666; break;
    }
    dayend[nrules] = tzrule->dayend ;
    xdayend[nrules] = tzrule->xdayend ;
    timeend[nrules] = tzrule->timeend ;
  }
  UNPROTECT(19);
  return r_retval;
}

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME GMT_to_zone

   DESCRIPTION  Convert from GMT to local zone time

   ARGUMENTS
      IOARG  tstruc  Time/date structure to convert
      IARG   tzone   Time zone object

   RETURN Returns 1/0 for success/failure

   ALGORITHM This function calls get_offset to find the 
   local time zone offset from GMT, and adds this offset 
   to the time/date using the add_offset function.  It also
   sets the daylight member of the time/date structure 
   correctly.
   
   EXCEPTIONS 

   NOTE  Leap seconds are not taken into account.
   \\
   \\
   See also: GMT_from_zone, find_zone

**********************************************************************/
int GMT_to_zone( TIME_DATE_STRUCT *tstruc, TZONE_STRUCT *tzone )
{
  Sint zone_offset = 0;

  if( !tstruc || !tzone )
    return 0;

  /* Figure out the zone offset */

  if( !get_offset( *tstruc, 0, tzone, &zone_offset, &(tstruc->daylight) ))
    return 0;

  /* add the zone offset to the time structure */
  return( add_offset( tstruc, zone_offset ));
}

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME GMT_from_zone

   DESCRIPTION  Convert to GMT from local zone time

   ARGUMENTS
      IOARG  tstruc  Time/date structure to convert
      IARG   tzone   Time zone object

   RETURN Returns 1/0 for success/failure

   ALGORITHM This function calls get_offset to find the local time
   zone offset from GMT, and subtracts this offset from the time/date
   using the add_offset function. The daylight member of the time
   structure is used to resolve ambiguities between daylight and
   non-daylight time.

   EXCEPTIONS 

   NOTE  Leap seconds are not taken into account.
   \\
   \\
   See also: GMT_to_zone, find_zone

**********************************************************************/
int GMT_from_zone( TIME_DATE_STRUCT *tstruc, TZONE_STRUCT *tzone )
{
  Sint zone_offset = 0;
  int tmp;

  if( !tstruc || !tzone )
    return 0;

  /* Figure out the zone offset */

  if( !get_offset( *tstruc, 1, tzone, &zone_offset, &tmp ))
    return 0;

  /* add the zone offset to the time structure */
  return( add_offset( tstruc, - zone_offset ));
}


/*****************************
  Time zone definitions.
 ****************************/

/* 
   Time zone daylight boundaries.  From the data files of the time
   zone code in the ftp://elsie.nci.nih.gov/pub/ tzdata file
   (see top comment for version)

   We provide daylight savings time zone rules from this source for 
   the following countries/areas which do observe daylight time:
       1967 and beyond, US
       1974 and beyond, Canada
       1976 and beyond, New Zealand
       1973 and beyond, Australia
       1972 and beyond, Great Britain
       1977 and beyond, European Union
       1965 and beyond, Hong Kong
       1970 and beyond, Singapore
   Exceptions and Notes: 
      Singapore has no daylight time, but was 1/2 hour different time 
   until May 1982, so its "daylight" time zone includes that. 

   Prior to the dates given, the daylight-observing time zones will
   follow the first given accurate year's rules. 

   We also provide standard time zones without daylight time.
 */

/* Standard time zones without daylight time */
/* These are in order around the globe */

static TZONE_STRUCT NewZealand_st = { 12 * 3600, NULL };
static TZONE_STRUCT Caroline_st = { 11 * 3600, NULL };
static TZONE_STRUCT EAustralia_st = { 10 * 3600, NULL };
static TZONE_STRUCT Japan_st = { 9 * 3600, NULL };
static TZONE_STRUCT China_st = { 8 * 3600, NULL };
static TZONE_STRUCT Saigon_st = { 7 * 3600, NULL };
static TZONE_STRUCT Kazakh_st = { 6 * 3600, NULL };
static TZONE_STRUCT Pakistan_st = { 5 * 3600, NULL };
static TZONE_STRUCT Caspian_st = { 4 * 3600, NULL };
static TZONE_STRUCT Moscow_st = { 3 * 3600, NULL };
static TZONE_STRUCT EEurope_st = { 2 * 3600, NULL };
static TZONE_STRUCT CEurope_st = { 1 * 3600, NULL };
static TZONE_STRUCT UTC = { 0, NULL };
static TZONE_STRUCT Azores_st = { -1 * 3600, NULL };
static TZONE_STRUCT Oscar_st = { -2 * 3600, NULL };
static TZONE_STRUCT Greenland_st = { -3 * 3600, NULL };
static TZONE_STRUCT Atlantic_st = { -4 * 3600, NULL };
static TZONE_STRUCT Eastern_st = { -5 * 3600, NULL };
static TZONE_STRUCT Central_st = { -6 * 3600, NULL };
static TZONE_STRUCT Mountain_st = { -7 * 3600, NULL };
static TZONE_STRUCT Pacific_st = { -8 * 3600, NULL };
static TZONE_STRUCT Alaska_st = { -9 * 3600, NULL };
static TZONE_STRUCT Hawaii_st = { -10 * 3600, NULL };
static TZONE_STRUCT Samoa_st = { -11 * 3600, NULL };

/* US daylight rules since 1967*/
/* Excerpt from ftp://elsie.nci.nih.gov/pub/ tzdata file (see top comment for version):
# Rule	NAME	FROM	TO	TYPE	IN	ON	AT	SAVE	LETTER
Rule	US	1967	1973	-	Apr	lastSun	2:00	1:00	D
Rule	US	1974	only	-	Jan	6	2:00	1:00	D
Rule	US	1975	only	-	Feb	23	2:00	1:00	D
Rule	US	1976	1986	-	Apr	lastSun	2:00	1:00	D
Rule	US	1987	2006	-	Apr	Sun>=1	2:00	1:00	D
Rule	US	2007	max	-	Mar	Sun>=8	2:00	1:00	D

Rule	US	1967	2006	-	Oct	lastSun	2:00	0	S
Rule	US	2007	max	-	Nov	Sun>=1	2:00	0	S
*/

/* So, all switches at 2AM wall clock time; always add 1 hour for daylight */
/* Last Sunday in April - Last Sunday in October 1967 to 1973 */
/* Jan 6 - Last Sunday in October  in 1974 only */
/* Feb 23 - Last Sunday in October  in 1975 only */
/* Last Sunday in April - Last Sunday in October 1976 - 1986 */
/* Sunday in April >= 1st - Last Sunday in October 1986-2006 */
/* Sunday in March >= 8th - Sunday on November >= 1st 2007-onward */

static TZONE_RULE_STRUCT USRuleOld = 
  { NULL, -1, 1973, 1, 3600, 
    4,  CODE_LAST_WEEKDAY, 0, 0, 2 * 3600,  /* last Sun in Apr, 2AM std */
    10, CODE_LAST_WEEKDAY, 0, 0, 1 * 3600 };/* last Sun in Oct, 2AM dst */
static TZONE_RULE_STRUCT USRule1974 = 
  { &USRuleOld, 1974, 1974, 1, 3600, 
    1,  CODE_MONTHDAY, 6, 0, 2 * 3600,      /* Jan 6, 2AM std */
    10, CODE_LAST_WEEKDAY, 0, 0, 1 * 3600 };/* last Sun in Oct, 2AM dst */
static TZONE_RULE_STRUCT USRule1975 = 
  { &USRule1974, 1975, 1975, 1, 3600, 
    2,  CODE_MONTHDAY, 23, 0, 2 * 3600,     /* Feb 23, 2AM std */
    10, CODE_LAST_WEEKDAY, 0, 0, 1 * 3600 };/* last Sun in Oct, 2AM dst */
static TZONE_RULE_STRUCT USRule1976 = 
  { &USRule1975, 1976, 1986, 1, 3600, 
    4,  CODE_LAST_WEEKDAY, 0, 0, 2 * 3600,  /* last Sun in Apr, 2AM std */
    10, CODE_LAST_WEEKDAY, 0, 0, 1 * 3600 };/* last Sun in Oct, 2AM dst */
static TZONE_RULE_STRUCT USRule1986 = 
  { &USRule1976, 1987, 2006, 1, 3600, 
    4,  CODE_WEEKDAY_GE, 0, 1, 2 * 3600,  /* Sun in Apr >= 1, 2AM std */
    10, CODE_LAST_WEEKDAY, 0, 0, 1 * 3600 };/* last Sun in Oct, 2AM dst */
static TZONE_RULE_STRUCT USRuleNow = 
  { &USRule1986, 2007, -1, 1, 3600, 
    3,  CODE_WEEKDAY_GE, 0, 8, 2 * 3600,  /* Sun in Mar >= 8, 2AM std */
    11, CODE_WEEKDAY_GE, 0, 1, 1 * 3600 };/* Sun on Nov >= 1, 2AM dst */

/* US zones */

static TZONE_STRUCT USEastern = { -5 * 3600, &USRuleNow };
static TZONE_STRUCT USCentral = { -6 * 3600, &USRuleNow };
static TZONE_STRUCT USMountain = { -7 * 3600, &USRuleNow };
static TZONE_STRUCT USPacific = { -8 * 3600, &USRuleNow };
static TZONE_STRUCT USAlaska = { -9 * 3600, &USRuleNow };
static TZONE_STRUCT USHawaii = { -10 * 3600, &USRuleNow };


/* Canada rules since 1974 */
/* Excerpt from ftp://elsie.nci.nih.gov/pub/ tzdata file (see top comment for version):
# Rule	NAME	FROM	TO	TYPE	IN	ON	AT	SAVE	LETTER
Rule	Canada	1974	1986	-	Apr	lastSun	2:00	1:00	D
Rule	Canada	1987	2006	-	Apr	Sun>=1	2:00	1:00	D
Rule	Canada	2007	max	-	Mar	Sun>=8	2:00	1:00	D

Rule	Canada	1974	2006	-	Oct	lastSun	2:00	0	S
Rule	Canada	2007	max	-	Nov	Sun>=1	2:00	0	S

Rule	StJohns	1951	1986	-	Apr	lastSun	2:00	1:00	D
Rule	StJohns	1987	only	-	Apr	Sun>=1	2:00	1:00	D
Rule	StJohns	1988	only	-	Apr	Sun>=1	2:00	2:00	DD
Rule	StJohns	1989	2006	-	Apr	Sun>=1	0:01	1:00	D
Rule	StJohns	2007	max	-	Mar	Sun>=8	0:01	1:00	D

Rule	StJohns	1960	1986	-	Oct	lastSun	2:00	0	S
Rule	StJohns	1987	2006	-	Oct	lastSun	0:01	0	S
Rule	StJohns	2007	max	-	Nov	Sun>=1	0:01	0	S
*/

/* Problem: Current code doesn't like falling back at local time 0:01,
   since this can switch back to the current day.  Therefore, I am going
   to encode the StJohns rules so they always fall back at 2:00. (mjs) */

/* So, for most of Canada, all switches at 2AM wall clock time, add 1 hr */
/* Last Sunday in April - Last Sunday in October 1974 - 1986 */
/* Sunday in April >= 1 - Last Sunday in October 1987 - 2006 */
/* Sunday in March >= 8 - Sunday in November >= 1  2007-onward */
/* Newfoundland had a 2 hour daylight time switch in 1988, so it has
   separate rules */

static TZONE_RULE_STRUCT CanRuleOld = 
  { NULL, -1, 1986, 1, 3600, 
    4,  CODE_LAST_WEEKDAY, 0, 0, 2 * 3600,  /* last Sun in Apr, 2AM std */
    10, CODE_LAST_WEEKDAY, 0, 0, 1 * 3600 };/* last Sun in Oct, 2AM dst */
static TZONE_RULE_STRUCT CanRule1987 = 
  { &CanRuleOld, 1987, 2006, 1, 3600, 
    4,  CODE_WEEKDAY_GE, 0, 1, 2 * 3600,  /* Sun in Apr >= 1, 2AM std */
    10, CODE_LAST_WEEKDAY, 0, 0, 1 * 3600 };/* last Sun in Oct, 2AM dst */
static TZONE_RULE_STRUCT CanRuleNow = 
  { &CanRule1987, 2007, -1, 1, 3600, 
    3,  CODE_WEEKDAY_GE, 0, 8, 2 * 3600,  /* Sun in Mar >= 8, 2AM std */
    11, CODE_WEEKDAY_GE, 0, 1, 1 * 3600 };/* Sun in Nov >= 1, 2AM dst */

static TZONE_RULE_STRUCT NewfRule1987 = 
  { &CanRuleOld, 1987, 1987, 1, 3600, 
    4,  CODE_WEEKDAY_GE, 0, 1, 2 * 3600,  /* Sun in Apr >=1, 2AM std */
    10, CODE_LAST_WEEKDAY, 0, 0, 1 * 3600 };/* last Sun in Oct, 2AM dst */
static TZONE_RULE_STRUCT NewfRule1988 = 
  { &NewfRule1987, 1988, 1988, 1, 2 * 3600, /* 2 hour shift! */
    4,  CODE_WEEKDAY_GE, 0, 1, 2 * 3600,  /* Sun in Apr >=1, 2AM std */
    10, CODE_LAST_WEEKDAY, 0, 0, 0 };/* last Sun in Oct, 2AM dst */
static TZONE_RULE_STRUCT NewfRule1989 = 
  { &NewfRule1988, 1989, 2006, 1, 3600,
    4,  CODE_WEEKDAY_GE, 0, 1, 60,  /* Sun in Apr >=1, 0:01AM std */
    10, CODE_LAST_WEEKDAY, 0, 0, 1 * 3600 };/* last Sun in Oct, 2AM dst */
static TZONE_RULE_STRUCT NewfRuleNow = 
  { &NewfRule1989, 2007, -1, 1, 3600, 
    3,  CODE_WEEKDAY_GE, 0, 8, 60,  /* Sun in Mar >= 8, 0:01AM std */
    11, CODE_WEEKDAY_GE, 0, 1, 1 * 3600 };/* Sun in Nov >= 1, 2AM dst */


/* Canadian zones */

/* Newfoundland standard time is 3:30 west of UTC */
static TZONE_STRUCT CanNewfoundland = { -3 * 3600 - 30 * 60, &NewfRuleNow };
static TZONE_STRUCT CanAtlantic = { -4 * 3600, &CanRuleNow };
static TZONE_STRUCT CanEastern = { -5 * 3600, &CanRuleNow };
static TZONE_STRUCT CanCentral = { -6 * 3600, &CanRuleNow };
static TZONE_STRUCT CanMountain = { -7 * 3600, &CanRuleNow };
static TZONE_STRUCT CanPacific = { -8 * 3600, &CanRuleNow };
static TZONE_STRUCT CanYukon = { -9 * 3600, &CanRuleNow };


/* New Zealand daylight rules since 1976*/
/* Excerpt from ftp://elsie.nci.nih.gov/pub/ tzdata file (see top comment for version):
# Rule	NAME	FROM	TO	TYPE	IN	ON	AT	SAVE	LETTER
Rule	NZ	1975	1988	-	Oct	lastSun	2:00s	1:00	D
Rule	NZ	1989	only	-	Oct	Sun>=8	2:00s	1:00	D
Rule	NZ	1990	max	-	Oct	Sun>=1	2:00s	1:00	D

Rule	NZ	1976	1989	-	Mar	Sun>=1	2:00s	0	S
Rule	NZ	1990	max	-	Mar	Sun>=15	2:00s	0	S

*/

/* So, change at 2AM std time, 1 hour time change */
/* 1976 - 1988: Last sunday in October, to Sunday in March>=1 */
/* 1989: Sunday in October>=8 to Sunday in March>=15 1990 */
/* 1990 and after: Sunday in October>=1 to Sunday in March >=15 */

static TZONE_RULE_STRUCT NZRuleOld = 
  { NULL, -1, 1988, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600, /* last Sun in Oct, 2AM std */
    3,  CODE_WEEKDAY_GE, 0, 1, 2 * 3600 }; /* Sun in Mar >=1, 2AM std */
static TZONE_RULE_STRUCT NZRule1989 = 
  { &NZRuleOld, 1989, 1989, 1, 3600, 
    10, CODE_WEEKDAY_GE, 0, 8, 2 * 3600,     /* Sun in Oct >=8, 2AM std */
    3,  CODE_WEEKDAY_GE, 0, 1, 2 * 3600 }; /* Sun in Mar >=1, 2AM std */
static TZONE_RULE_STRUCT NZRuleNow = 
  { &NZRule1989, 1990, -1, 1, 3600, 
    10, CODE_WEEKDAY_GE, 0, 1, 2 * 3600,    /* Sun in Oct >=1, 2AM std */
    3,  CODE_WEEKDAY_GE, 0, 15, 2 * 3600 }; /* Sun in Mar >=15, 2AM std */

/* New Zealand time zone */

static TZONE_STRUCT NewZealand = { 12 * 3600, &NZRuleNow };

/* Australia daylight rules -- several areas -- since 1973 */
/* Note that changeovers happen at 2AM **standard** time */
/* Excerpt from ftp://elsie.nci.nih.gov/pub/ tzdata file (see top comment for version):
# Rule	NAME	FROM	TO	TYPE	IN	ON	AT	SAVE	LETTER
# New South Wales (+10 hrs offset )
Rule	AN	1971	1985	-	Oct	lastSun	2:00s	1:00	-
Rule	AN	1986	only	-	Oct	19	2:00s	1:00	-
Rule	AN	1987	1999	-	Oct	lastSun	2:00s	1:00	-
Rule	AN	2000	only	-	Aug	lastSun	2:00s	1:00	-
Rule	AN	2001	max	-	Oct	lastSun	2:00s	1:00	-

Rule	AN	1973	1981	-	Mar	Sun>=1	2:00s	0	-
Rule	AN	1982	only	-	Apr	Sun>=1	2:00s	0	-
Rule	AN	1983	1985	-	Mar	Sun>=1	2:00s	0	-
Rule	AN	1986	1989	-	Mar	Sun>=15	2:00s	0	-
Rule	AN	1990	1995	-	Mar	Sun>=1	2:00s	0	-
Rule	AN	1996	2005	-	Mar	lastSun	2:00s	0	-
Rule	AN	2006	only	-	Apr	Sun>=1	2:00s	0	-
Rule	AN	2007	max	-	Mar	lastSun	2:00s	0	-


# Tasmania  (+10 hrs offset )
# Same as NSW until 1982.
Rule	AT	1968	1985	-	Oct	lastSun	2:00s	1:00	-
Rule	AT	1986	only	-	Oct	Sun>=15	2:00s	1:00	-
Rule	AT	1987	only	-	Oct	Sun>=22	2:00s	1:00	-
Rule	AT	1988	1990	-	Oct	lastSun	2:00s	1:00	-
Rule	AT	1991	1999	-	Oct	Sun>=1	2:00s	1:00	-
Rule	AT	2000	only	-	Aug	lastSun	2:00s	1:00	-
Rule	AT	2001	max	-	Oct	Sun>=1	2:00s	1:00	-

Rule	AT	1973	1981	-	Mar	Sun>=1	2:00s	0	-
Rule	AT	1982	1983	-	Mar	lastSun	2:00s	0	-
Rule	AT	1984	1986	-	Mar	Sun>=1	2:00s	0	-
Rule	AT	1987	1990	-	Mar	Sun>=15	2:00s	0	-
Rule	AT	1991	2005	-	Mar	lastSun	2:00s	0	-
Rule	AT	2006	only	-	Apr	Sun>=1	2:00s	0	-
Rule	AT	2007	max	-	Mar	lastSun	2:00s	0	-

# South Australia (+9:30 offset)
Rule	AS	1971	1985	-	Oct	lastSun	2:00s	1:00	-
Rule	AS	1986	only	-	Oct	19	2:00s	1:00	-
Rule	AS	1987	max	-	Oct	lastSun	2:00s	1:00	-

Rule	AS	1973	1985	-	Mar	Sun>=1	2:00s	0	-
Rule	AS	1986	1989	-	Mar	Sun>=15	2:00s	0	-
Rule	AS	1990	only	-	Mar	Sun>=18	2:00s	0	-
Rule	AS	1991	only	-	Mar	Sun>=1	2:00s	0	-
Rule	AS	1992	only	-	Mar	Sun>=18	2:00s	0	-
Rule	AS	1993	only	-	Mar	Sun>=1	2:00s	0	-
Rule	AS	1994	only	-	Mar	Sun>=18	2:00s	0	-
Rule	AS	1995	2005	-	Mar	lastSun	2:00s	0	-
Rule	AS	2006	only	-	Apr	Sun>=1	2:00s	0	-
Rule	AS	2007	max	-	Mar	lastSun	2:00s	0	-

# Victoria  (+10 hrs offset)
# Same as South Australia through 1985
Rule	AV	1971	1985	-	Oct	lastSun	2:00s	1:00	-
Rule	AV	1986	1987	-	Oct	Sun>=15	2:00s	1:00	-
Rule	AV	1988	1999	-	Oct	lastSun	2:00s	1:00	-
Rule	AV	2000	only	-	Aug	lastSun	2:00s	1:00	-
Rule	AV	2001	max	-	Oct	lastSun	2:00s	1:00	-

Rule	AV	1973	1985	-	Mar	Sun>=1	2:00s	0	-
Rule	AV	1986	1990	-	Mar	Sun>=15	2:00s	0	-
Rule	AV	1991	1994	-	Mar	Sun>=1	2:00s	0	-
Rule	AV	1995	2005	-	Mar	lastSun	2:00s	0	-
Rule	AV	2006	only	-	Apr	Sun>=1	2:00s	0	-
Rule	AV	2007	max	-	Mar	lastSun	2:00s	0	-

# Zone	NAME		GMTOFF	RULES	FORMAT	[UNTIL]
# Western Australia
Zone Australia/Perth	 7:43:24 -	LMT	1895 Dec
			 8:00	Aus	WST	1943 Jul
			 8:00	-	WST	1974 Oct lastSun 2:00s
			 8:00	1:00	WST	1975 Mar Sun>=1 2:00s
			 8:00	-	WST	1983 Oct lastSun 2:00s
			 8:00	1:00	WST	1984 Mar Sun>=1 2:00s
			 8:00	-	WST	1991 Nov 17 2:00s
			 8:00	1:00	WST	1992 Mar Sun>=1 2:00s
			 8:00	-	WST
*/


/* So, New South Wales: */

static TZONE_RULE_STRUCT NSWRuleOld = 
  { NULL, -1, 1981, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600, /* last Sun in Oct, 2AM std */
    3,  CODE_WEEKDAY_GE, 0, 1, 2 * 3600 }; /* Sun in Mar >=1, 2AM std */
static TZONE_RULE_STRUCT NSWRule1982 = 
  { &NSWRuleOld, 1982, 1982, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600, /* last Sun in Oct, 2AM std */
    4,  CODE_WEEKDAY_GE, 0, 1, 2 * 3600 }; /* Sun in Apr >=1, 2AM std */
static TZONE_RULE_STRUCT NSWRule1983 = 
  { &NSWRule1982, 1983, 1985, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600, /* last Sun in Oct, 2AM std */
    3,  CODE_WEEKDAY_GE, 0, 1, 2 * 3600 }; /* Sun in Mar >=1, 2AM std */
static TZONE_RULE_STRUCT NSWRule1986 = 
  { &NSWRule1983, 1986, 1986, 1, 3600, 
    10, CODE_MONTHDAY, 19, 0, 2 * 3600,     /* Oct 19, 2AM std */
    3,  CODE_WEEKDAY_GE, 0, 15, 2 * 3600 }; /* Sun in Mar >=15, 2AM std */
static TZONE_RULE_STRUCT NSWRule1987 = 
  { &NSWRule1986, 1987, 1989, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600, /* last Sun in Oct, 2AM std */
    3,  CODE_WEEKDAY_GE, 0, 15, 2 * 3600 }; /* Sun in Mar >=15, 2AM std */
static TZONE_RULE_STRUCT NSWRule1990 = 
  { &NSWRule1987, 1990, 1995, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600, /* last Sun in Oct, 2AM std */
    3,  CODE_WEEKDAY_GE, 0, 1, 2 * 3600 }; /* Sun in Mar >=1, 2AM std */
static TZONE_RULE_STRUCT NSWRule1996 = 
  { &NSWRule1990, 1996, 1999, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600, /* last Sun in Oct, 2AM std */
    3,  CODE_LAST_WEEKDAY, 0, 0, 2 * 3600 }; /* last Sun in Mar, 2AM std */
static TZONE_RULE_STRUCT NSWRule2000 = 
  { &NSWRule1996, 2000, 2000, 1, 3600, 
    8, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600, /* last Sun in Aug, 2AM std */
    3,  CODE_LAST_WEEKDAY, 0, 0, 2 * 3600 }; /* last Sun in Mar, 2AM std */
static TZONE_RULE_STRUCT NSWRule2001 = 
  { &NSWRule2000, 2001, 2005, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600, /* last Sun in Oct, 2AM std */
    3,  CODE_LAST_WEEKDAY, 0, 0, 2 * 3600 }; /* last Sun in Mar, 2AM std */
static TZONE_RULE_STRUCT NSWRule2006 = 
  { &NSWRule2001, 2006, 2006, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600, /* last Sun in Oct, 2AM std */
    4,  CODE_WEEKDAY_GE, 0, 1, 2 * 3600 }; /* Sun in Apr >=1, 2AM std */
static TZONE_RULE_STRUCT NSWRuleNow = 
  { &NSWRule2006, 2007, -1, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600, /* last Sun in Oct, 2AM std */
    3,  CODE_LAST_WEEKDAY, 0, 0, 2 * 3600 }; /* last Sun in Mar, 2AM std */

/* Tasmania */
static TZONE_RULE_STRUCT TasRule1982 = 
  { &NSWRuleOld, 1982, 1983, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600,   /* last Sun in Oct, 2AM std */
    3,  CODE_LAST_WEEKDAY, 0, 0, 2 * 3600 }; /* last Sun in Mar, 2AM std */
static TZONE_RULE_STRUCT TasRule1984 = 
  { &TasRule1982, 1984, 1985, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600, /* last Sun in Oct, 2AM std */
    3,  CODE_WEEKDAY_GE, 0, 1, 2 * 3600 }; /* Sun in Mar >=1, 2AM std */
static TZONE_RULE_STRUCT TasRule1986 = 
  { &TasRule1984, 1986, 1986, 1, 3600, 
    10, CODE_WEEKDAY_GE, 0, 15, 2 * 3600,  /* Sun in Oct >=15, 2AM std */
    3,  CODE_WEEKDAY_GE, 0, 1, 2 * 3600 }; /* Sun in Mar >=1, 2AM std */
static TZONE_RULE_STRUCT TasRule1987 = 
  { &TasRule1986, 1987, 1987, 1, 3600, 
    10, CODE_WEEKDAY_GE, 0, 22, 2 * 3600,  /* Sun in Oct >= 22, 2AM std */
    3,  CODE_WEEKDAY_GE, 0, 15, 2 * 3600 }; /* Sun in Mar >=15, 2AM std */
static TZONE_RULE_STRUCT TasRule1988 = 
  { &TasRule1987, 1988, 1990, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600,  /* last Sun in Oct, 2AM std */
    3,  CODE_WEEKDAY_GE, 0, 15, 2 * 3600 }; /* Sun in Mar >=15, 2AM std */
static TZONE_RULE_STRUCT TasRule1991 = 
  { &TasRule1988, 1991, 1999, 1, 3600, 
    10, CODE_WEEKDAY_GE, 0, 1, 2 * 3600,  /* Sun in Oct >= 1, 2AM std */
    3,  CODE_LAST_WEEKDAY, 0, 0, 2 * 3600 }; /* last Sun in Mar, 2AM std */
static TZONE_RULE_STRUCT TasRule2000 = 
  { &TasRule1991, 2000, 2000, 1, 3600, 
    8, CODE_LAST_WEEKDAY, 0, 1, 2 * 3600,  /* last Sun in Aug, 2AM std */
    3,  CODE_LAST_WEEKDAY, 0, 0, 2 * 3600 }; /* last Sun in Mar, 2AM std */
static TZONE_RULE_STRUCT TasRule2001 = 
  { &TasRule2000, 2001, 2005, 1, 3600, 
    10, CODE_WEEKDAY_GE, 0, 1, 2 * 3600,     /* Sun in Oct >=1, 2AM std */
    3,  CODE_LAST_WEEKDAY, 0, 0, 2 * 3600 }; /* last Sun in Mar, 2AM std */
static TZONE_RULE_STRUCT TasRule2006 = 
  { &TasRule2001, 2006, 2006, 1, 3600, 
    10, CODE_WEEKDAY_GE, 0, 1, 2 * 3600,     /* Sun in Oct >=1, 2AM std */
    4,  CODE_WEEKDAY_GE, 0, 1, 2 * 3600 };   /* Sun in Apr >=1, 2AM std */
static TZONE_RULE_STRUCT TasRuleNow = 
  { &TasRule2006, 2007, -1, 1, 3600, 
    10, CODE_WEEKDAY_GE, 0, 1, 2 * 3600,     /* Sun in Oct >=1, 2AM std */
    3,  CODE_LAST_WEEKDAY, 0, 0, 2 * 3600 }; /* last Sun in Mar, 2AM std */


/* South Australia */
static TZONE_RULE_STRUCT SARuleOld = 
  { NULL, -1, 1985, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600, /* last Sun in Oct, 2AM std */
    3,  CODE_WEEKDAY_GE, 0, 1, 2 * 3600 }; /* Sun in Mar >=1, 2AM std */
static TZONE_RULE_STRUCT SARule1986 = 
  { &SARuleOld, 1986, 1986, 1, 3600, 
    10, CODE_MONTHDAY, 19, 0, 2 * 3600,    /* Oct 19, 2AM std */
    3,  CODE_WEEKDAY_GE, 0, 15, 2 * 3600 }; /* Sun in Mar >=15, 2AM std */
static TZONE_RULE_STRUCT SARule1987 = 
  { &SARule1986, 1987, 1989, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600,  /* last Sun in Oct, 2AM std */
    3,  CODE_WEEKDAY_GE, 0, 15, 2 * 3600 }; /* Sun in Mar >=15, 2AM std */
static TZONE_RULE_STRUCT SARule1990 = 
  { &SARule1987, 1990, 1990, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600,  /* last Sun in Oct, 2AM std */
    3,  CODE_WEEKDAY_GE, 0, 18, 2 * 3600 }; /* Sun in Mar >=18, 2AM std */
static TZONE_RULE_STRUCT SARule1991 = 
  { &SARule1990, 1991, 1991, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600,  /* last Sun in Oct, 2AM std */
    3,  CODE_WEEKDAY_GE, 0, 1, 2 * 3600 };  /* Sun in Mar >=1, 2AM std */
static TZONE_RULE_STRUCT SARule1992 = 
  { &SARule1991, 1992, 1992, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600,  /* last Sun in Oct, 2AM std */
    3,  CODE_WEEKDAY_GE, 0, 18, 2 * 3600 }; /* Sun in Mar >=18, 2AM std */
static TZONE_RULE_STRUCT SARule1993 = 
  { &SARule1992, 1993, 1993, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600,  /* last Sun in Oct, 2AM std */
    3,  CODE_WEEKDAY_GE, 0, 1, 2 * 3600 };  /* Sun in Mar >=1, 2AM std */
static TZONE_RULE_STRUCT SARule1994 = 
  { &SARule1993, 1994, 1994, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600,  /* last Sun in Oct, 2AM std */
    3,  CODE_WEEKDAY_GE, 0, 18, 2 * 3600 }; /* Sun in Mar >=18, 2AM std */
static TZONE_RULE_STRUCT SARule1995 = 
  { &SARule1994, 1995, 2005, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600,   /* last Sun in Oct, 2AM std */
    3,  CODE_LAST_WEEKDAY, 0, 0, 2 * 3600 }; /* last Sun in Mar, 2AM std */
static TZONE_RULE_STRUCT SARule2006 = 
  { &SARule1995, 2006, 2006, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600,   /* last Sun in Oct, 2AM std */
    4,  CODE_WEEKDAY_GE, 0, 1, 2 * 3600 }; /* Sun in Apr >=1, 2AM std */
static TZONE_RULE_STRUCT SARuleNow = 
  { &SARule2006, 2007, -1, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600,   /* last Sun in Oct, 2AM std */
    3,  CODE_LAST_WEEKDAY, 0, 0, 2 * 3600 }; /* last Sun in Mar, 2AM std */

/* Victoria */
static TZONE_RULE_STRUCT VictRule1986 = 
  { &SARuleOld, 1986, 1987, 1, 3600, 
    10, CODE_WEEKDAY_GE, 0, 15, 2 * 3600,   /* Sun in Oct >= 15, 2AM std */
    3,  CODE_WEEKDAY_GE, 0, 15, 2 * 3600 }; /* Sun in Mar >=15, 2AM std */
static TZONE_RULE_STRUCT VictRule1988 = 
  { &VictRule1986, 1988, 1990, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600,  /* last Sun in Oct, 2AM std */
    3,  CODE_WEEKDAY_GE, 0, 15, 2 * 3600 }; /* Sun in Mar >=15, 2AM std */
static TZONE_RULE_STRUCT VictRule1991 = 
  { &VictRule1988, 1991, 1994, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600,  /* last Sun in Oct, 2AM std */
    3,  CODE_WEEKDAY_GE, 0, 1, 2 * 3600 };  /* Sun in Mar >=1, 2AM std */
static TZONE_RULE_STRUCT VictRule1995 = 
  { &VictRule1991, 1995, 1999, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600,  /* last Sun in Oct, 2AM std */
    3,  CODE_LAST_WEEKDAY, 0, 0, 2 * 3600 }; /* last Sun in Mar, 2AM std */
static TZONE_RULE_STRUCT VictRule2000 = 
  { &VictRule1995, 2000, 2000, 1, 3600, 
    8, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600,  /* last Sun in Aug, 2AM std */
    3,  CODE_LAST_WEEKDAY, 0, 0, 2 * 3600 }; /* last Sun in Mar, 2AM std */
static TZONE_RULE_STRUCT VictRule2001 = 
  { &VictRule2000, 2001, 2005, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600,   /* last Sun in Oct, 2AM std */
    3,  CODE_LAST_WEEKDAY, 0, 0, 2 * 3600 }; /* last Sun in Mar, 2AM std */
static TZONE_RULE_STRUCT VictRule2006 = 
  { &VictRule2001, 2006, 2006, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600,   /* last Sun in Oct, 2AM std */
    4,  CODE_WEEKDAY_GE, 0, 1, 2 * 3600 };   /* Sun in Apr >=1, 2AM std */
static TZONE_RULE_STRUCT VictRuleNow = 
  { &VictRule2006, 2007, -1, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600,   /* last Sun in Oct, 2AM std */
    3,  CODE_LAST_WEEKDAY, 0, 0, 2 * 3600 }; /* last Sun in Mar, 2AM std */

/* Western Australia */
/* Currently has no daylight time, but has a few times recently: */
/* Last Sun Oct 1974 to 1st Sun Mar 1975 */
/* Last Sun Oct 1983 to 1st Sun Mar 1984 */
/* Nov 17 1991 to 1st Sun Mar 1992 */

static TZONE_RULE_STRUCT WARuleOld = 
  { NULL, -1, 1973, 0, 0, 0,0,0,0,0, 0,0,0,0,0 }; /* no DST */
static TZONE_RULE_STRUCT WARule1974 = 
  { &WARuleOld, 1974, 1974, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600, /* last Sun in Oct, 2AM std */
    1,  CODE_MONTHDAY, 1, 0, 0 };          /* Start the year off DST, end on */
static TZONE_RULE_STRUCT WARule1975 = 
  { &WARule1974, 1975, 1975, 1, 3600, 
    1,  CODE_MONTHDAY, 1, 0, 0,           /* Start the year on DST, end off */
    3, CODE_WEEKDAY_GE, 0, 1, 2 * 3600 }; /* Sun in Mar >=1, 2AM std */
static TZONE_RULE_STRUCT WARule1976 = 
  { &WARule1975, 1976, 1982, 0, 0, 0,0,0,0,0, 0,0,0,0,0 }; /* no DST */
static TZONE_RULE_STRUCT WARule1983 = 
  { &WARule1976, 1983, 1983, 1, 3600, 
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600, /* last Sun in Oct, 2AM std */
    1,  CODE_MONTHDAY, 1, 0, 0 };          /* Start the year off DST, end on */
static TZONE_RULE_STRUCT WARule1984 = 
  { &WARule1983, 1984, 1984, 1, 3600, 
    1, CODE_MONTHDAY, 1, 0, 0,            /* Start the year on DST, end off */
    3, CODE_WEEKDAY_GE, 0, 1, 2 * 3600 }; /* Sun in Mar >=1, 2AM std */
static TZONE_RULE_STRUCT WARule1985 = 
  { &WARule1984, 1985, 1990, 0, 0, 0,0,0,0,0, 0,0,0,0,0 }; /* no DST */
static TZONE_RULE_STRUCT WARule1991 = 
  { &WARule1985, 1991, 1991, 1, 3600, 
    11, CODE_MONTHDAY, 17, 0, 2 * 3600, /* Nov 17, 2AM std */
    1,  CODE_MONTHDAY, 1, 0, 0 };       /* Start the year off DST, end on */
static TZONE_RULE_STRUCT WARule1992 = 
  { &WARule1991, 1992, 1992, 1, 3600, 
    1, CODE_MONTHDAY, 1, 0, 0,            /* Start the year on DST, end off */
    3, CODE_WEEKDAY_GE, 0, 1, 2 * 3600 }; /* Sun in Mar >=1, 2AM std */
static TZONE_RULE_STRUCT WARuleNow = 
  { &WARule1992, 1993, -1, 0, 0, 0,0,0,0,0, 0,0,0,0,0 }; /* no DST */

/* Now, the time zones in Australia since 1972. */

/* Queensland does not observe daylight savings, so no zone */

/*  There is a zone for Tasmania */
static TZONE_STRUCT AustTasmania = { 10 * 3600, &TasRuleNow };

/* There is a zone for NSW */
static TZONE_STRUCT AustNSW = { 10 * 3600, &NSWRuleNow };

/* There is a zone for Victoria */
static TZONE_STRUCT AustVictoria = { 10 * 3600, &VictRuleNow };

/* Northern territory: 9:30 East of GMT always, no daylight since 1940s */
static TZONE_STRUCT CAustralia_st = { 9 * 3600 + 30 * 60, NULL };

/* South Australia: same as Northern Territory, but with daylight savings */
static TZONE_STRUCT AustSouth = { 9 * 3600 + 30 * 60, &SARuleNow };

/* Western Australia */
static TZONE_STRUCT AustWestern = { 8 * 3600, &WARuleNow };


/* Great Britain since 1972 */
/* Excerpt from ftp://elsie.nci.nih.gov/pub/ tzdata file (see top comment for version):
# Rule	NAME	FROM	TO	TYPE	IN	ON	AT	SAVE	LETTER
Rule	GB-Eire	1972	1980	-	Mar	Sun>=16	2:00s	1:00	BST
Rule	GB-Eire	1981	1995	-	Mar	lastSun	1:00u	1:00	BST

Rule	GB-Eire	1972	1980	-	Oct	Sun>=23	2:00s	0	GMT
Rule	GB-Eire 1981	1989	-	Oct	Sun>=23	1:00u	0	GMT
Rule	GB-Eire 1990	1995	-	Oct	Sun>=22	1:00u	0	GMT

# See EU for rules starting in 1996.
Rule	EU	1981	max	-	Mar	lastSun	 1:00u	1:00	S

Rule	EU	1996	max	-	Oct	lastSun	 1:00u	0	-
*/

static TZONE_RULE_STRUCT GBRuleOld = 
  { NULL, -1, 1980, 1, 3600, 
    3,  CODE_WEEKDAY_GE, 0, 16, 2 * 3600,  /* Sun Mar >= 16, 2AM std */
    10, CODE_WEEKDAY_GE, 0, 23, 2 * 3600 };/* Sun Oct >= 23, 2AM std */
static TZONE_RULE_STRUCT GBRule1981 = 
  { &GBRuleOld, 1981, 1989, 1, 3600, 
    3,  CODE_LAST_WEEKDAY, 0, 0, 1 * 3600,  /* Last Sun Mar, 1AM std */
    10, CODE_WEEKDAY_GE, 0, 23, 1 * 3600 }; /* Sun Oct >= 23, 1AM std */
static TZONE_RULE_STRUCT GBRule1990 = 
  { &GBRule1981, 1990, 1995, 1, 3600, 
    3,  CODE_LAST_WEEKDAY, 0, 0, 1 * 3600,  /* Last Sun Mar, 1AM std */
    10, CODE_WEEKDAY_GE, 0, 22, 1 * 3600 };/* Sun Oct >= 22, 1AM std */
static TZONE_RULE_STRUCT GBRuleNow = 
  { &GBRule1990, 1996, -1, 1, 3600, 
    3,  CODE_LAST_WEEKDAY, 0, 0, 1 * 3600,  /* Last Sun Mar, 1AM std */
    10, CODE_LAST_WEEKDAY, 0, 0, 1 * 3600 };/* Last Sun Oct, 1AM std */


/* EU rules since 1977 */
/* Excerpt from ftp://elsie.nci.nih.gov/pub/ tzdata file (see top comment for version):
# Rule	NAME	FROM	TO	TYPE	IN	ON	AT	SAVE	LETTER
Rule	EU	1977	1980	-	Apr	Sun>=1	 1:00u	1:00	S
Rule	EU	1981	max	-	Mar	lastSun	 1:00u	1:00	S

Rule	EU	1977	only	-	Sep	lastSun	 1:00u	0	-
Rule	EU	1978	only	-	Oct	 1	 1:00u	0	-
Rule	EU	1979	1995	-	Sep	lastSun	 1:00u	0	-
Rule	EU	1996	max	-	Oct	lastSun	 1:00u	0	-
*/
/* Note that EU switches at 1AM GMT, which means we need separate rules
   for the 3 time zones under E.U. rules */

static TZONE_RULE_STRUCT WERuleOld = 
  { NULL, -1, 1977, 1, 3600, 
    4, CODE_WEEKDAY_GE, 0, 1, 1 * 3600,    /* Sun Apr >=1, 1AM std */
    9, CODE_LAST_WEEKDAY, 0, 0, 1 * 3600 };/* Last Sun Sept, 1AM std */
static TZONE_RULE_STRUCT WERule1978 = 
  { &WERuleOld, 1978, 1978, 1, 3600, 
    4, CODE_WEEKDAY_GE, 0, 1, 1 * 3600,  /* Sun Apr >=1, 1AM std */
    10, CODE_MONTHDAY, 1, 0, 1 * 3600 }; /* Oct 1, 1AM std */
static TZONE_RULE_STRUCT WERule1979 = 
  { &WERule1978, 1979, 1980, 1, 3600, 
    4, CODE_WEEKDAY_GE, 0, 1, 1 * 3600,    /* Sun Apr >=1, 1AM std */
    9, CODE_LAST_WEEKDAY, 0, 0, 1 * 3600 };/* Last Sun Sept, 1AM std */
static TZONE_RULE_STRUCT WERule1981 = 
  { &WERule1979, 1981, 1995, 1, 3600, 
    3, CODE_LAST_WEEKDAY, 0, 0, 1 * 3600,  /* Last Sun March, 1AM std */
    9, CODE_LAST_WEEKDAY, 0, 0, 1 * 3600 };/* Last Sun Sept, 1AM std */
static TZONE_RULE_STRUCT WERuleNow = 
  { &WERule1981, 1996, -1, 1, 3600, 
    3, CODE_LAST_WEEKDAY, 0, 0, 1 * 3600,   /* Last Sun March, 1AM std */
    10, CODE_LAST_WEEKDAY, 0, 0, 1 * 3600 };/* Last Sun Oct, 1AM std */

static TZONE_RULE_STRUCT CERuleOld = 
  { NULL, -1, 1977, 1, 3600, 
    4, CODE_WEEKDAY_GE, 0, 1, 2 * 3600,    /* Sun Apr >=1, 2AM std */
    9, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600 };/* Last Sun Sept, 2AM std */
static TZONE_RULE_STRUCT CERule1978 = 
  { &CERuleOld, 1978, 1978, 1, 3600, 
    4, CODE_WEEKDAY_GE, 0, 1, 2 * 3600,  /* Sun Apr >=1, 2AM std */
    10, CODE_MONTHDAY, 1, 0, 2 * 3600 }; /* Oct 1, 2AM std */
static TZONE_RULE_STRUCT CERule1979 = 
  { &CERule1978, 1979, 1980, 1, 3600, 
    4, CODE_WEEKDAY_GE, 0, 1, 2 * 3600,    /* Sun Apr >=1, 2AM std */
    9, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600 };/* Last Sun Sept, 2AM std */
static TZONE_RULE_STRUCT CERule1981 = 
  { &CERule1979, 1981, 1995, 1, 3600, 
    3, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600,  /* Last Sun March, 2AM std */
    9, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600 };/* Last Sun Sept, 2AM std */
static TZONE_RULE_STRUCT CERuleNow = 
  { &CERule1981, 1996, -1, 1, 3600, 
    3, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600,   /* Last Sun March, 2AM std */
    10, CODE_LAST_WEEKDAY, 0, 0, 2 * 3600 };/* Last Sun Oct, 2AM std */

static TZONE_RULE_STRUCT EERuleOld = 
  { NULL, -1, 1977, 1, 3600, 
    4, CODE_WEEKDAY_GE, 0, 1, 3 * 3600,    /* Sun Apr >=1, 3AM std */
    9, CODE_LAST_WEEKDAY, 0, 0, 3 * 3600 };/* Last Sun Sept, 3AM std */
static TZONE_RULE_STRUCT EERule1978 = 
  { &EERuleOld, 1978, 1978, 1, 3600, 
    4, CODE_WEEKDAY_GE, 0, 1, 3 * 3600,  /* Sun Apr >=1, 3AM std */
    10, CODE_MONTHDAY, 1, 0, 3 * 3600 }; /* Oct 1, 3AM std */
static TZONE_RULE_STRUCT EERule1979 = 
  { &EERule1978, 1979, 1980, 1, 3600, 
    4, CODE_WEEKDAY_GE, 0, 1, 3 * 3600,    /* Sun Apr >=1, 3AM std */
    9, CODE_LAST_WEEKDAY, 0, 0, 3 * 3600 };/* Last Sun Sept, 3AM std */
static TZONE_RULE_STRUCT EERule1981 = 
  { &EERule1979, 1981, 1995, 1, 3600, 
    3, CODE_LAST_WEEKDAY, 0, 0, 3 * 3600,  /* Last Sun March, 3AM std */
    9, CODE_LAST_WEEKDAY, 0, 0, 3 * 3600 };/* Last Sun Sept, 3AM std */
static TZONE_RULE_STRUCT EERuleNow = 
  { &EERule1981, 1996, -1, 1, 3600, 
    3, CODE_LAST_WEEKDAY, 0, 0, 3 * 3600,   /* Last Sun March, 3AM std */
    10, CODE_LAST_WEEKDAY, 0, 0, 3 * 3600 };/* Last Sun Oct, 3AM std */

/* European time zones */

static TZONE_STRUCT Britain = { 0, &GBRuleNow };
static TZONE_STRUCT WEurope = { 0, &WERuleNow };
static TZONE_STRUCT CEurope = { 1 * 3600, &CERuleNow };
static TZONE_STRUCT EEurope = { 2 * 3600, &EERuleNow };


/* Hong Kong since 1965 */
/* Excerpt from ftp://elsie.nci.nih.gov/pub/ tzdata file (see top comment for version):

# Rule	NAME	FROM	TO	TYPE	IN	ON	AT	SAVE	LETTER
Rule	HK	1965	1977	-	Apr	Sun>=16	3:30	1:00	S
Rule	HK	1979	1980	-	May	Sun>=8	3:30	1:00	S

Rule	HK	1965	1977	-	Oct	Sun>=16	3:30	0	-
Rule	HK	1979	1980	-	Oct	Sun>=16	3:30	0	-
*/
/* Apparently Hong Kong has had no daylight time since 1981, or in 1978. */

static TZONE_RULE_STRUCT HKRuleOld = 
  { NULL, -1, 1977, 1, 3600, 
    4, CODE_WEEKDAY_GE, 0, 16, 3*3600 + 30*60, /* Sun Apr >=16, 3:30 AM std */
    9, CODE_WEEKDAY_GE, 0, 16, 2*3600 + 30*60};/* Sun Oct >=16, 2:30 AM std */
static TZONE_RULE_STRUCT HKRule1978 = 
  { &HKRuleOld, 1978, 1978, 0, 0, 0,0,0,0,0, 0,0,0,0,0 };
static TZONE_RULE_STRUCT HKRule1979 = 
  { &HKRule1978, 1979, 1980, 1, 3600, 
    5, CODE_WEEKDAY_GE, 0, 8, 3*3600 + 30*60,  /* Sun May >=8, 3:30 AM std */
    9, CODE_WEEKDAY_GE, 0, 16, 2*3600 + 30*60};/* Sun Oct >=16, 2:30 AM std */
static TZONE_RULE_STRUCT HKRuleNow = 
  { &HKRule1979, 1981, -1, 0, 0, 0,0,0,0,0, 0,0,0,0,0 };

/* Hong Kong time zone */
static TZONE_STRUCT HongKong = { 8 * 3600, &HKRuleNow };

/* Singapore since 1965 */
/* Excerpt from ftp://elsie.nci.nih.gov/pub/ tzdata file (see top comment for version):

# Singapore
# Zone	NAME		GMTOFF	RULES	FORMAT	[UNTIL]
Zone	Asia/Singapore	7:30	-	MALT	1965 Aug  9 # independence
			7:30	-	SGT	1982 Jan 1 # Singapore Time
			8:00	-	SGT

Singapore was apparently on 7:30 offset from GMT until Jan of 1982,
   after which they changed to 8:00.  We'll implement this as a DST
   rule that is on 1/2 hour DST until Jan 1982, and then no DST afterwards */

static TZONE_RULE_STRUCT SingRuleOld = 
  { NULL, -1, 1981, 1, - 30 * 60,        /* -30 minute offset */
    1, CODE_MONTHDAY, 1, 0, 0,           /* Start Jan 1 */
   12, CODE_MONTHDAY, 31, 0, 24 * 3600}; /* end Dec 31 */
static TZONE_RULE_STRUCT SingRuleNow = 
  { &SingRuleOld, 1982, -1, 0, 0, 0,0,0,0,0, 0,0,0,0,0 };

/* Singapore time zone */
static TZONE_STRUCT Singapore = { 8 * 3600, &SingRuleNow };

/* Table of time zone names for lookup.  One name per defined zone */
static struct _zonename_ 
{
  const char *name;
  TZONE_STRUCT *ptr;
} zones[] = {

{ "newzealand", &NewZealand },
{ "st/newzealand", &NewZealand_st },

{ "st/caroline", &Caroline_st },

{ "st/eaustralia", &EAustralia_st },
{ "aust/nsw", &AustNSW },
{ "aust/tasmania", &AustTasmania },
{ "aust/victoria", &AustVictoria },

{ "st/caustralia", &CAustralia_st },
{ "aust/south", &AustSouth },

{ "st/japan", &Japan_st },

{ "st/china", &China_st },
{ "aust/western", &AustWestern },
{ "hongkong", &HongKong },
{ "singapore", &Singapore },

{ "st/saigon", &Saigon_st },

{ "st/kazakh", &Kazakh_st },

{ "st/pakistan", &Pakistan_st },

{ "st/caspian", &Caspian_st },

{ "st/moscow", &Moscow_st },

{ "st/eeurope", &EEurope_st },
{ "europe/east", &EEurope },

{ "st/ceurope", &CEurope_st },
{ "europe/central", &CEurope },

{ "utc", &UTC },
{ "britain", &Britain },
{ "europe/west", &WEurope },

{ "st/azores", &Azores_st },

{ "st/oscar", &Oscar_st },

{ "st/wgreenland", &Greenland_st },

{ "can/newfoundland", &CanNewfoundland },

{ "can/atlantic", &CanAtlantic },
{ "st/atlantic", &Atlantic_st },

{ "us/eastern", &USEastern },
{ "can/eastern", &CanEastern },
{ "st/eastern", &Eastern_st },

{ "us/central", &USCentral },
{ "can/central", &CanCentral },
{ "st/central", &Central_st },

{ "us/mountain", &USMountain },
{ "can/mountain", &CanMountain },
{ "st/mountain",  &Mountain_st },

{ "us/pacific", &USPacific },
{ "can/pacific", &CanPacific },
{ "st/pacific", &Pacific_st },

{ "us/alaska", &USAlaska },
{ "can/yukon", &CanYukon },
{ "st/alaska", &Alaska_st },

{ "us/hawaii", &USHawaii },
{ "st/hawaii", &Hawaii_st },

{ "st/samoa", &Samoa_st },

};

/**********************
  Internal functions
  *********************/

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME built_in_from_name

   DESCRIPTION  Return the built-in time zone object with the given name.

   ARGUMENTS
      IARG  mixed_name  mixed-case input name

   RETURN Returns a pointer to the time zone object with the given name,
   or NULL if not found.

   ALGORITHM Uses case-insensitive matching to find the time zone
   name.  Currently defined time zones are listed in the R documentation
   on time zones. 

   EXCEPTIONS 

   NOTE This function is a modified version of a function in Prediction
   Company's PCTime.cpp file.
   \\
   \\
   See also: find_zone

**********************************************************************/
static TZONE_STRUCT *built_in_from_name(const char *mixed_name)
{
  /* don't care about matching beyond 50 characters */

  char name[50];
  int i, len;

  if( !mixed_name )
    return NULL;

  strncpy( name, mixed_name, 49 );
  name[49] = '\0' ;

  for( i = 0; i < 50; i++ )
    /*LINTED: this cast is definitely OK*/
    name[i] = (char) tolower( name[i] );

  len = sizeof(zones)/sizeof(struct _zonename_);
  for (i = 0; i < len; i++) 
   {
     if (strcmp(name, zones[i].name) == 0) 
       return zones[i].ptr;
   }

   return NULL;
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME get_offset

   DESCRIPTION  Find the offset from GMT to the local time zone,
   in seconds. For example, Eastern US standard time would return
   -5 hours, which is what you have to add to the GMT time to get
   the local time there.

   ARGUMENTS
      IARG  tstruc         structure giving input time/date
      IARG  in_local_time  1 if tstruc is in local time, 0 if in GMT
      IARG  tzone          the local time zone
      OARG  offset         seconds offset from GMT to local time
      OARG  is_daylight    1/0 for in/out of daylight time

   RETURN Returns 1/0 for success/failure.

   ALGORITHM The function first finds the time zone rule corresponding 
   to the input year by following the prev_rule pointers through the 
   time zone rules.  Then by comparing the dates and times of the
   time changes for that year, if any, with the input time (in 
   conjunction with the julian_from_mdy, julian_from_tzcode,
   and ms_from_hms functions), it determines whether it is daylight 
   or standard time, and returns the appropriate offset.

   EXCEPTIONS 

   NOTE This function assumes that changing from standard to/from daylight
   time never changes the date.  Also note that if local time is passed
   in, the given time of day may be either illegal or ambiguous if it
   is around the time change.  The daylight member of the input struct is
   used to resolve ambiguous times into daylight or standard time. Illegal
   times are tacitly ignored, so they'll be converted to legal times.
   \\
   \\
   See also: GMT_from_zone, GMT_to_zone

**********************************************************************/
static int get_offset( TIME_DATE_STRUCT tstruc, int in_local_time,
		       TZONE_STRUCT *tzone, Sint *offset, int *is_daylight )
{
  TZONE_RULE_STRUCT *cur_rule;
  Sint julstart, julend, julnow, msnow;

  if( !tzone || !offset || !is_daylight )
    return 0;

  /* set the offset to non-daylight value */
  *offset = tzone->offset;
  *is_daylight = 0;

  /* march through the time zone rules until we find the right year */
  cur_rule = tzone->rule;
  while( cur_rule )
  {
    if((( cur_rule->yearto == -1 ) || 
	( cur_rule->yearto >= tstruc.year )) &&
       (( cur_rule->yearfrom == -1 ) || 
	( cur_rule->yearfrom <= tstruc.year )))
      break;
    cur_rule = cur_rule->prev_rule;
  }

  if( !cur_rule ) /* didn't find one, so just return regular offset */
    return 1;


  /* see whether we're in daylight savings or not */

  /* first see if we actually have daylight savings time */
  if( !cur_rule->hasdaylight || !cur_rule->dsextra ) 
    return 1;
  
  /* convert input time to local **standard** time if it isn't in local time */
  /* note that this is a local copy of tstruc */
  /* also note that time of day for switching is always given in local
     standard time, not in standard/daylight */

  if( !in_local_time )
  {
    if( !add_offset( &tstruc, *offset ))
      return 0;
  }

  /* figure out the julian day for start, end, input */

  if( !julian_from_mdy( tstruc, &julnow ) ||
      !julian_from_tzcode( cur_rule->codestart, cur_rule->monthstart,
			   cur_rule->daystart, cur_rule->xdaystart,
			   tstruc.year, &julstart ) ||
      !julian_from_tzcode( cur_rule->codeend, cur_rule->monthend,
			   cur_rule->dayend, cur_rule->xdayend,
			   tstruc.year, &julend ))
    return 0;

  if(((( julnow < julstart ) || ( julnow > julend )) &&
      ( julstart <= julend )) ||  /* before start/after end of DST, north */
     ((( julnow > julend ) && ( julnow < julstart )) &&
      ( julstart >= julend ))) /* between end and start of DST, south */
    return 1; /* it is not DST */

  if(((( julnow < julend ) || ( julnow > julstart )) &&
      ( julstart >= julend )) || /* between start and end of DST, north */
     ((( julnow > julstart ) && ( julnow < julend )) &&
      ( julstart <= julend ))) /* before end/after start of DST, south */
  {
    /* it is DST, so add extra offset and return */
    *offset += cur_rule->dsextra;
    *is_daylight = 1;
    return 1;
  }

  /* If we get here, we must be on a daylight changing day, 
     so must check time */

  if( !ms_from_hms( tstruc, &msnow ))
    return 0;

  msnow /= 1000;

  /* Note that if we didn't originate in GMT, msnow has current local
     standard/daylight time that we are trying to convert. If we did
     originate in GMT, it has local standard time. 
     Also note that the time of day for switching is always given
     in local standard time, not daylight, in cur_rule. */

  if( julnow == julstart ) /* on the start day of DST */
  {
    /* On this day, time cur_rule->timestart (standard) jumps to 
       cur_rule->timestart + cur_rule->dsextra. Normally,
       dsextra > 0, so there are no duplicated times. The times
       between timestart and timestart + dsextra are illegal, but
       we'll conveniently ignore that. Check,
       just in case dsextra < 0, however, and break ties with 
       the tstruc.daylight parameter */

    if( in_local_time && cur_rule->dsextra < 0 &&
	msnow >= cur_rule->timestart + cur_rule->dsextra &&
	msnow < cur_rule->timestart ) {
      if( tstruc.daylight ) {
	*offset += cur_rule->dsextra;
	*is_daylight = 1;
	return 1;
      } else {
	return 0;
      }
    }


    if( msnow >= cur_rule->timestart ) {
      /* unambigously past start time, even if msnow was local 
         daylight time */
      *offset += cur_rule->dsextra;
      *is_daylight = 1;
      return 1;
    }

    /* haven't started */
    return 1;
  }

  /* if we get here we're on end boundary date */

  if( !in_local_time ) {
    /* in this case, our input was GMT, and we've converted it
       to local standard time, and our rule is in standard time,
       so we're OK to compare directly */

    if( msnow >= cur_rule->timeend ) return 1; /* we changed back */
    
    /* if we get here, we're before the change-back, so we're in daylight
       time */

    *offset += cur_rule->dsextra;
    *is_daylight = 1;
    return 1;
  }

  /* If we get here, in_local_time is true, so we passed in the wall clock
     time of day, which is now stored in msnow (seconds after midnight).
     This is possibly an ambiguous time: 
     on this day, time cur_rule->timeend + dsextra (wall clock time)
     jumps back to cur_rule->timeend, and then the times between
     cur_rule->timeend and cur_rule->timeend + dsextra are repeated. 
  */

  if( msnow >= cur_rule->timeend + cur_rule->dsextra ) {
    /* no ambiguity, and daylight time has ended for sure */
    return 1;
  }

  if( msnow < cur_rule->timeend ) {
    /* no ambiguity, still in daylight time */
    *offset += cur_rule->dsextra;
    *is_daylight = 1;
    return 1;
  }

  /* if we get here, we're in the ambigous hour. */

  if( !tstruc.daylight ) return 1;

  *offset += cur_rule->dsextra;
  *is_daylight = 1;
  return 1;

}

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME julian_from_tzcode

   DESCRIPTION  Find the julian day of a time change.

   ARGUMENTS
      IARG  code       code saying what the day and xday mean
      IARG  month      month of the time change
      IARG  day        day or weekday of time change
      IARG  xday       other day needed to calculate date
      IARG  year       year of the time change
      OARG  julian     julian day of the time change

   RETURN Returns 1/0 for success/failure.

   ALGORITHM Depending on the passed-in code, calls one or both of
   julian_from_mdy and julian_from_index to find the julian day number
   that is returned.

   EXCEPTIONS 

   NOTE See also get_offset, and notes under julian_from_mdy about what
   the julian day means.

**********************************************************************/
static int julian_from_tzcode( TZONE_CODE code, Sint month, Sint day,
			Sint xday, Sint year, Sint *julian )
{
  TIME_DATE_STRUCT tmp;
  Sint tmpjul;

  if( !julian )
    return 0;

  switch( code )
  {
  case CODE_MONTHDAY: /* day stands for day of month */
    tmp.year = year;
    tmp.month = month;
    tmp.day = day;
    return( julian_from_mdy( tmp, julian ));

  case CODE_LAST_WEEKDAY: /* day stands for weekday number, want last one */
    return( julian_from_index( month, day, -1, year, julian ));

  case CODE_WEEKDAY_GE:  /* day stands for weekday number, xday is day of month
		            and we want first of these weekdays >= xday */

    /* find the xday */
    tmp.year = year;
    tmp.month = month;
    tmp.day = xday;
    if( !julian_from_mdy( tmp, &tmpjul ))
      return 0;

    /* find the first of this weekday in month */
    if( !julian_from_index( month, day, 1, year, julian ))
      return 0;

    /* add some weeks to  julian to get past xday */

    tmpjul = 7 * (Sint) ceil(( tmpjul - *julian ) / 7.0 );
    if( tmpjul > 28 ) /* something bad happened */
      return 0;
    if( tmpjul > 0 )
      *julian += tmpjul;

    return 1;

  case CODE_WEEKDAY_LE:  /* day stands for weekday number, xday is day of month
		            and we want first of these weekdays <= xday */

    /* find the xday */
    tmp.year = year;
    tmp.month = month;
    tmp.day = xday;
    if( !julian_from_mdy( tmp, &tmpjul ))
      return 0;

    /* find the last of this weekday in month */
    if( !julian_from_index( month, day, -1, year, julian ))
      return 0;

    /* subtract some weeks to  julian to get before xday */

    tmpjul = 7 * (Sint) ceil(( *julian - tmpjul ) / 7.0 );
    if( tmpjul > 28 ) /* something bad happened */
      return 0;
    if( tmpjul > 0 )
      *julian -= tmpjul;

    return 1;
  default: 
    return 0;
  }
}

