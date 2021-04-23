/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
*************************************************************************/

#ifndef TIMELIB_TIMEFUNS_H
#define TIMELIB_TIMEFUNS_H

#include "timeUtils.h"
#include "timeObj.h"
#include "timeFormat.h"
#include "timeSpanFormat.h"
#include "mdy.h"

SEXP time_to_string( SEXP time_vec, SEXP opt_list, 
		     SEXP zone_list );
SEXP time_from_string( SEXP char_vec, SEXP format_string,
		       SEXP opt_list, SEXP zone_list );
SEXP tspan_to_string( SEXP time_vec );
SEXP tspan_from_string( SEXP char_vec, SEXP format_string );
SEXP time_to_month_day_year( SEXP time_vec, 
			     SEXP zone_list );
SEXP time_from_month_day_year( SEXP month_vec, 
			       SEXP day_vec, 
			       SEXP year_vec );
SEXP time_to_year_day( SEXP time_vec, SEXP zone_list );
SEXP time_from_month_day_index( SEXP month, SEXP wkday, 
				SEXP index, SEXP year_vec );
SEXP time_easter( SEXP year_vec );
SEXP time_to_hour_min_sec( SEXP time_vec, SEXP zone_list );
SEXP time_from_hour_min_sec( SEXP hour_vec, SEXP min_vec, 
			     SEXP sec_vec, SEXP ms_vec );
SEXP time_to_numeric( SEXP time_vec );
SEXP time_from_numeric( SEXP num_vec, SEXP ret_class );
SEXP time_to_weekday( SEXP time_vec, SEXP zone_list );
SEXP time_to_zone( SEXP time_vec, SEXP zone, 
		   SEXP zone_list );

int jms_to_struct( Sint julian, Sint ms, 
		   TIME_DATE_STRUCT *td_output );


#endif  // TIMELIB_TIMEFUNS_H
