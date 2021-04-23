/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
 *************************************************************************/

#ifndef TIMELIB_TIMEOBJ_H
#define TIMELIB_TIMEOBJ_H

#include "timeUtils.h"
#include "timeFormat.h"
#include "timeSpanFormat.h"
#include "zoneObj.h"

/******************
 Functions in timeobj.c
 ******************/

/*
 * Access functions for the time class that get pointers to the various
 * R objects inside of it -- USE WITH CARE!
 *
 * They check to see if the passed in argument is a time or time span
 * class R object, and return NULL if not.
 *
 * Note that the "data" function gets a pointer to an R list containing
 * the julian day and millisecond vectors.
 */
SEXP time_format_pointer( SEXP time_obj );
SEXP time_zone_pointer( SEXP time_obj );
SEXP time_julian_pointer( SEXP time_obj );
SEXP time_ms_pointer( SEXP time_obj );
SEXP time_data_pointer( SEXP time_obj );

/*
 * Functions to extract all the parts of the object as c-readable data,
 * and to create a new time object and extract the time/date vectors.
 * Return 1/0 for success/failure.
 */
int time_get_pieces( SEXP time_obj, SEXP opt_obj,
			    Sint **day_vec, Sint **ms_vec,
			    Sint *vec_length, char **new_format,
			    char **zone, TIME_OPT_STRUCT *opt_struct );

int tspan_get_pieces( SEXP time_obj, Sint **day_vec,
				 Sint **ms_vec, Sint *vec_length,
				 char **format_string );

SEXP time_create_new( Sint new_length, Sint **day_data,
				      Sint **ms_data );
SEXP tspan_create_new( Sint new_length, Sint **day_data,
				      Sint **ms_data );
SEXP time_to_string( SEXP time_vec, SEXP opt_list, 
		      SEXP zone_list );
SEXP time_from_string( SEXP char_vec, SEXP format_string,
		       SEXP opt_list, SEXP zone_list );
SEXP tspan_to_string( SEXP time_vec );


/******************
 Functions in optobj.c
 ******************/

/* parse the R time options structure */
int time_opt_parse( SEXP in_obj, TIME_OPT_STRUCT *out_struct );

/* calculate the max size of abbreviations and full names of months/days
   for use in calculating the size needed for allocating strings for time
   output */
void time_opt_sizes( TIME_OPT_STRUCT opt_struct, int *abb_size,
		     int *full_size );

#endif /* TIMELIB_TIMEOBJ_H */
