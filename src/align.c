/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
 *************************************************************************/

/*************************************************************************
 *
 * It contains C code utility functions for alignment of R objects 
 * that are positions of time series. 
 *
 * The exported functions here were written to be called with the 
 * .Call interface of S.  They include (see documentation below):
  SEXP *num_align( s_object *num_obj, s_object *align_pos, 
                       s_object *how_obj, s_object *match_tol );
*************************************************************************/

#include "align.h"

/**********************************************************************
 * C Code DOCUMENTATION ************************************************
 **********************************************************************
   NAME num_align

   DESCRIPTION  Align a numeric positions object to new positions.
   To be called from R as 
   \\
   {\tt 
    .Call("num_align", orig.obj, new.pos, c( how, error.how ),
          matchtol)
   }

   ARGUMENTS
      IARG  num_obj   Original positions object
      IARG  align_pos New positions to align to
      IARG  how_obj   How to perform alignment (see below)
      IARG  match_tol Tolerance for matching

   RETURN Returns an R list object, where each element is a vector of
   the same length as the alignment positions telling what data to take
   from the original ordered data object to make an aligned ordered data
   object.  The first element of 
   the list is a logical vector telling whether or not the output
   should be set to NA for the corresponding alignment position.  The
   second is a logical vector telling whether or not the corresponding
   alignment position should be dropped from the output.  Unless
   the first element of how_obj is ``interp'', the third element of the
   return list is an integer vector of subscripts for the original
   data to form the new data.  If how_obj is ``interp'', the third through
   sixth elements of the return list give the two subscripts to 
   interpolate between and their two weights: the third element is
   the first weight; the fourth is the first subscript; the fifth is
   the second weight; the sixth is the second subscript. 

   ALGORITHM The two positions objects are assumed to be monotonic
   in either direction, with repeats allowed.  This function goes
   through the positions in align_pos, and for each one finds the 
   nearest positions from num_obj before and after the given position.
   If one of the two match within match_tol, the corresponding subscript 
   element of the return list is set to the appropriate subscript of
   num_obj.  The first element of how_obj tells what to do if there
   is no match: ``NA'' causes the NA element of the return list to be set
   to true; ``drop'' causes the drop element to be set to true; 
   ``nearest'' puts the subscript of the nearest position into the 
   subscript element; ``before'' uses the position before; ``after''
   uses the position after'', and ``interp'' sets up interpolation 
   weights.  In the last three cases, if the appropriate subscript
   or weights cannot be calculated (i.e. off the end of the series),
   then the second element of how_obj tells what to do: ``NA'', 
   ``drop'', or ``nearest''. 

   EXCEPTIONS 

   NOTE See also: time_align

**********************************************************************/

SEXP num_align( SEXP num_obj, SEXP align_pos, 
		SEXP how_obj, SEXP match_tol )
{

  SEXP ret;

  double *in_nums, *in_pos, *pmatch_tol, diff_under, diff_over;

  Sint in_len, align_len, tmplen;
  Sint in_inc, in_start, in_curr;
  Sint align_inc, align_start, align_end, align_curr;
  int over_set, under_set;

  int how, error_how;

  Sint *na_data, *drop_data, *sub1_data, *sub2_data;
  double *weight1_data, *weight2_data;

  /* extract input data*/

  if( !IS_NUMERIC( num_obj ) || (( in_len = length(num_obj) ) < 1) ||
      !IS_NUMERIC( align_pos ) || (( align_len = length(align_pos) ) < 1) ||
      !IS_CHARACTER( how_obj ) || (( tmplen = length(how_obj) ) < 2) ||
      !IS_NUMERIC( match_tol ) || (( tmplen = length(match_tol) ) < 1))
    error( "invalid data in c function num_align" ); 

  in_nums = REAL( num_obj );
  in_pos = REAL( align_pos );
  pmatch_tol = REAL( match_tol );

  if( !strcmp( CHAR(STRING_ELT(how_obj, 0)), "NA" ))
    how = 0;
  else if( !strcmp( CHAR(STRING_ELT(how_obj, 0)), "drop" ))
    how = 1;
  else if( !strcmp( CHAR(STRING_ELT(how_obj, 0)), "nearest" ))
    how = 2;
  else if( !strcmp( CHAR(STRING_ELT(how_obj, 0)), "before" ))
    how = 3;
  else if( !strcmp( CHAR(STRING_ELT(how_obj, 0)), "after" ))
    how = 4;
  else if( !strcmp( CHAR(STRING_ELT(how_obj, 0)), "interp" ))
    how = 5;
  else
    error( "invalid third argument in C function num_align" );

  if( !strcmp( CHAR(STRING_ELT(how_obj, 1)), "NA" ))
    error_how = 0;
  else if( !strcmp( CHAR(STRING_ELT(how_obj, 1)), "drop" ))
    error_how = 1;
  else if( !strcmp( CHAR(STRING_ELT(how_obj, 1)), "nearest" ))
    error_how = 2;
  else
    error( "Invalid third argument in C function num_align" );

  if(pmatch_tol[0] < 0)
    error( "invalid fourth argument in C function num_align" );

  /* see if the inputs are increasing or decreasing series */
  in_start = align_start = 0;
  in_inc = align_inc = 1;
  align_end = align_len - 1;

  for( in_curr = 1; in_curr < in_len; in_curr++ )
  {
    if( in_nums[in_curr] > in_nums[ in_curr - 1 ] )
      break;
    if( in_nums[in_curr] < in_nums[ in_curr - 1 ] )
    {
      in_inc = -1;
      in_start = in_len - 1;
      break;
    }
  }

  for( in_curr = 1; in_curr < align_len; in_curr++ )
  {
    if( in_pos[in_curr] > in_pos[ in_curr - 1 ] )
      break;
    if( in_pos[in_curr] < in_pos[ in_curr - 1 ] )
    {
      align_inc = -1;
      align_start = align_len - 1;
      align_end = 0;
      break;
    }
  }

  /* create return list */

  if( how == 5 )
  {
    PROTECT(ret = NEW_LIST(6));
    na_data = LOGICAL( SET_VECTOR_ELT(ret, 0 , NEW_LOGICAL(align_len)) );
    drop_data = LOGICAL( SET_VECTOR_ELT(ret, 1 , NEW_LOGICAL(align_len)) );
    weight1_data = REAL( SET_VECTOR_ELT(ret, 2 , NEW_NUMERIC(align_len)) );
    sub1_data = INTEGER( SET_VECTOR_ELT(ret, 3 , NEW_INTEGER(align_len)) );
    weight2_data = REAL( SET_VECTOR_ELT(ret, 4 , NEW_NUMERIC(align_len)) );
    sub2_data = INTEGER( SET_VECTOR_ELT(ret, 5 , NEW_INTEGER(align_len)) );
  } else
  {
    PROTECT(ret = NEW_LIST(6));
    na_data = LOGICAL( SET_VECTOR_ELT(ret, 0 , NEW_LOGICAL(align_len)) );
    drop_data = LOGICAL( SET_VECTOR_ELT(ret, 1 , NEW_LOGICAL(align_len)) );
    sub1_data = INTEGER( SET_VECTOR_ELT(ret, 2 , NEW_INTEGER(align_len)) );
  }

  /* go through the alignment positions and find the right indexes,
     NA or not values, and drop values */

  in_curr = in_start;
  for( align_curr = align_start; align_curr != ( align_end + align_inc ); 
       align_curr += align_inc )
  {
    /* move along the nums series until we pass current pos position */
    while(( in_curr  >= 0 ) && ( in_curr < in_len ) && 
	  ( in_nums[ in_curr ] < in_pos[ align_curr ] ))
      in_curr += in_inc;

    /* see how far we are from the current position */

    /* over_set and under_set get set if we're inside the respective ends of 
       the time series; diff_over and diff_under store the difference
       between the align_ time and the in_ times before and after it. */

    over_set = under_set = 0;
    if(( in_curr  >= 0 ) && ( in_curr < in_len ))
    {
      diff_over = in_nums[ in_curr ] - in_pos[ align_curr ];
      over_set = 1;
    }
    if( (( in_curr - in_inc ) >= 0 ) && (( in_curr - in_inc ) < in_len ))
    {
      diff_under = in_pos[ align_curr ] - in_nums[ in_curr - in_inc ];
      under_set = 1;
    }

    na_data[ align_curr ] = 0;
    drop_data[ align_curr ] = 0;
    sub1_data[ align_curr ] = 1;
    if( how == 5 )
    {
      weight1_data[ align_curr ] = 1.0;
      sub2_data[ align_curr ] = 1;
      weight2_data[ align_curr ] = 0.0;
    }

    /* is it a match? */
    if( under_set && 
       ( !over_set || ( diff_under < diff_over )) &&
       ( diff_under <= *pmatch_tol ))
      sub1_data[ align_curr ] = 1 + in_curr - in_inc; /* matches under */
    else if( over_set && ( diff_over <= *pmatch_tol ))
      sub1_data[ align_curr ] = 1 + in_curr;  /* matches over */
    else if(( how == 0 ) || 
	    (( error_how == 0 ) &&
	     (( !under_set && (( how == 3 ) || ( how == 5 ))) ||
	      ( !over_set && (( how == 4 ) || ( how == 5 ))))))
      na_data[ align_curr ] = 1;  /* make it an NA on no match */
    else if(( how == 1 ) || 
	    (( error_how == 1 ) &&
	     (( !under_set && (( how == 3 ) || ( how == 5 ))) ||
	      ( !over_set && (( how == 4 ) || ( how == 5 ))))))
      drop_data[ align_curr ] = 1;  /* drop on no match */
    else if(( how == 2 ) || 
	    (( error_how == 2 ) &&
	     (( !under_set && (( how == 3 ) || ( how == 5 ))) ||
	      ( !over_set && (( how == 4 ) || ( how == 5 ))))))
    {
      /* take nearest on no match */
      if( !under_set )
	sub1_data[ align_curr ] = 1 + in_curr;
      else if( !over_set )
	sub1_data[ align_curr ] = 1 + in_curr - in_inc;
      else if( diff_under <= diff_over )
	sub1_data[ align_curr ] = 1 + in_curr - in_inc;
      else
	sub1_data[ align_curr ] = 1 + in_curr;
    } else if( how == 3 ) /* before */
      sub1_data[ align_curr ] = 1 + in_curr - in_inc;
    else if( how == 4 ) /* after */
      sub1_data[ align_curr ] = 1 + in_curr;
    else /* interp */
    {
      sub1_data[ align_curr ] = 1 + in_curr;
      sub2_data[ align_curr ] = 1 + in_curr - in_inc;
      weight1_data[ align_curr ] = diff_under / ( diff_over + diff_under );
      weight2_data[ align_curr ] = diff_over / ( diff_over + diff_under );
    }
  }

  UNPROTECT(1);
  return( ret );
}


/**********************************************************************
 * C Code DOCUMENTATION ************************************************
 **********************************************************************
   NAME time_align

   DESCRIPTION  Align a calendar positions object to new positions.
   To be called from R as 
   \\
   {\tt 
    .Call("time_align", orig.obj, new.pos, c( how, error.how ),
          matchtol)
   },
   where TIMECLASS is replaced by the name of the time class.

   ARGUMENTS
      IARG  time_obj  Original positions object
      IARG  align_pos New positions to align to
      IARG  how_obj   How to perform alignment (see below)
      IARG  match_tol Tolerance for matching

   RETURN Returns an R list object, where each element is a vector of
   the same length as the alignment positions telling what data to take
   from the original ordered data object to make an aligned ordered data
   object.  The first element of 
   the list is a logical vector telling whether or not the output
   should be set to NA for the corresponding alignment position.  The
   second is a logical vector telling whether or not the corresponding
   alignment position should be dropped from the output.  Unless
   the first element of how_obj is ``interp'', the third element of the
   return list is an integer vector of subscripts for the original
   data to form the new data.  If how_obj is ``interp'', the third through
   sixth elements of the return list give the two subscripts to 
   interpolate between and their two weights: the third element is
   the first weight; the fourth is the first subscript; the fifth is
   the second weight; the sixth is the second subscript. 

   ALGORITHM The two positions objects are assumed to be monotonic
   in either direction, with repeats allowed.  This function goes
   through the positions in align_pos, and for each one finds the 
   nearest positions from time_obj before and after the given position.
   If one of the two match within matchtol, after conversion of both
   times to numbers, the corresponding subscript element of the 
   return list is set to the appropriate subscript of
   time_obj.  The first element of how_obj tells what to do if there
   is no match: ``NA'' causes the NA element of the return list to be set
   to true; ``drop'' causes the drop element to be set to true; 
   ``nearest'' puts the subscript of the nearest position into the 
   subscript element; ``before'' uses the position before; ``after''
   uses the position after'', and ``interp'' sets up interpolation 
   weights.  In the last three cases, if the appropriate subscript
   or weights cannot be calculated (i.e. off the end of the series),
   then the second element of how_obj tells what to do: ``NA'', 
   ``drop'', or ``nearest''. 

   EXCEPTIONS 

 See also: num_align

**********************************************************************/
SEXP time_align( SEXP time_obj, SEXP align_pos, 
		 SEXP how_obj, SEXP match_tol )
{

  SEXP ret;

  double *pmatch_tol, diff_under, diff_over, over_num, 
    under_num, align_num;
  Sint *in_days, *in_ms, *align_days, *align_ms;

  Sint in_len, align_len, tmplen;
  Sint in_inc, in_start, in_curr;
  Sint align_inc, align_start, align_end, align_curr;
  int how, error_how;
  int over_set, under_set;

  Sint *na_data, *drop_data, *sub1_data, *sub2_data;
  double *weight1_data, *weight2_data;

  /* extract input data*/

  if( !time_get_pieces( time_obj, NULL, &in_days, &in_ms, &in_len, NULL, 
			NULL, NULL ) ||
      !in_days || !in_ms || !in_len )
    error( "Invalid first argument to c function time_align" );

  if( !time_get_pieces( align_pos, NULL, &align_days, &align_ms, 
			&align_len, NULL, NULL, NULL ) ||
      !align_days || !align_ms || !align_len )
    error( "Invalid second argument to c function time_align" );

  if( !IS_NUMERIC( match_tol ) || (( tmplen = length(match_tol) ) < 1) ||
      !IS_CHARACTER( how_obj ) || (( tmplen = length(how_obj) ) < 2)
       )
    error( "Invalid data in c function time_align" ); 
  
  if( !strcmp( CHAR(STRING_ELT(how_obj, 0)), "NA" ))
    how = 0;
  else if( !strcmp( CHAR(STRING_ELT(how_obj, 0)), "drop" ))
    how = 1;
  else if( !strcmp( CHAR(STRING_ELT(how_obj, 0)), "nearest" ))
    how = 2;
  else if( !strcmp( CHAR(STRING_ELT(how_obj, 0)), "before" ))
    how = 3;
  else if( !strcmp( CHAR(STRING_ELT(how_obj, 0)), "after" ))
    how = 4;
  else if( !strcmp( CHAR(STRING_ELT(how_obj, 0)), "interp" ))
    how = 5;
  else
    error( "Invalid third argument in C function time_align" );

  if( !strcmp( CHAR(STRING_ELT(how_obj, 1)), "NA" ))
    error_how = 0;
  else if( !strcmp( CHAR(STRING_ELT(how_obj, 1)), "drop" ))
    error_how = 1;
  else if( !strcmp( CHAR(STRING_ELT(how_obj, 1)), "nearest" ))
    error_how = 2;
  else
    error( "invalid third argument in C function time_align" );

  pmatch_tol = REAL( match_tol );
  if(pmatch_tol[0] < 0)
    error( "invalid fourth argument in C function num_align" );

  /* see if the inputs are increasing or decreasing series */
  in_start = align_start = 0;
  in_inc = align_inc = 1;
  align_end = align_len - 1;

  for( in_curr = 1; in_curr < in_len; in_curr++ )
  {
    if(( in_days[in_curr] > in_days[ in_curr - 1 ] ) ||
       (( in_days[in_curr] == in_days[ in_curr - 1 ] ) &&
	( in_ms[in_curr] > in_ms[ in_curr - 1 ] )))
      break;
    if(( in_days[in_curr] < in_days[ in_curr - 1 ] ) ||
       (( in_days[in_curr] == in_days[ in_curr - 1 ] ) &&
	( in_ms[in_curr] < in_ms[ in_curr - 1 ] )))
    {
      in_inc = -1;
      in_start = in_len - 1;
      break;
    }
  }

  for( align_curr = 1; align_curr < align_len; align_curr++ )
  {
    if(( align_days[align_curr] > align_days[ align_curr - 1 ] ) ||
       (( align_days[align_curr] == align_days[ align_curr - 1 ] ) &&
	( align_ms[align_curr] > align_ms[ align_curr - 1 ] )))
      break;
    if(( align_days[align_curr] < align_days[ align_curr - 1 ] ) ||
       (( align_days[align_curr] == align_days[ align_curr - 1 ] ) &&
	( align_ms[align_curr] < align_ms[ align_curr - 1 ] )))
    {
      align_inc = -1;
      align_start = align_len - 1;
      align_end = 0;
      break;
    }
  }

  /* create return list */

  if( how == 5 )
  {
    PROTECT(ret = NEW_LIST(6));
    na_data = LOGICAL( SET_VECTOR_ELT(ret, 0 , NEW_LOGICAL(align_len)) );
    drop_data = LOGICAL( SET_VECTOR_ELT(ret, 1 , NEW_LOGICAL(align_len)) );
    weight1_data = REAL( SET_VECTOR_ELT(ret, 2 , NEW_NUMERIC(align_len)) );
    sub1_data = INTEGER( SET_VECTOR_ELT(ret, 3 , NEW_INTEGER(align_len)) );
    weight2_data = REAL( SET_VECTOR_ELT(ret, 4 , NEW_NUMERIC(align_len)) );
    sub2_data = INTEGER( SET_VECTOR_ELT(ret, 5 , NEW_INTEGER(align_len)) );
  } else
  {
    PROTECT(ret = NEW_LIST(3));
    na_data = LOGICAL( SET_VECTOR_ELT(ret, 0 , NEW_LOGICAL(align_len)) );
    drop_data = LOGICAL( SET_VECTOR_ELT(ret, 1 , NEW_LOGICAL(align_len)) );
    sub1_data = INTEGER( SET_VECTOR_ELT(ret, 2 , NEW_INTEGER(align_len)) );
  }

  /* go through the alignment positions and find the right indexes,
     NA or not values, and drop values */

  in_curr = in_start;
  for( align_curr = align_start; align_curr != ( align_end + align_inc ); 
       align_curr += align_inc )
  {
    /* move along the input series until we pass current align position */
    while(( in_curr  >= 0 ) && ( in_curr < in_len ) &&
	  (( in_days[ in_curr ] < align_days[ align_curr ] ) ||
	   (( in_days[ in_curr ] == align_days[ align_curr ] ) && 
	    ( in_ms[ in_curr ] < align_ms[ align_curr ] ))))
      in_curr += in_inc;

    /* see how far we are from the current position */


    if( !ms_to_fraction( align_ms[ align_curr ], &align_num ))
	error( "Cannot convert time to numeric in time_align" );
    align_num += align_days[ align_curr ];

    /* over_set and under_set get set if we're inside the respective ends of 
       the time series; diff_over and diff_under store the difference
       between the align_ time and the in_ times before and after it. */

    over_set = under_set = 0;
    if(( in_curr  >= 0 ) && ( in_curr < in_len ))
    {
      if( !ms_to_fraction( in_ms[ in_curr ], &over_num ))
	error( "Cannot convert time to numeric in time_align" );
      over_num += in_days[ in_curr ];
      diff_over = over_num - align_num; 
      over_set = 1;
    }

    if( (( in_curr - in_inc ) >= 0 ) && (( in_curr - in_inc ) < in_len ))
    {
      if( !ms_to_fraction( in_ms[ in_curr - in_inc ], &under_num ))
	error( "Cannot convert time to numeric in time_align" );
      under_num += in_days[ in_curr - in_inc ];
      diff_under = align_num - under_num; 
      under_set = 1;
    }

    na_data[ align_curr ] = 0;
    drop_data[ align_curr ] = 0;
    sub1_data[ align_curr ] = 1;
    if( how == 5 )
    {
      weight1_data[ align_curr ] = 1.0;
      sub2_data[ align_curr ] = 1;
      weight2_data[ align_curr ] = 0.0;
    }

    /* is it a match? */
    if( under_set && 
       ( !over_set || ( diff_under < diff_over )) && /* better than over */
       ( diff_under <= *pmatch_tol ))
      sub1_data[ align_curr ] = 1 + in_curr - in_inc; /* matches under */
    else if( over_set && ( diff_over <= *pmatch_tol ))
      sub1_data[ align_curr ] = 1 + in_curr;  /* matches over */
    else if(( how == 0 ) || 
	    (( error_how == 0 ) &&
	     (( !under_set && (( how == 3 ) || ( how == 5 ))) ||
	      ( !over_set && (( how == 4 ) || ( how == 5 ))))))
      na_data[ align_curr ] = 1;  /* make it an NA on no match */
    else if(( how == 1 ) || 
	    (( error_how == 1 ) &&
	     (( !under_set && (( how == 3 ) || ( how == 5 ))) ||
	      ( !over_set && (( how == 4 ) || ( how == 5 ))))))
      drop_data[ align_curr ] = 1;  /* drop on no match */
    else if(( how == 2 ) || 
	    (( error_how == 2 ) &&
	     (( !under_set && (( how == 3 ) || ( how == 5 ))) ||
	      ( !over_set && (( how == 4 ) || ( how == 5 ))))))
    {
      /* take nearest on no match */
      if( !under_set )  /* one end */
	sub1_data[ align_curr ] = 1 + in_curr;
      else if( !over_set ) /* other end */
	sub1_data[ align_curr ] = 1 + in_curr - in_inc;
      else if( diff_under <= diff_over ) 
	sub1_data[ align_curr ] = 1 + in_curr - in_inc;
      else
	sub1_data[ align_curr ] = 1 + in_curr;
    } else if( how == 3 ) /* before */
      sub1_data[ align_curr ] = 1 + in_curr - in_inc;
    else if( how == 4 ) /* after */
      sub1_data[ align_curr ] = 1 + in_curr;
    else /* interp */
    {
      sub1_data[ align_curr ] = 1 + in_curr;
      sub2_data[ align_curr ] = 1 + in_curr - in_inc;
      weight1_data[ align_curr ] = diff_under / ( diff_over + diff_under );
      weight2_data[ align_curr ] = diff_over / ( diff_over + diff_under );
    }
  }

  UNPROTECT(5); //1+4 from time_get_pieces

  return( ret );
}



