/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
 *************************************************************************/

#ifndef TIMELIB_MDY_H
#define TIMELIB_MDY_H

#include "timeUtils.h"

int julian_from_mdy( TIME_DATE_STRUCT td_input, Sint *julian );
int julian_from_index( Sint month, Sint wkday, Sint index, Sint year, 
		       Sint *julian );
int julian_to_mdy( Sint julian,  TIME_DATE_STRUCT *td_output );
int julian_to_weekday( Sint julian );
int mdy_to_yday( TIME_DATE_STRUCT *td_input );
int julian_easter( Sint year, Sint *julian );
int ms_from_hms( TIME_DATE_STRUCT td_input, Sint *ms_ret );
int ms_to_hms( Sint ms, TIME_DATE_STRUCT *td_output );
int ms_from_fraction( double frac, Sint *ms );
int ms_to_fraction( Sint ms, double *frac );
int jms_to_struct( Sint julian, Sint ms, TIME_DATE_STRUCT *td_output );
int adjust_span( Sint *julian, Sint *ms );
int adjust_time( Sint *julian, Sint *ms );
Sint days_in_month( Sint month, Sint year );


#endif  // TIMELIB_MDY_H
