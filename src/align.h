/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
*************************************************************************/

#ifndef TIMELIB_ALIGN_H
#define TIMELIB_ALIGN_H

#include "timeUtils.h"
#include "timeObj.h"
#include "zoneObj.h"
#include "timeFuns.h"
#include <string.h>

SEXP num_align( SEXP num_obj, SEXP align_pos, SEXP how_obj, SEXP match_tol );
SEXP time_align( SEXP time_obj, SEXP align_pos, SEXP how_obj, SEXP match_tol );


#endif  // TIMELIB_ALIGN_H
