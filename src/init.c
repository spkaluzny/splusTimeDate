/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
 *************************************************************************/

#include "timeUtils.h"
#include "timeObj.h"
#include "zoneObj.h"
#include "timeFuns.h"
#include "zoneFuns.h"
#include "stMath.h"
#include "align.h"
#include "Syms.h"

#include <R_ext/Rdynload.h>

#define CALLDEF(name, n)  {#name, (DL_FUNC) &name, n}

static R_CallMethodDef CallEntries[] = {
  // from Stime.c
  CALLDEF(time_to_string, 3),
  CALLDEF(time_from_string, 4),
  CALLDEF(tspan_to_string, 1),
  CALLDEF(tspan_from_string, 2),
  CALLDEF(time_to_month_day_year, 2),
  CALLDEF(time_from_month_day_year, 3),
  CALLDEF(time_from_month_day_index, 4),
  CALLDEF(time_easter, 1),
  CALLDEF(time_to_year_day, 2),
  CALLDEF(time_to_hour_min_sec, 2),
  CALLDEF(time_from_hour_min_sec, 4),
  CALLDEF(time_to_numeric, 1),
  CALLDEF(time_from_numeric, 2),
  CALLDEF(time_to_weekday, 2),
  CALLDEF(time_to_zone, 3),
  CALLDEF(time_floor, 2),
  CALLDEF(time_ceiling, 2),
  CALLDEF(time_time_add, 4),
  CALLDEF(time_num_op, 3),
  CALLDEF(time_range, 2),
  CALLDEF(time_sum, 3),
  CALLDEF(time_rel_add, 4),
  CALLDEF(time_rel_seq, 7),
  CALLDEF(num_align, 4),
  CALLDEF(time_align, 4),
  {NULL, NULL, 0}
};

void
#ifdef HAVE_VISIBILITY_ATTRIBUTE
__attribute__ ((visibility ("default")))
#endif
R_init_splusTimeDate(DllInfo *dll)
{
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);

/* These are callable from other packages' C code: */

#define RREGDEF(name)  R_RegisterCCallable("splusTimeDate", #name, (DL_FUNC) name)

    
    /* define the slots */
    splusTimeDate_HolidaysSym = install("holidays");
    splusTimeDate_DataSym = install("Data");
    splusTimeDate_ColumnsSym = install("columns");
    splusTimeDate_FormatSym = install("format");
    splusTimeDate_ZoneSym = install("time.zone");

    splusTimeDate_NS = R_FindNamespace(mkString("splusTimeDate"));
    if(splusTimeDate_NS == R_UnboundValue)
      error("missing 'splusTimeDate' namespace: should never happen");

#ifdef DEBUG_splusTimeDate
    if(isEnvironment(splusTimeDate_NS))
	Rprintf("splusTimeDate_NS: %s\n",
		CHAR(asChar(eval(lang2(install("format"),splusTimeDate_NS),
				 R_GlobalEnv))));
    else
#else
    if(!isEnvironment(splusTimeDate_NS))
#endif
	error("splusTimeDate namespace not determined correctly");
}

