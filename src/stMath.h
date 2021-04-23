/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
 *************************************************************************/

#ifndef TIMELIB_STMATH_H
#define TIMELIB_STMATH_H

#include "timeUtils.h"
#include "timeObj.h"
#include "timeFuns.h"
#include "zoneObj.h"
#include "zoneFuns.h"
#include "relTime.h"
#include <string.h>

SEXP time_floor( SEXP time_vec, SEXP zone_list );
SEXP time_ceiling( SEXP time_vec, SEXP zone_list );
SEXP time_time_add( SEXP time1, SEXP time2, 
		     SEXP sign, SEXP ret_class );
SEXP time_num_op( SEXP time_vec, SEXP num_vec, 
		   SEXP op);
SEXP time_range( SEXP time_vec, SEXP na_rm );
SEXP time_sum( SEXP time_vec, SEXP na_rm, SEXP cum );
SEXP time_rel_add( SEXP time_vec, SEXP rel_strs, 
		    SEXP hol_vec, SEXP zone_list);
SEXP time_rel_seq( SEXP start_time, SEXP end_time,
		    SEXP len_vec, SEXP has_len,
		    SEXP rel_strs, SEXP hol_vec,
		    SEXP zone_list);


#endif  // TIMELIB_STMATH_H
