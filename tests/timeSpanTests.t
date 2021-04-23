{
  library(splusTimeDate)
  TRUE
}

{
  # test timeSpan creation
  a <- new( "timeSpan" )
  b <- timeSpan()
  validObject(a)
  all.equal(a,b)
}

{
  # test timeSpan function
  a <- timeSpan( c( "378d 21h 4m 36s 365MS", "378 d", "1y, 13d, 21h 04m" ))
  b <- timeSpan( julian = 50 * (1:10), ms = 234 * (1:10))

  ( all( a@columns[[1]] == 378 ) &&
    all( a@columns[[2]] == c( 75876365, 0, 75840000 )) &&
    all( b@columns[[1]] == 50 * (1:10 )) &&
    all( b@columns[[2]] == 234 * (1:10 )))
}

{
  # test summary and format functions
  a <- timeSpan( "378d 21h 4m 36s 365MS" )
  b <- summary(a)
  ( all( b == "378d 21h 4m 36s 365MS" ) && ( length(b) == 6 ) && 
    ( as(a, "character") == "378d 21h 4m 36s 365MS" ))
}

{
  # test is.na
  b <- timeSpan( jul = c( 1, NA, 55, NA ),
		  ms = c( 1, 34, NA, NA ))

  all( is.na(b) == c(FALSE,TRUE,TRUE,TRUE))
}

{
  # test sort
  a <- c( 1, 5, 3, 7, 2, 10, 4, 6 )
  b <- as( a, "timeSpan" )
  all( sort( b ) == as( sort( a ), "timeSpan" ))
}
{
   # test match    
   all( match( b, b + 1, nomatch=0 ) == match(a,a+1,nomatch=0))
}

{
  # test all formatting specs
  a <- timeSpan( "378d 21h 4m 36s 365MS", format = 
    "%dd %Hh %Mm %Ss %NMS %yy %Dd %Ww %Ed %ss" )

  as(a,"character") == "378d 21h 4m 36s 365MS 1y 13d 54w 0d 75876s"
}

{
  # test subscripting
  b <- as( 1:3, "timeSpan" )
  b[3] <- b[2]
  b[4] <- 5.2
  b[5] <- "378d 50s"
  ( all( b@columns[[1]] == c(1,2,2,5,378)) &&
    all( b@columns[[2]] == c(0,0,0,.2*3600*24*1000,50000)))
}

{
  # test numeric coercion and back
  a <- timeSpan( c( "378d 21h 4m 36s 365MS", "378 d", "1y, 13d, 21h 04m" ))
  all.equal( b, as( as( b, "numeric" ), "timeSpan" ))
}

{
  # test format palette -- output
  a <- timeSpan( "387d 3h 24m 32s 241MS" )
  b <- names( format.timeSpan )
  all( sapply( 1:length(b), 
	 function(n, nms, thedate )
	 {
	   thedate@format <- format.timeSpan[[n]]$output
	   as( thedate, "character" ) == nms[n]
	 }, b, a ))
  
}   

{
  # test format palette -- input
  b <- names( format.timeSpan )
  all( sapply( 1:length(b),
	 function(n, nms)
	 {
	   x <- timeSpan( nms[n], in.format = format.timeSpan[[n]]$input,
			 format=format.timeSpan[[n]]$output )
	   as( x, "character" ) == nms[n]
	 }, b ))
}
