{
  library(splusTimeDate)
  TRUE
}

{
  # test timeRelative function and new
  a <- new("timeRelative")
  b <- timeRelative()
  all.equal(a,b)
}

{
  # test timeRelative function
  a <- timeRelative( "blah", timeDate("1/1/96"))
  (all( a@Data == "blah" ) && all( a@holidays == timeDate("1/1/96")))

}

{
  # test subscripting
  allcodes <- c( "ms", "sec", "min", "hr", "day", "wkd", "biz",
		"sun", "mon", "tue", "wed", "thu", "fri", "sat",
		"wk", "mth", "qtr", "yr", "tdy" )
  whichwk <- match( "wk", allcodes )

  allrels <- c( paste( "+1", allcodes, sep=""), 
	        paste( "-1", allcodes, sep=""),
	        paste( "+a2", allcodes[-whichwk], sep=""),
	        paste( "-a2", allcodes[-whichwk], sep=""))
  allnegrels <- c( paste( "-1", allcodes, sep=""), 
	        paste( "+1", allcodes, sep=""),
	        paste( "-a2", allcodes[-whichwk], sep=""),
	        paste( "+a2", allcodes[-whichwk], sep=""))
  hols <- holidays( 1996:2000 )
  allrt <- timeRelative( allrels, hols )

  a <- allrt[1:5]
  b <- a
  b[1] <- a[2]

  ( all( a@Data == allrels[1:5] ) &&
    all( b@Data == allrels[c(2,2:5)] ) &&
    all( a@holidays == hols ) &&
    all( b@holidays == hols ))
}
{
  # test math ops on rel time objects with themselves
  a <- allrt + 2 * allrt
  b <- - allrt

  ( all( a@Data == paste( allrels, allrels, allrels )) &&
    all( b@Data == allnegrels ))
}

{
  # test adding/subtracting rel time with time

  a <- timeCalendar( d=5, m=7, y=1998, h=14, min=10, s=24, ms = 678, 
		    format = "%a %02m/%02d/%Y %02H:%02M:%02S.%03N",
		    zone = "PST")
  all( a + allrt ==
       timeCalendar( zone="PST", 
		    format = "%a %02m/%02d/%Y %02H:%02M:%02S.%03N",
		    d = c( rep(5, 4), rep(6, 3), 12, 6:11, 12, 5, 5, 5, 11,
		           rep(5, 4), 4:2, 28:30, 1:4, 28, 5, 5, 5, 1,
		           rep(5, 4), rep(7, 3), 19, 13:18, 1, 1, 1, 11, 
		           rep(5, 5), 2, 1, 28, 22:27, 1, 1, 1, 1 ),
		    m = c( rep(7, 15), 8, 10, 7, 7, 
		           rep(7, 7), rep(6, 3), rep(7, 4), 6, 6, 4, 7, 7,
		           rep(7, 14), 9, 1, 1, 7, 
		           rep(7, 7), rep( 6, 7), 7, 7, 1, 7 ),
		    y = c( rep( 1998, 17 ), 1999, 1998,
		           rep( 1998, 17 ), 1997, 1998, 
		           rep( 1998, 15 ), 1999, 2000, 1998, 
		           rep( 1998, 18 )),
		    h = c( rep( 14, 3), 15, rep(14, 15),
		           rep( 14, 3), 13, rep(14, 15),
		           rep( 14, 3), 16, rep( 0, 14),
		           rep( 14, 4), rep(0,14)),
		    min = c( 10, 10, 11, rep( 10, 16 ),
		           10, 10, 9, rep(10, 16),
		           10, 10, 12, rep( 0, 15 ),
		           10, 10, 10, rep( 0, 15 )),
		    s = c( 24, 25, rep( 24, 17 ),
		           24, 23, rep( 24, 17 ),
		           24, 26, rep( 0, 16 ),
		           24, 24, rep( 0, 16 )),
		    ms = c( 679, rep( 678, 18 ),
		            677, rep( 678, 18 ),
		            680, rep( 0, 17),
		            676, rep( 0, 17)))
      )
}

{
  # test adding with alignment only 

  zercodes <- c( "sec", "min", "hr", "day", "wkd", "biz",
		"sun", "mon", "tue", "wed", "thu", "fri", "sat",
		"tdy", "mth", "qtr", "yr" )

  zerrels <- c( paste( "+a0", zercodes, sep=""), 
	        paste( "-a0", zercodes, sep=""))
  zerrt <- timeRelative( zerrels, hols )

  a <- timeCalendar( d=5, m=7, y=1998, h=14, min=10, s=24, ms = 678, 
		    format = "%a %02m/%02d/%Y %02H:%02M:%02S.%03N",
		    zone = "PST")
  all( a + zerrt ==
       timeCalendar( zone="PST", 
		    format = "%a %02m/%02d/%Y %02H:%02M:%02S.%03N",
		    d = rep(c( rep(5, 4), 3, 2, 5, 29, 30, 1:4, rep(1, 4 )),2),
		    m = rep(c( rep(7, 7), 6, 6, rep(7, 7), 1 ), 2),
		    y = rep( 1998, 34 ),
		    h = rep(c( rep( 14, 3), rep(0,14)),2),
		    min = rep(c( 10, 10, rep( 0, 15 )),2),
		    s = rep(c(24, rep(0, 16 )), 2 ),
		    ms = rep(0, 34 ))
      )
}

{
  # test misc methods
  length( allrels ) <- 5
  length( allrt ) <- 5
  a <- c( allrt, allrt )
  b <- c( allrels, allrels )

  ( all( as( allrt, "character" ) == allrels ) &&
    length( allrt ) == length( allrels ) &&
    !any( is.na( allrt )) &&
    all( match( allrt, allrels ) == seq( along = allrels )) &&
    all( match( allrels, allrt ) == seq( along = allrels )) &&
    all( as( a, "character" ) == b ) &&
    all( duplicated(a ) == duplicated( b )))
}

{
  # test possible problem cases
  a <- timeDate( "1/31/1990" ) + timeRelative( "+a1day" )
  ( !is.na( a ) && ( a == timeDate( "2/1/1990" )))
}

{
  # test possible problem cases 2
  a <- timeDate( "1/31/1990" ) + timeRelative( "+1day" )
  ( !is.na( a ) && ( a == timeDate( "2/1/1990" )))
}

{
  # test possible problem cases 3

  a <- timeDate( "1/31/1990" ) + timeRelative( "+1mth" )
  ( !is.na( a ) && ( a == timeDate( "2/28/1990" )))
}

{
  # test possible problem cases 4

  a <- timeDate( "2/29/1996" ) + timeRelative( "+1yr" )
  ( !is.na( a ) && ( a == timeDate( "2/28/1997" )))
}

{
  # test possible problem cases 5

  a <- timeDate( "1/31/1990" ) + timeRelative( "+a5day" )
  ( !is.na( a ) && ( a == timeDate( "2/1/1990" )))
}

{
  # test possible problem cases 6

  a <- timeDate( "1/1/1990" ) + timeRelative( "-a1day" )
  ( !is.na( a ) && ( a == timeDate( "12/31/1989" )))
}

{
  # test possible problem cases 7

  a <- timeDate( "1/1/1990" ) + timeRelative( "-a10day" )
  ( !is.na( a ) && ( a == timeDate( "12/31/1989" )))
}

{
  # test possible problem cases 8

  a <- timeDate( "1/1/1990" ) + timeRelative( "-1day" )
  ( !is.na( a ) && ( a == timeDate( "12/31/1989" )))
}

{
  # test possible problem cases 9

  a <- timeDate( "1/1/1990" ) + timeRelative( "-1mth" )
  ( !is.na( a ) && ( a == timeDate( "12/1/1989" )))
}

{
  # test possible problem cases 10

  a <- timeDate( "1/1/1990" ) + timeRelative( "-a1mth" )
  ( !is.na( a ) && ( a == timeDate( "12/1/1989" )))
}

{
  # test possible problem cases 11

  a <- timeDate( "2/29/96" ) + timeRelative( "-1yr" )
  ( !is.na( a ) && ( a == timeDate( "2/28/1995" )))
}

{
  # test possible problem cases 12

  a <- timeDate( "2/29/96" ) + timeRelative( "+a2tdy" )
  ( !is.na( a ) && ( a == timeDate( "3/1/1996" )))
}
