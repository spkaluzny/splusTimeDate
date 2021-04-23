/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
*************************************************************************/

#ifndef TIMELIB_DATEMATH_H
#define TIMELIB_DATEMATH_H

#include "timeUtils.h"
#include "zoneObj.h"
#include "zoneFuns.h"
#include "mdy.h"

/* functions to find the floor/ceiling for dates.
   return true/false for success/failure */

int date_floor( Sint in_jul, Sint in_ms, TZONE_STRUCT *zone,
		Sint *out_jul, Sint *out_ms );
int date_ceil( Sint in_jul, Sint in_ms, TZONE_STRUCT *zone,
	       Sint *out_jul, Sint *out_ms );

/* add seconds to a time structure, return 1/0 for success/failure */

int add_offset( TIME_DATE_STRUCT *tstruc, Sint secs_to_add );

#endif /* TIMELIB_DATEMATH_H */
