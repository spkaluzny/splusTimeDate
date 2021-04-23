{
  library(splusTimeDate)
  TRUE
}

{
  # test numeric sequence creation
  a <- new( "numericSequence" )
  b <- numericSequence()
  validObject(a)
  all.equal( a, b )
}

{
  # test numericSequence function
  a <- numericSequence( 5, 15, length = 5 )
  b <- numericSequence( 5, 15, by = 2.5 )
  ( all( a == c( 5, 7.5, 10, 12.5, 15 )) &&
    all( b == c( 5, 7.5, 10, 12.5, 15 )))
}

{
  # test numeric to sequence conversion 
  a <- c( 5, 7.5, 10, 12.5, 15 )
  b <- as( a, "numericSequence" )
  (( class( b ) == "numericSequence" ) &&
   all( b == a ))
}

{
  # test character coercion and format 
  a <- numericSequence( 5, 15, length = 5 )
  b <- c( 5, 7.5, 10, 12.5, 15 )
  ( all( as( a, "character" ) == as( b, "character" )) )
}

{
  a <- numericSequence( 5, 15, by = 2.5 )
  
  (( length( a ) == 5 ))
  ## ( a[3] == 10 ))
}

{
  # test math
  a <- numericSequence( 5, 15, length = 5 )
  b <- c( 5, 7.5, 10, 12.5, 15 )

  ( all( log( a ) == log( b )) &&
    all( abs( a ) == abs( b )) &&
    all( acosh( a ) == acosh( b )) &&
    all( asinh( a ) == asinh( b )) &&
    all( atan( a ) == atan( b )) &&
    all( acosh( a ) == acosh( b )) &&
    all( ceiling( a ) == ceiling( b )) &&
    all( cos( a ) == cos( b )) &&
    all( cosh( a ) == cosh( b )) &&
    all( cumsum( a ) == cumsum( b )) &&
    all( exp( a ) == exp( b )) &&
    all( floor( a ) == floor( b )) &&
    all( gamma( a ) == gamma( b )) &&
    all( lgamma( a ) == lgamma( b )) &&
    all( sin( a ) == sin( b )) &&
    all( sinh( a ) == sinh( b )) &&
    all( tan( a ) == tan( b )) &&
    all( tanh( a ) == tanh( b )) &&
    all( trunc( a ) == trunc( b )) &&
    all( logb( a ) == logb( b )) &&
    all( logb( a, 5 ) == logb( b, 5 )) &&
    all( max( a ) == max( b )) &&
    all( min( a ) == min( b )) &&
    all( prod( a ) == prod( b )) &&
    all( sum( a ) == sum( b )) &&
    all( range( a ) == range( b )) &&
    all( (3 * a + a / 2 ) == ( 3 * b + b / 2 )) &&
    all( ( a >= 10 ) == ( b >= 10 )) &&
    !any( is.na( a )))
}

{
  # test funcs which have be less than 1 args

  a <- numericSequence( 0, .5, length = 5 )
  b <- c( 0, .125, .25, .375, .5 )

  ( all( atanh( a ) == atanh( b )) &&
    all( acos( a ) == acos( b )) &&
    all( asin( a ) == asin( b )))
}

{
  # test misc functions
  a <- numericSequence( 5, 15, length = 5 )
  b <- rev( c( 5, 7.5, 10, 12.5, 15 ))
  
  ( all( match( a, b ) == 5:1 ) &&
    all( unique( a ) == rev( b )) &&
    all( duplicated( a ) == FALSE ) &&
    ( mean( a ) == mean( b )) &&
    ( median( a ) == median( b )) &&
    all( quantile( a ) == quantile( b )) &&
    all( rev( a ) == b ) &&
    all( sort( rev( a )) == sort( b )) &&
    all( c( a, a ) == rep( a, 2 )))

}
