/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
 *************************************************************************/

#ifndef TIMELIB_TIMEFORMAT_H
#define TIMELIB_TIMEFORMAT_H

#include "timeUtils.h"
#include "timeObj.h"
#include "mdy.h"
#include <ctype.h>

int new_out_format( const char *old_format, char **new_format, 
		    int abb_size, int full_size, int zone_size );
int new_in_format( const char *old_format, char **new_format );
int mdyt_format( TIME_DATE_STRUCT td_input, const char *format_string, 
		 TIME_OPT_STRUCT topt, char *ret_string );
int mdyt_input( const char *input_string, char *format_string, 
		TIME_OPT_STRUCT topt, TIME_DATE_STRUCT *td_output );


#endif  // TIMELIB_TIMEFORMAT_H
