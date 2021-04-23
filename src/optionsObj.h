/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
 *************************************************************************/

#ifndef TIMELIB_OPTIONSOBJ_H
#define TIMELIB_OPTIONSOBJ_H

#include "timeUtils.h"
#include "zoneObj.h"

/* parse the R time options structure */
int time_opt_parse( SEXP in_obj, TIME_OPT_STRUCT *out_struct );

/* calculate the max size of abbreviations and full names of months/days
   for use in calculating the size needed for allocating strings for time
   output */
void time_opt_sizes( TIME_OPT_STRUCT opt_struct, int *abb_size,
		     int *full_size );

#endif /* TIMELIB_OPTIONSOBJ_H */
