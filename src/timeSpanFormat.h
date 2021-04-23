/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
 *************************************************************************/

#ifndef TIMELIB_TSPANFORMAT_H
#define TIMELIB_TSPANFORMAT_H

#include "timeUtils.h"
#include "mdy.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>


int tspan_format( const char *format_string, Sint julian, Sint ms,
		  char *ret_string );
int tspan_input( const char *input_string, const char *format_string, 
		 Sint *julian, Sint *ms );
int tspan_output_length( const char *format_string );

#endif  // TIMELIB_TSPANFFORMAT_H
