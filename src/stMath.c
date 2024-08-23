/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
 *************************************************************************/

/*************************************************************************
 *
 * It contains C code utility functions for performing math on 
 * R time and time span objects
 *
 * The exported functions here were written to be called with the 
 * .Call interface of R.  They include (see documentation in func headers):


*************************************************************************/

#include "stMath.h"

static const char *IS_TIME_CLASS[] = {
  TIME_CLASS_NAME
};

static const char *IS_TSPAN_CLASS[] = {
    TSPAN_CLASS_NAME
  };



/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME time_floor

   DESCRIPTION  Find new times corresponding to the input times, such that
   in the input times' time zone, the time of day is midnight and the 
   output times are no later than the input times.
   To be called from R as 
   \\
   {\tt 
   .Call("time_floor", time.vec, zone.list)
   }
   where TIMECLASS is replaced by the name of the time class.

   ARGUMENTS
      IARG  time_vec  The R time vector object
      IARG  zone_list The list of R time zone objects

   RETURN Returns a time vector object of the same length as the input time
   vector, containing the above described output times.

   ALGORITHM  For each input time, this function calculates the desired
   floor time using the date_floor function, in conjunction with the 
   time zone information found from the find_zone function.

   EXCEPTIONS 

   NOTE See also: time_ceiling

**********************************************************************/
SEXP time_floor( SEXP time_vec, SEXP zone_list )
{
  SEXP ret;
  Sint *in_days, *in_ms, *jul_data, *ms_data;
  char *zone;
  Sint i, lng;
  TZONE_STRUCT *tzone;
  
  /* get the desired parts of the time object */

  if( !time_get_pieces( time_vec, NULL, &in_days, &in_ms, &lng, NULL, 
			&zone, NULL ))
    error("Invalid argument in C function time_floor");

  tzone = find_zone( zone, zone_list );
  if( !tzone )
    error( "Unknown or unreadable time zone in C function time_floor" );

  /* create output time object and find pointers for data*/

  PROTECT(ret = time_create_new( lng, &jul_data, &ms_data ));
  if( !ret || !jul_data || !ms_data ){
    UNPROTECT(3);
    error( "Could not create new time object in c function time_floor" );
  }

  /* for each time, calculate floor */

  for( i = 0; i < lng; i++ )
  {
    if(  in_days[i] ==NA_INTEGER ||
	 in_ms[i] ==NA_INTEGER ||
	!date_floor( in_days[i], in_ms[i], tzone, &(jul_data[i]), 
		     &(ms_data[i] )))
    {
      /* error occurred -- put NA into return value */
      jul_data[i] = NA_INTEGER;
      ms_data[i] = NA_INTEGER;
    }
  }

  UNPROTECT(3);//1+2 from time_get_pieces
  return( ret );
}

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME time_ceiling

   DESCRIPTION  Find new times corresponding to the input times, such that
   in the input times' time zone, the time of day is midnight and the 
   output times are no earlier than the input times.
   To be called from R as 
   \\
   {\tt 
   .Call("time_ceiling", time.vec, zone.list)
   }
   where TIMECLASS is replaced by the name of the time class.

   ARGUMENTS
      IARG  time_vec  The R time vector object
      IARG  zone_list The list of R time zone objects

   RETURN Returns a time vector object of the same length as the input time
   vector, containing the above described output times.

   ALGORITHM  For each input time, this function calculates the desired
   ceiling time using the date_ceil function, in conjunction with the 
   time zone information found from find_zone.

   EXCEPTIONS 

   NOTE See also: time_floor

**********************************************************************/
SEXP time_ceiling( SEXP time_vec, SEXP zone_list )
{

  SEXP ret;
  Sint *in_days, *in_ms, *jul_data, *ms_data;
  char *zone;
  Sint i, lng;
  TZONE_STRUCT *tzone;
  
  /* get the desired parts of the time object */

  if( !time_get_pieces( time_vec, NULL, &in_days, &in_ms, &lng, NULL, 
			&zone, NULL ))
    error( "Invalid argument in C function time_ceiling" );

  tzone = find_zone( zone, zone_list );
  if( !tzone )
    error( "Unknown or unreadable time zone in C function time_ceiling" );

  /* create output time object and find pointers for data*/

  PROTECT(ret = time_create_new( lng, &jul_data, &ms_data ));
  if( !ret || !jul_data || !ms_data ){
    UNPROTECT(3);
    error( "Could not create new time object in c function time_ceiling" );
  }

  /* for each time, calculate floor */

  for( i = 0; i < lng; i++ )
  {
    if(  in_days[i] ==NA_INTEGER ||
	 in_ms[i] ==NA_INTEGER ||
	!date_ceil( in_days[i], in_ms[i], tzone, &(jul_data[i]), 
		     &(ms_data[i] )))
    {
      /* error occurred -- put NA into return value */
      jul_data[i] = NA_INTEGER;
      ms_data[i] = NA_INTEGER;
    }
  }

  UNPROTECT(3); //1+2 from time_get_pieces
  return( ret );
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME time_time_add

   DESCRIPTION  Add or subtract two time or time span objects.
   To be called from R as 
   \\
   {\tt 
   .Call("time_time_add", time1, time2, add.sign, ret.class)
   }
   where TIMECLASS is replaced by the name of the time or time
   span classes passed in those arguments.

   ARGUMENTS
      IARG  time1     The first R time or time span vector object
      IARG  time2     The second R time or time span vector object
      IARG  sign      Either +1. or -1., to add or subtract the second
      IARG  ret_class Return class, as a character string.

   RETURN Returns a time or time span vector (depending on ret_class) 
   that is the sum or difference of the input time and time span vectors.

   ALGORITHM  Each element of the second object is added to or subtracted
   from the corresponding element of the first object, by combining their
   days and milliseconds and then carrying milliseconds over into days
   as necessary using the adjust_time or adjust_span functions.  
   No special time zones or formats are put on the returned object.
   If one of the two vectors has a length that is a multiple of the other,
   the shorter one is repeated.

   EXCEPTIONS 

   NOTE See also: time_num_op, time_rel_add

**********************************************************************/
SEXP time_time_add( SEXP time1, SEXP time2, 
		    SEXP sign, SEXP ret_class )
{

  SEXP ret;
  double *in_sign;
  Sint *in_days1, *in_ms1, *in_days2, *in_ms2, *out_days, *out_ms;
  Sint i, lng1, lng2, lng, ind1, ind2, sign_na, is_span, tmp;
  const char *in_class;

  /* get the desired parts of the time objects */

  if( !time_get_pieces( time1, NULL, &in_days1, &in_ms1, &lng1, NULL, 
			NULL, NULL ))
    error( "Invalid time1 argument in C function time_time_add" );
  if(  !time_get_pieces( time2, NULL, &in_days2, &in_ms2, &lng2, NULL, 
			 NULL, NULL ))
    error( "Invalid time2 argument in C function time_time_add" );

  if(lng1 && lng2 && ( lng1 % lng2 ) && ( lng2 % lng1 ))
    error( "Length of longer operand is not a multiple of length of shorter in C function time_time_add" );

  /* get the sign and class */
  PROTECT(sign = AS_NUMERIC(sign));
  in_sign = REAL(sign);
  if( length(sign) < 1L ){
    UNPROTECT(5);
    error( "Problem extracting sign argument in C function time_time_add" );    
  }

  sign_na = (Sint) ISNA( *in_sign );

  if( !isString(ret_class) || length(ret_class) < 1L){
    UNPROTECT(5);
    error( "Problem extracting class argument in C function time_time_add" );    
  }
  in_class = (char *) CHAR(STRING_ELT(ret_class, 0));

  /* create output time or time span object */
  if( !lng1 || !lng2 )
    lng = 0;
  else if( lng2 > lng1 )
    lng = lng2;
  else
    lng = lng1;
  
  is_span = 1;
  if( !strcmp( in_class, TIME_CLASS_NAME ))
  {
    is_span = 0;
    PROTECT(ret = time_create_new( lng, &out_days, &out_ms ));
  }
  else if( !strcmp( in_class, TSPAN_CLASS_NAME ))
    PROTECT(ret = tspan_create_new( lng, &out_days, &out_ms ));
  else{
    UNPROTECT(5);
    error( "Unknown class argument in C function time_time_add" );
  }

  if( !ret || !out_days || !out_ms )
    error( "Could not create return object in C function time_time_add" );

  /* go through input and add */
  for( i = 0; i < lng; i++ )
  {
    ind1 = i % lng1;
    ind2 = i % lng2;

    /* check for NA */
    if( sign_na ||
	 in_days1[ind1] ==NA_INTEGER || 
	 in_ms1[ind1] ==NA_INTEGER ||
	 in_days2[ind2] ==NA_INTEGER || 
	 in_ms2[ind2] ==NA_INTEGER)
    {
      out_days[i] = NA_INTEGER;
      out_ms[i] = NA_INTEGER;
      continue;
    }

    /* add and adjust output */
    out_days[i] = in_days1[ind1] + *in_sign * in_days2[ind2];
    out_ms[i] = in_ms1[ind1] + *in_sign * in_ms2[ind2];
    if( is_span )
      tmp = adjust_span( &(out_days[i]), &(out_ms[i] ));
    else
      tmp = adjust_time( &(out_days[i]), &(out_ms[i] ));

    if( !tmp )
    {
      out_days[i] = NA_INTEGER;
      out_ms[i] = NA_INTEGER;
      continue;
    }

  }

  UNPROTECT(6); //2+4 from time_get_pieces
  return ret;
}

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME time_num_op

   DESCRIPTION  Perform an arithmetic operation between a time or
   time span and a numeric.  Supported operations are "+", "-", 
   "*", and "/".    To be called from R as 
   \\
   {\tt 
   .Call("time_num_op", time_vec, num_vec, op)
   }
   where TIMECLASS is replaced by the name of the time or time
   span class.

   ARGUMENTS
      IARG  time_vec    The R time or time span vector object
      IARG  num_vec     The numeric vector object
      IARG  op          Character string giving the operation

   RETURN Returns a time or time span vector (same as passed in class)
   that is the result of time_vec op num_vec.

   ALGORITHM  Addition and subtraction are performed by combining the integer 
   part of the numeric with the julian days of the time and the fractional 
   part of the numeric (converted from fraction of a day to milliseconds)
   to the milliseconds of the time object.  Multiplication and division
   are performed by converting the time object to a numeric with its
   integer part the number of days and fractional part the fraction of
   the day (found by the ms_to_fraction function), multiplying or dividing, 
   and then converting back. 
   No special time zones or formats are put on the returned object.
   If one of the two vectors has a length that is a multiple of the other,
   the shorter one is repeated.

   EXCEPTIONS 

   NOTE See also: time_time_add, time_rel_add

**********************************************************************/
SEXP time_num_op( SEXP time_vec, SEXP num_vec, SEXP op )
{

  SEXP ret;
  double *in_nums, tmpdbl;
  Sint *in_days, *in_ms, *out_days, *out_ms, add_sign;
  Sint i, lng1, lng2, lng, ind1, ind2, is_span, is_ok, tmp;
  const char *in_op;

  /* get the desired parts of the time object */

  if( !time_get_pieces( time_vec, NULL, &in_days, &in_ms, &lng1, NULL, 
			NULL, NULL ))
    error( "Invalid time argument in C function time_num_op" );

  /* extract other input data */
  PROTECT( num_vec = (SEXP) AS_NUMERIC(num_vec) );
  if( (lng2 = length(num_vec)) < 1L){
    UNPROTECT(3);
    error( "Problem extracting numeric argument in C function time_num_op" );
  }
  in_nums = REAL(num_vec);

  if(lng1 && lng2 && ( lng1 % lng2 ) && ( lng2 % lng1 )){
    UNPROTECT(3);
    error( "Length of longer operand is not a multiple of length of shorter in C function time_num_op" );
  }

  if( !isString(op) || length(op) < 1L){
    UNPROTECT(3);
    error( "Problem extracting operation argument in C function time_num_op" ); 
  }
  if( length(op) > 1L )
    warning( "Using only the first string in operation argument in C function time_num_op" );
  in_op = CHAR(STRING_ELT(op, 0));

  if(( *in_op != '*' ) && ( *in_op != '+' ) && ( *in_op != '-' ) && 
     ( *in_op != '/' )){
    UNPROTECT(3);
    error( "Unknown operator in C function time_num_op" );    
  }

  /* create output time or time span object */
  if( !lng1 || !lng2 )
    lng = 0;
  else if( lng2 > lng1 )
    lng = lng2;
  else
    lng = lng1;
  
  is_span = 1;
  if( checkClass( time_vec, IS_TIME_CLASS, 1L ))
  {
    is_span = 0;
    PROTECT(ret = time_create_new( lng, &out_days, &out_ms ));
  }
  else if( checkClass( time_vec, IS_TSPAN_CLASS, 1L )){
    PROTECT(ret = tspan_create_new( lng, &out_days, &out_ms ));
  } else {
    UNPROTECT(3);
    error( "Unknown class on first argument in C function time_num_op" );
  }

  if( !out_days || !out_ms || !ret ){
    UNPROTECT(4);
    error( "Could not create return object in C function time_num_op" );
  }

  /* go through input and perform operation */
  for( i = 0; i < lng; i++ )
  {
    ind1 = i % lng1;
    ind2 = i % lng2;

    /* check for NA */
    if(	 in_days[ind1] == NA_INTEGER || 
	 in_ms[ind1] == NA_INTEGER ||
	ISNA( in_nums[ind2]))
    {
      out_days[i] = NA_INTEGER;
      out_ms[i] = NA_INTEGER;
      continue;
    }

    /* operate and adjust output */
    add_sign = 1;
    is_ok = 1;
    switch( *in_op )
    {
    case '-':
      add_sign = -1;
    /*LINTED: Meant to fall through here */
    case '+':
      /* add/subtract integer part to days and fractional part to ms */
      out_days[i] = in_days[ind1] + add_sign * (Sint) floor( in_nums[ind2] );
      is_ok = ms_from_fraction( in_nums[ ind2 ] - floor( in_nums[ind2] ), 
			&(out_ms[i]));
      out_ms[i] = in_ms[ind1] + add_sign * out_ms[i];
      break;

    case '*':
      /* convert time to numeric, multiply, convert back */
      if( in_ms[ind1] > 0 )
	is_ok = ms_to_fraction( in_ms[ind1], &tmpdbl );
      else
      {
	is_ok = ms_to_fraction( - in_ms[ind1], &tmpdbl );
	tmpdbl = -tmpdbl;
      }
      tmpdbl = ( tmpdbl + in_days[ind1] ) * in_nums[ind2];
      out_days[i] = (Sint) floor( tmpdbl );
      is_ok = is_ok && ms_from_fraction( tmpdbl - out_days[i], &out_ms[i] );
      break;

    case '/':
      /* convert time to numeric, divide, convert back */
      if( in_ms[ind1] > 0 )
	is_ok = ms_to_fraction( in_ms[ind1], &tmpdbl );
      else
      {
	is_ok = ms_to_fraction( - in_ms[ind1], &tmpdbl );
	tmpdbl = -tmpdbl;
      }
      if( in_nums[ind2] == 0 )
	is_ok = 0;
      else
	tmpdbl = ( tmpdbl + in_days[ind1] ) / in_nums[ind2];
      out_days[i] = (Sint) floor( tmpdbl );
      is_ok = is_ok && ms_from_fraction( tmpdbl - out_days[i], &out_ms[i] );
      break;

    default:
      is_ok = 0;
    }

    if( !is_ok )
    {
      out_days[i] = NA_INTEGER;
      out_ms[i] = NA_INTEGER;
      continue;
    }

    if( is_span )
      tmp = adjust_span( &(out_days[i]), &(out_ms[i] ));
    else
      tmp = adjust_time( &(out_days[i]), &(out_ms[i] ));

    if( !tmp )
    {
      out_days[i] = NA_INTEGER;
      out_ms[i] = NA_INTEGER;
      continue;
    }

  }

  UNPROTECT(4); //2+2 from time_get_pieces
  return ret;
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME time_range

   DESCRIPTION  Find the range of a time or time span.
   To be called from R as 
   \\
   {\tt 
   .Call("time_range", time_vec, na_rm)
   }
   where TIMECLASS is replaced by the name of the time or time
   span class.

   ARGUMENTS
      IARG  time_vec    The R time or time span vector object
      IARG  na_rm       T to remove NAs

   RETURN Returns a length 2 time or time span vector (same as passed 
   in class) containing the minimum and maximum times or time spans.

   ALGORITHM  If na_rm is False and there are NAs, this function will
   return NA for the min and max.  Otherwise, it will find the minimum
   and maximum time or time span in the passed in vector, and return
   them in a newly created time object.
   No special time zones or formats are put on the returned object.


   EXCEPTIONS 

   NOTE 

**********************************************************************/
SEXP time_range( SEXP time_vec, SEXP na_rm )
{

  SEXP ret;
  Sint *in_days, *in_ms, *out_days, *out_ms, *rm_na;
  Sint i, lng, initialized, tmplng;

  /* get the desired parts of the time object */

  if( !time_get_pieces( time_vec, NULL, &in_days, &in_ms, &lng, NULL, 
			NULL, NULL ))
    error( "Invalid time argument in C function time_range" );

  /* get na_rm */
  PROTECT(na_rm = AS_LOGICAL(na_rm));
  if( length(na_rm) < 1L){
    UNPROTECT(3);
    error( "Problem extracting data from second argument in C function time_range" );
  }
  rm_na = (Sint *) LOGICAL(na_rm);

  /* create output time or time span object */
  
  if( checkClass( time_vec, IS_TIME_CLASS, 1L ))
    PROTECT(ret = time_create_new( 2, &out_days, &out_ms ));
  else if( checkClass( time_vec, IS_TSPAN_CLASS, 1L ))
    PROTECT(ret = tspan_create_new( 2, &out_days, &out_ms ));
    else {
      UNPROTECT(3);
      error( "Unknown class on first argument in C function time_range" );
    }

  if( !out_days || !out_ms || !ret ){
    UNPROTECT(4);
    error( "Could not create return object in C function time_range" );
  }

  /* go through input and find_range */
  initialized = 0;

  for( i = 0; i < lng; i++ )
  {
    /* check for NA */
    if(	 in_days[i] ==NA_INTEGER || 
	 in_ms[i] ==NA_INTEGER)
    {
      if( *rm_na ) /* ignore NA */
	continue;
      else /* NA causes output to be NA */
      {
	out_days[0] = NA_INTEGER;
	out_ms[0] = NA_INTEGER;
	out_days[1] = NA_INTEGER;
	out_ms[1] = NA_INTEGER;

	return( ret );
      }
    }

    /* if we haven't put anything into return vector yet, put this value in */
    if( !initialized )
    {
      out_days[0] = out_days[1] = in_days[i];
      out_ms[0] = out_ms[1] = in_ms[i];
      initialized = 1;
      continue;
    }

    /* check to see if we're bigger or smaller than current max/min */
    if(( in_days[i] > out_days[1] ) || 
       (( in_days[i] == out_days[1] ) && ( in_ms[i] > out_ms[1] )))
    {
      out_days[1] = in_days[i];
      out_ms[1] = in_ms[i];
    }

    if(( in_days[i] < out_days[0] ) || 
       (( in_days[i] == out_days[0] ) && ( in_ms[i] < out_ms[0] )))
    {
      out_days[0] = in_days[i];
      out_ms[0] = in_ms[i];
    }
  }

  if( !initialized ) /* didn't find anything in the vector */
  {
    out_days[0] = NA_INTEGER;
    out_ms[0] = NA_INTEGER;
    out_days[1] = NA_INTEGER;
    out_ms[1] = NA_INTEGER;
  }

  UNPROTECT(4); //2+2 from time_get_pieces
  return ret;
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME time_sum

   DESCRIPTION  Sum a time span or time object.
   To be called from R as 
   \\
   {\tt 
   .Call("time_sum", time_vec, na_rm, cum)
   }
   where TIMECLASS is replaced by the name of the time or time
   span class.  Normally only time spans are summed, but a time
   object can be summed to calculate mean.

   ARGUMENTS
      IARG  time_vec    The R time or time span vector object
      IARG  na_rm       T to remove NAs
      IARG  cum         T to do a cumulative sum vector, F for sum

   RETURN If cum is T, na_rm is ignored and this function returns a 
   vector of the same type as the input whose elements are the 
   cumulative sum of the inputs through the corresponding input element.  
   If the jth element is NA, then all subsequent elements of the return
   will be NA and a warning will be generated. If cum is F, this
   function returns a length 1 vector of the same type as the input, 
   containing the sum of all the elements.  If na_rm is False, any
   NA in the input will generate an NA return value.

   ALGORITHM  The sums are calculated through addition, along with
   the adjust_time or adjust_span function. 
   No special time zones or formats are put on the returned object.

   EXCEPTIONR 

   NOTE 

**********************************************************************/
SEXP time_sum( SEXP time_vec, SEXP na_rm, SEXP cum )
{

  SEXP ret;
  Sint *in_days, *in_ms, *out_days, *out_ms, *rm_na, *in_cum;
  Sint i, lng, is_span, tmplng, tmp;

  /* get the desired parts of the time object */

  if( !time_get_pieces( time_vec, NULL, &in_days, &in_ms, &lng, NULL, 
			NULL, NULL ))
    error( "Invalid time argument in C function time_sum" );

  /* get na_rm and cum */
  PROTECT(na_rm = AS_LOGICAL(na_rm));
  if( length(na_rm) < 1L){
    UNPROTECT(3);
    error( "Problem extracting data from second argument in C function time_sum" );
  }
  rm_na = (Sint *) LOGICAL(na_rm);

  PROTECT(cum = AS_LOGICAL(cum));
  if( length(cum) < 1L){
    UNPROTECT(4);
    error( "Problem extracting data from third argument in C function time_sum" );
  }
  in_cum = (Sint *) LOGICAL(cum);

  /* create output time or time span object */

  is_span = 0;
  if( checkClass( time_vec, IS_TIME_CLASS, 1L ))
    PROTECT(ret = time_create_new( *in_cum ? lng : 1, &out_days, &out_ms ));
  else if( checkClass( time_vec, IS_TSPAN_CLASS, 1L ))
  {
    is_span = 1;
    PROTECT(ret = tspan_create_new( *in_cum ? lng : 1, &out_days, &out_ms ));
  }
  else{
    UNPROTECT(4);
    error( "Unknown class on first argument in C function time_sum" );
  }

  if( !out_days || !out_ms || !ret ){
    UNPROTECT(5);
    error( "Could not create return object in C function time_sum" );
  }	

  out_days[0] = out_ms[0] = 0;

  /* go through input and find sum */

  for( i = 0; i < lng; i++ )
  {
    /* check for NA */
    if(	 in_days[i] ==NA_INTEGER || 
	 in_ms[i] ==NA_INTEGER)
    {
      if( !*in_cum && *rm_na ) /* ignore NA */
	continue;
      else /* NA causes output to be NA */
      {
	if( *in_cum ) {
	  for( ; i < lng; i++ ) /* fill in all the rest */
	  {
	    out_days[i] = NA_INTEGER;
	    out_ms[i] = NA_INTEGER;
	  }
	  warning( "Found NA value in cumsum" );
	}
	else /* make the sum NA */
	{
	  out_days[0] = NA_INTEGER;
	  out_ms[0] = NA_INTEGER;
	}

	UNPROTECT(5);
	return( ret );
      }
    }

    /* add this value in */

    if( *in_cum && ( i >= 1 ))
    {
      out_days[i] = out_days[i-1] + in_days[i];
      out_ms[i] = out_ms[i-1] + in_ms[i];
      if( is_span )
	tmp = adjust_span( &(out_days[i]), &(out_ms[i] ));
      else
	tmp = adjust_time( &(out_days[i]), &(out_ms[i] ));
    } else {
      out_days[0] += in_days[i];
      out_ms[0] += in_ms[i];
      if( is_span )
	tmp = adjust_span( &(out_days[0]), &(out_ms[0] ));
      else
	tmp = adjust_time( &(out_days[0]), &(out_ms[0] ));
    }

    if( !tmp )
    {
	  out_days[0] = NA_INTEGER;
	  out_ms[0] = NA_INTEGER;
    }
  }

  UNPROTECT(5); //3+2 from time_get_pieces
  return ret;
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME time_rel_add

   DESCRIPTION  Add a relative time object to a time object. 
   To be called from R as 
   \\
   {\tt 
   .Call("time_rel_add", time.vec, rel.obj@.Data, sort( rel.obj@holidays),
          zone.list)
   }
   where TIMECLASS is replaced by the name of the time class.

   ARGUMENTS
      IARG  time_vec    The R time vector object
      IARG  rel_strs    The character strings from the relative time object
      IARG  hol_vec     SORTED vector of holiday dates
      IARG  zone_list   The list of R time zone objects


   RETURN Returns a time vector that is the result of adding the
   relative times to the input time vector, taking into account the
   given holidays.  The relative time addition is done in the 
   object's local time zone.

   ALGORITHM  First, a list of the holidays' julian dates is 
   created, taking into account the holidays' time zone.
   (Time values are converted to local zones by first converting
   to a TIME_DATE_STRUCT using the jms_to_struct function, and then
   to their local time zones by calling GMT_to_zone function
   in conjunction with find_zone.  If needed, they can then be
   converted back to julian dates by calling julian_from_mdy.)
   Then the times are combined with the
   relative times using the rtime_add_with_zones function.
   No special time zones or formats are put on the returned object.
   If time_vec or rel_strs has a length that is a multiple of the other,
   the shorter one is repeated. If the sequence becomes monotonic
   or is not moving from start towards end, an error is generated.

   EXCEPTIONS 

   NOTE See also: time_rel_seq, time_time_add, time_num_op

**********************************************************************/
SEXP time_rel_add( SEXP time_vec, SEXP rel_strs, 
		   SEXP hol_vec, SEXP zone_list )
{

  SEXP ret;
  Sint *in_days, *in_ms, *out_days, *out_ms;
  Sint *hol_days, *hol_ms;
  Sint i, lng1, lng2, lng_hol, lng, ind1, ind2, all_na;
  TIME_DATE_STRUCT td, td_hol;
  TZONE_STRUCT *tzone, *tzone_hol;
  Sint *hol_dates;

  /* get the desired parts of the time objects */

  if( !time_get_pieces( time_vec, NULL, &in_days, &in_ms, &lng1, NULL, 
			&td.zone, NULL ) ||
      !td.zone || ( lng1 && ( !in_days || !in_ms )))
    error( "Invalid time argument in C function time_rel_add" );

  tzone = find_zone( td.zone, zone_list );
  if( !tzone )
    error( "Unknown or unreadable time zone in C function time_rel_add" );

  if( !time_get_pieces( hol_vec, NULL, &hol_days, &hol_ms, &lng_hol, NULL, 
			&td_hol.zone, NULL ) ||
      (( lng_hol && (!hol_days || !hol_ms )) || !td_hol.zone ))
    error( "Invalid holiday argument in C function time_rel_add" );

  tzone_hol = find_zone( td_hol.zone, zone_list );
  if( !tzone_hol )
    error( "Unknown or unreadable time zone for holidays in C function time_rel_add" );

  /* extract the rel time strings */
  if(!isString(rel_strs) || (lng2 = length(rel_strs)) < 1L)
    error( "Problem extracting relative time strings in C function time_rel_add" );

  if(lng1 && lng2 && ( lng1 % lng2 ) && ( lng2 % lng1 ))
    error( "Length of longer operand is not a multiple of length of shorter in C function time_rel_add" );

  /* create output time object */
  if( !lng1 || !lng2 )
    lng = 0;
  else if( lng2 > lng1 )
    lng = lng2;
  else
    lng = lng1;


  PROTECT(ret = time_create_new( lng, &out_days, &out_ms ));

  if( !out_days || !out_ms || !ret )
    error( "Could not create return object in C function time_rel_add" );

  /* get list of holiday dates */
  hol_dates = NULL;
  all_na = 0;
  if( lng_hol>0 )
  {
    hol_dates = (Sint *) R_alloc( lng_hol, sizeof(Sint) );

    for( i = 0; i < lng_hol; i++ )
    {
      if(  hol_days[i] == NA_INTEGER || 
	   hol_ms[i] == NA_INTEGER ||
	  !jms_to_struct( hol_days[i], hol_ms[i], &td_hol ) ||
	  !GMT_to_zone( &td_hol, tzone_hol ) ||
	  !julian_from_mdy( td_hol, &(hol_dates[i])))
	{
	  all_na = 1;
	  break;
	}
    }
  }

  /* go through input and perform operation */
  for( i = 0; i < lng; i++ )
  {
    ind1 = i % lng1;
    ind2 = i % lng2;

    /* check for NA, convert to local zone, add, and convert back */
    if(	all_na ||
	 in_days[ind1] == NA_INTEGER || 
	 in_ms[ind1] == NA_INTEGER ||
	!jms_to_struct( in_days[ind1], in_ms[ind1], &td ) ||
	!rtime_add_with_zones( &td, (char *) CHAR(STRING_ELT(rel_strs, ind2)), 
			       hol_dates, lng_hol, tzone ) ||
	!julian_from_mdy( td, &(out_days[i] )) ||
	!ms_from_hms( td, &(out_ms[i] )))
    {
      out_days[i] = NA_INTEGER;
      out_ms[i] = NA_INTEGER;
    }
  }

  UNPROTECT(5); //4+1 from time_get_pieces
  return ret;

}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME time_rel_seq

   DESCRIPTION  Make a sequence by repeatedly adding a relative time 
   object to a time object. To be called from R as 
   \\
   {\tt 
   .Call("time_rel_seq", start.time, end.time, length, use.length,
         rel.obj@.Data, sort( rel.obj@holidays),
          zone.list)
   }
   where TIMECLASS is replaced by the name of the time class.

   ARGUMENTS
      IARG  start_time  The starting R time object
      IARG  end_time    The ending R time object
      IARG  len_vec     The length of the sequence
      IARG  has_len     True if length is used; False if end_time is used
      IARG  rel_strs    The character string from the relative time object
      IARG  hol_vec     SORTED vector of holiday dates
      IARG  zone_list   The list of R time zone objects


   RETURN Returns a time vector that is the result of adding the
   relative time repeatedly to the input start time vector, 
   taking into account the given holidays, until either end_time
   is reached (if has_len is False) or the desired length is
   reached (if has_len is True).

   ALGORITHM  First, a list of the holidays' julian dates is 
   created, taking into account the holidays' time zone.
   (Time values are converted to local zones by first converting
   to a TIME_DATE_STRUCT using the jms_to_struct function, and then
   to their local time zones by calling GMT_to_zone function
   in conjunction with find_zone.  If needed, they can then be
   converted back to julian dates by calling julian_from_mdy.)
   Then the starting time is repeatedly combined 
   with the relative time using the rtime_add_with_zones function.
   No special time zones or formats are put on the returned object.
   If start, end, length, or relative time has a length > 1, the 
   extra values are ignored and a warning is generated.

   EXCEPTIONS 

   NOTE See also: time_rel_add

**********************************************************************/
static Sint avoid_bad_start_day = 0 ;
void s_set_avoid_bad_start_day(Sint *flag)
{
  Sint tmp = *flag ;
  *flag = avoid_bad_start_day ;
  if (tmp>=0)
    avoid_bad_start_day = tmp ;
}

SEXP time_rel_seq( SEXP start_time, SEXP end_time,
		   SEXP len_vec, SEXP has_len,
		   SEXP rel_strs, SEXP hol_vec,
		   SEXP zone_list)
{
  SEXP ret, tmp_days, tmp_ms;
  Sint *start_days, *start_ms, *end_days, *end_ms,
    *out_days, *out_ms, *use_len, *seq_len;
  Sint *hol_days, *hol_ms, num_alloc;
  Sint i, lng_hol, lng, direction=0;
  TIME_DATE_STRUCT td, td_hol;
  TZONE_STRUCT *tzone, *tzone_hol;
  char *in_strs;
  Sint *hol_dates;
  Sint pre_start_day, pre_start_ms, used_old_alg ;
  Sint num_protect=0;

  /* figure out if we have end time or length */
  PROTECT(has_len = AS_LOGICAL(has_len));
  num_protect++;
  if( length(has_len) < 1L){
    UNPROTECT(num_protect);
    error( "Problem extracting data from second argument in C function time_rel_seq" );
  }
  use_len = (Sint *) LOGICAL(has_len);

  /* get the desired parts of the time objects */
  if( !time_get_pieces( start_time, NULL, &start_days, &start_ms, &lng, NULL, 
			&td.zone, NULL ) ||
      !td.zone || !lng || !start_days || !start_ms ){
    UNPROTECT(num_protect);
    error( "Invalid time argument in C function time_rel_seq" );
  }
  num_protect += 2; //from time_get_pieces
  if( lng > 1 )
    warning( "Start time has multiple elements; only the first will be used" );

  tzone = find_zone( td.zone, zone_list );
  if( !tzone ){
    UNPROTECT(num_protect);
    error( "Unknown or unreadable time zone in C function time_rel_seq" );
  }

  if( !(*use_len))
  {
    if( !time_get_pieces( end_time, NULL, &end_days, &end_ms, 
			  &lng, NULL, NULL, NULL ) ||
	!lng || !end_days || !end_ms ){
      error( "Invalid time argument in C function time_rel_seq" );
      UNPROTECT(num_protect);
    }
    num_protect +=2;
    if( lng > 1 )
      warning( "End time has multiple elements; only the first will be used" );
  }

  if( !time_get_pieces( hol_vec, NULL, &hol_days, &hol_ms, &lng_hol, NULL, 
			&td_hol.zone, NULL ) ||
      (( lng_hol && (!hol_days || !hol_ms )) || !td_hol.zone )){
    UNPROTECT(num_protect);
    error( "Invalid holiday argument in C function time_rel_seq" );
  }
  num_protect +=2;

  tzone_hol = find_zone( td_hol.zone, zone_list );
  if( !tzone_hol ){
    UNPROTECT(num_protect);
    error( "Unknown or unreadable time zone for holidays in C function time_rel_seq" );
  }

  /* extract the rel time string */
  if(!isString(rel_strs) || (lng = length(rel_strs)) < 1L){
    UNPROTECT(num_protect);
    error( "Problem extracting relative time strings in C function time_rel_seq" );    
  }
  if( lng > 1 )
    warning( "Relative time has multiple elements; only the first will be used" );
  in_strs = (char *) CHAR(STRING_ELT(rel_strs, 0));
  /* extract the length */

  if( *use_len )
  {
    if( !IS_INTEGER(len_vec) || (lng = length(len_vec)) < 1L){
      UNPROTECT(num_protect);
      error( "Problem extracting data from third argument in C function time_rel_seq" );
    }
    seq_len = INTEGER(len_vec);

    if( *seq_len < 0 )
      error( "Length cannot be less than zero" );

    if( lng > 1 )
      warning( "Length has multiple elements; only the first will be used" );
  }

  /* get list of holiday dates */
  hol_dates = NULL;
  if( lng_hol )
  {
    hol_dates = (Sint *) R_alloc( lng_hol, sizeof(Sint) );

    for( i = 0; i < lng_hol; i++ )
    {
      if(  hol_days[i] ==NA_INTEGER || 
	   hol_ms[i] ==NA_INTEGER ||
	  !jms_to_struct( hol_days[i], hol_ms[i], &td_hol ) ||
	  !GMT_to_zone( &td_hol, tzone_hol ) ||
	  !julian_from_mdy( td_hol, &(hol_dates[i])))
	  error( "Bad holiday data in C function time_rel_seq" );
    }
  }

  /* create output time object or temporary storage */

  if( *use_len )
  {
    if(  *seq_len ==NA_INTEGER){
      UNPROTECT(num_protect);
      error( "NA not allowed in sequence" );
    }

    PROTECT(ret = time_create_new( *seq_len, &out_days, &out_ms ));
    num_protect++;

    if( !out_days || !out_ms || !ret ){
      UNPROTECT(num_protect);
      error( "Could not create return object in C function time_rel_seq" );
    }
    if(*seq_len == 0){
      UNPROTECT(num_protect);
      return ret;
    }
  } else
  {
    /* figure out the direction */

    if(  *end_days ==NA_INTEGER ||  *end_ms ==NA_INTEGER){
      UNPROTECT(num_protect);
      error( "NA not allowed in sequence" );
    }

    if(( *start_days > *end_days ) || 
       (( *start_days == *end_days ) && ( *start_ms > *end_ms )))
      direction = -1;
    else if(( *end_days > *start_days ) ||
       (( *start_days == *end_days ) && ( *end_ms > *start_ms )))
      direction = 1;
    else
      direction = 0; /* this will be a flag to end after copying in start */

    /* we don't know the length we'll need.  Allocate at least 100,
       and assume daily to figure out approx length if longer */

    num_alloc = 100;
    if( *start_days - *end_days > 100 )
      num_alloc =  *start_days - *end_days + 20;
    if( *end_days - *start_days > 100 )
      num_alloc =  *end_days - *start_days + 20;

    PROTECT(tmp_days = NEW_INTEGER(num_alloc));
    PROTECT(tmp_ms = NEW_INTEGER(num_alloc));
    num_protect += 2;

    out_days = INTEGER(tmp_days);
    out_ms = INTEGER(tmp_ms);
  }

  /* start with the start time */

  if(  *start_days ==NA_INTEGER ||  *start_ms ==NA_INTEGER){
    UNPROTECT(num_protect);
    error( "NA not allowed in sequence" );
  }
  /* fprintf(stderr, " time_rel_seq: start=%ld,%ld, in_strs[0]=%s\n", *start_days, *start_ms, in_strs[0]); */
  if (avoid_bad_start_day) {
     /* the following is gross.  -wwd */
     char tmp_strs[100] ;
     strncpy(tmp_strs, in_strs, 99);
     if (tmp_strs[0] == '-')
       tmp_strs[0] = '+';
     else if (tmp_strs[0] == '+')
       tmp_strs[0] = '-';
     if (tmp_strs[1] == 'a') {
       /* fprintf(stderr, " time_rel_seq: alignment might have caused problems -- using old algorithm\n");  */
       out_days[0] = *start_days;
       out_ms[0] = *start_ms;
       used_old_alg = 1 ;
       i = 1 ;
     } else {
       /* convert to local zone, add, and convert back */
       if( !jms_to_struct( *start_days, *start_ms, &td ) ||
	   !rtime_add_with_zones( &td, tmp_strs, hol_dates, lng_hol, tzone ) ||
	   !julian_from_mdy( td, &pre_start_day) ||
	   !ms_from_hms( td, &pre_start_ms)){
	 UNPROTECT(num_protect);
         error( "Could not subtract relative time in C function time_rel_seq" );
       }
        /* fprintf(stderr, "pre_start=%ld,%ld\n", pre_start_day, pre_start_ms); */
        i = 0; /* was i=1 */
        used_old_alg = 0 ;
     }
  } else {
      /* original algorithm */
      out_days[0] = *start_days;
      out_ms[0] = *start_ms;
      i = 1 ;
      used_old_alg = 1 ;
  }

  /* go through input and perform operation */
#define PREV_DAY ( (i>0) ? out_days[i-1] : pre_start_day )
#define PREV_MS  ( (i>0) ? out_ms[i-1] : pre_start_ms )

  /*LINTED: Const meant to be in cond context here */
  while( 1 )
  {
    /* see if we are done */
    if( *use_len && ( i >= *seq_len )){
      UNPROTECT(num_protect);
      return ret;
    }
    
    if( !*use_len )
    {
      if( !direction )
	break;
      if(( direction * ( PREV_DAY - *end_days ) > 0 ) ||
	 (( PREV_DAY == *end_days ) && 
	  ( direction * ( PREV_MS - *end_ms ) > 0 )))
      {
	if (i>0) /* is i==0 possible? I think it means an error */
          i--;
	break;
      }

      /* also check on our allocation */
      if( i >= num_alloc - 1 )
      {
	num_alloc += 200;
	/* SETLENGTH( tmp_days, num_alloc ); */
  tmp_days = Rf_xlengthgets(tmp_days, num_alloc );
        out_days = INTEGER(tmp_days) ;
	/* SETLENGTH( tmp_ms, num_alloc ); */
	tmp_ms = Rf_xlengthgets( tmp_ms, num_alloc );
        out_ms = INTEGER(tmp_ms) ;
      }
    }

    /* convert to local zone, add, and convert back */
    if(	!jms_to_struct( PREV_DAY, PREV_MS, &td ) ||
	!rtime_add_with_zones( &td, in_strs, hol_dates, lng_hol, tzone ) ||
	!julian_from_mdy( td, &(out_days[i] )) ||
	!ms_from_hms( td, &(out_ms[i] ))){
      UNPROTECT(num_protect);
      error( "Could not add relative time in C function time_rel_seq" );
    }

    /* make sure we went in the right direction */

    if(( out_days[i] == PREV_DAY ) &&
       ( out_ms[i] == PREV_MS )){
      UNPROTECT(num_protect);
      error( "Relative date addition resulted in stationary time" );
    }

    if( !direction )
    {
      if(( out_days[i] > PREV_DAY ) ||
	 (( out_days[i] == PREV_DAY ) &&
	  ( out_ms[i] > PREV_MS )))
	direction = 1;
      else
	direction = -1;
    } else
    {
      if(( direction * ( out_days[i] - PREV_DAY ) < 0 ) ||
	 (( out_days[i] == PREV_DAY ) && 
	  ( direction * ( out_ms[i] - PREV_MS) < 0 ))){
	UNPROTECT(num_protect);
	error( "Relative date addition resulted in non-monotonic sequence" );
      }
    }

    i++;
  }


  /* if we got here, it means we have to make a time object and
     copy in the numbers now */

  num_alloc = i;
  PROTECT(ret = time_create_new( num_alloc, &end_days, &end_ms ));
  num_protect++;
  if( !end_days || !end_ms || !ret ){
    UNPROTECT(num_protect);
    error( "Could not create return object in C function time_rel_seq" );
  }

  for( i = 0; i < num_alloc; i++ )
  {
    end_days[i] = out_days[i];
    end_ms[i] = out_ms[i];
  }

  UNPROTECT(num_protect);
  return ret;
}
