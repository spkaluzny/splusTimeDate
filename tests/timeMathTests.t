{
  library(splusTimeDate)
  TRUE
}

{ 
	save.options <- timeDateOptions()
	timeDateOptions(time.zone="GMT")
	TRUE
}

{
  # test arith ops with numbers
  a <- c( 1, 2 )
  b <- c(3.4456, -12.9384)
  ( all( ( as(a,"timeDate" ) + b )  == as( a + b, "timeDate" )) &&
    all( ( as(a,"timeDate" ) - b )  == as( a - b, "timeDate" )) &&
    all( ( b + as(a,"timeDate" ))  == as( b + a, "timeDate" )) &&
    all( ( as(a,"timeSpan" ) + b )  == as( a + b, "timeSpan" )) &&
    all( ( as(a,"timeSpan" ) - b )  == as( a - b, "timeSpan" )) &&
    all( ( b * as(a,"timeSpan" ))  ==  b * a))
}

{
  # test arith between times and/or time spans
  a <- c( 1, 2 )
  b <- c(3.4456, -12.9384)

  ( all(( as( a, "timeDate" ) - as( b, "timeDate" )) == as( a - b, "timeSpan" )) &&
    all(( as( a, "timeDate" ) + as( b, "timeSpan" )) == as( a + b, "timeDate" )) &&
    all(( as( a, "timeSpan" ) - as( b, "timeSpan" )) == 
	as( a - b, "timeSpan" )) &&
    all( - as( a, "timeSpan" ) == as( -a, "timeSpan" )))
}

{
  # test summary ops
  a <- as( 1:5, "timeDate" )
  b <- as( 6:10, "timeSpan" )
  ( all( range( a ) == as( c( 1,5), "timeDate" )) &&
    all( range( b ) == as( c( 6, 10 ), "timeSpan" )) &&
    ( sum( b ) == as( sum( 6:10 ), "timeSpan" )) &&
    all( cumsum( b ) == as( cumsum( 6:10 ), "timeSpan" )))
}
    
{
  # test math functions
  a <- as( c( -1.2, 3.3 ), "timeDate" )
  b <- as( c( -1.2, 3.3 ), "timeSpan" )
atU

  ( all( timeFloor( a ) == as( c( -2, 3 ), "timeDate" )) &&
    all( timeFloor( b ) == as( c( -2, 3 ), "timeSpan" )) &&
    all( timeCeiling( a ) == as( c( -1, 4 ), "timeDate" )) &&
    all( timeCeiling( b ) == as( c( -1, 4 ), "timeSpan" )) &&
    all( timeTrunc( b ) == as( c( -1, 3 ), "timeSpan" )))
}

{
  # test compare ops
  a <- as( 1:5, "timeDate" )
  b <- as( 1:5, "timeSpan" )
  ( all(( a < as( 3, "timeDate" )) == c( TRUE, TRUE, FALSE, FALSE, FALSE )) &&
    all(( b > as( 3, "timeSpan" )) == c( FALSE, FALSE, FALSE, TRUE, TRUE )) &&
    all(( a <= as( 3, "timeDate" )) == c( TRUE, TRUE, TRUE, FALSE, FALSE )) &&
    all(( b >= as( 3, "timeSpan" )) == c( FALSE, FALSE, TRUE, TRUE, TRUE )) &&
    all(( a == as( 3, "timeDate" )) == c( FALSE, FALSE, TRUE, FALSE, FALSE )) &&
    all(( b != as( 3, "timeSpan" )) == c( TRUE, TRUE, FALSE, TRUE, TRUE )))
}

{
  # test basic stats
  a <- c( 1:5, 2, 89 )
  # b <- a + rnorm(7)
  # above b with set.seed(42):
  b <- c(2.37095844714667, 1.43530182860391, 3.36312841133734,
    4.63286260496104, 5.404268323141, 1.89387548390852, 90.5115219974389)
  ( all( mean( as( a, "timeDate" )) == as( mean( a ), "timeDate" )) &&
    all( mean( as( a, "timeSpan" )) == as( mean( a ), "timeSpan" )) &&
    all( median( as( a, "timeDate" )) == as( median( a ), "timeDate" )) &&
    all( median( as( a, "timeSpan" )) == as( median( a ), "timeSpan" )) &&
    all( quantile( as( a, "timeDate" )) == as( quantile( a ), "timeDate" )) &&
    all( quantile( as( a, "timeSpan" )) == as( quantile( a ), "timeSpan" )) &&
    all( var( a ) == var( as( a, "timeDate" ))) &&
    all( var( a ) == var( as( a, "timeSpan" ))) &&
    all.equal( cor( a, b ) , cor( as( a, "timeDate" ), as( b, "timeDate" ))) &&
    all.equal( cor( a, b ) , cor( as( a, "timeSpan" ), as( b, "timeSpan" ))))
}

{
  # test diff
  a <- c( 1,3,4,2,3,5,6)
  
  ( all( as( diff( a, 3, 2 ), "timeSpan" ) == 
	 diff( as( a, "timeDate" ), 3, 2 )) &&
    all( as( diff( a, 3, 2 ), "timeSpan" ) == 
	diff( as( a, "timeSpan" ), 3, 2 )))
  
}

{
  # test cut, ordered, factor
  a <- c( 1,3,4,2,3,5,6)
  b <- as( a, "timeDate" )
  a <- as( a, "timeSpan" )

  ( ##all( as.character(factor( b )) == as( b, "character" )) &&
    ##all( as.character(factor( a )) == as( a, "character" )) &&
    all( as.numeric(cut( a, c( 0, 3, 7 ), right=FALSE)) == 
	    as.numeric(cut( as( a, "numeric" ), c( 0, 3, 7 ), right=FALSE))) &&
    all( as.numeric(cut( b, c( 0, 3, 7 ), right=TRUE)) == 
	    as.numeric(cut( as( b, "numeric" ), c( 0, 3, 7 ), right=TRUE))))
    ##all( as.numeric( ordered( a )) == as.numeric(ordered(as(a,"numeric")))) &&
    ##all( as.numeric( ordered( b )) == as.numeric(ordered(as(b,"numeric")))))
}

{
  # cleanup
  timeDateOptions(save.options)
  TRUE
}
          
