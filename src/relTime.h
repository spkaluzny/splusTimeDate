/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
 *************************************************************************/

 #ifndef TIMELIB_RELTIME_H
#define TIMELIB_RELTIME_H

#include "timeUtils.h"
#include "timeObj.h"
#include "zoneObj.h"
#include "zoneFuns.h"

#include "timeFormat.h"
#include "timeSpanFormat.h"
#include "mdy.h"

#include <string.h>
#include <ctype.h>

int rtime_add( TIME_DATE_STRUCT *td, char *rt_str, Sint *hol_dates, 
	       Sint num_hols );
int rtime_add_with_zones( TIME_DATE_STRUCT *td, char *rt_str, Sint *hol_dates, 
			  Sint num_hols, TZONE_STRUCT *tzone );

#endif  // TIMELIB_RELTIME_H
