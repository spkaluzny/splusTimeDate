{
  library(splusTimeDate)
  TRUE
}

{ 
  save.timeDateOptions <- timeDateOptions(time.zone="GMT")
  timeDateOptions(time.zone="GMT")
  TRUE
}

{
  # test time creation
  a <- new( "timeDate" )
  b <- timeDate()
  validObject(a)
  all.equal(a,b)
}

{
  # test time, mdy, hms functions
  a <- timeDate( c( "9/5/1997 14:36:24.3", "January 25, 1984 5:30 PM" ))
  b <- cbind( mdy(a), hms(a))
  a <- timeDate( julian = 50 * (1:10), in.origin = c(month=1, day=10, year=1960 ))
  all.equal(b,data.frame( month=c(9,1),day=c(5,25),year=c(1997,1984),
			   hour=c(14,17),minute=c(36,30),second=c(24,0),
			   ms=c(300,0)))
}

{
  # test time, mdy, hms functions continued

   ( all( a@columns[[1]] == (50 * (1:10 ) + 9 )) &&
    all( a@columns[[2]] == 0 ) &&
    all( hms(timeDate( "blah hi 3", in.format = "%w %2c %N" ))$ms 
	 == 300 ))
}

{
  # test timeCalendar function
  a <- timeCalendar( 1, 2, 1933, 4, 5, 6, 777 )
  b <- cbind( mdy(a), hms(a))
  all.equal(b,data.frame( month=1, day = 2, year = 1933, hour = 4, minute = 5,
			  second = 6, ms = 777 ))
}

{
  # test summary and format functions
  a <- timeCalendar( 1, 2, 1933, 4, 5, 6, 777 )
  b <- summary(a)
  all( b == "01/02/1933 04:05:06.777" ) && ( length(b) == 6 ) && 
    ( as(a, "character") == "01/02/1933 04:05:06.777" )
}

{
  # test timeAlign function
  a <- timeCalendar( d=5, m=7, y=1998, h=14, min=10, s=22, ms = 678, 
		    format = "%a %02m/%02d/%Y %02H:%02M:%02S.%03N",
		    zone = "PST")
   all.equal( timeAlign(a, "milliseconds", 2, -1 ), a )
}
{
  # test timeAlign function # 2

    all.equal( timeAlign(a, "seconds", 3 ), a + timeSpan(ms=2000 - 678) )
}
{
  # test timeAlign function # 3

    all.equal( timeAlign(a, "minutes", 30, -1), 
  	timeCalendar( d=5, m=7, y=1998, h=14, 
		    format = "%a %02m/%02d/%Y %02H:%02M:%02S.%03N",
		    zone = "PST"))
}
{
  # test timeAlign function # 4

    all.equal( timeAlign(a, "hours", 6), 
  	timeCalendar( d=5, m=7, y=1998, h=18, 
		    format = "%a %02m/%02d/%Y %02H:%02M:%02S.%03N",
		    zone = "PST"))
}
{
  # test timeAlign function # 5

    all.equal( timeAlign(a, "days", direction=-1), 
  	timeCalendar( d=5, m=7, y=1998, 
		    format = "%a %02m/%02d/%Y %02H:%02M:%02S.%03N",
		    zone = "PST"))
}
{
  # test timeAlign function # 6

    all.equal( timeAlign(a, "weekdays"), 
  	timeCalendar( d=6, m=7, y=1998, 
		    format = "%a %02m/%02d/%Y %02H:%02M:%02S.%03N",
		    zone = "PST"))
}
{
  # test timeAlign function # 7

    all.equal( timeAlign(a, "bizdays", direction=-1, 
	                 holidays=timeDate("7/3/1998")), 
  	timeCalendar( d=2, m=7, y=1998, 
		    format = "%a %02m/%02d/%Y %02H:%02M:%02S.%03N",
		    zone = "PST"))
}
{
  # test timeAlign function # 8

    all.equal( timeAlign(a, "weeks", week.align="TUE"), 
  	timeCalendar( d=7, m=7, y=1998, 
		    format = "%a %02m/%02d/%Y %02H:%02M:%02S.%03N",
		    zone = "PST"))
}
{
  # test timeAlign function # 9

    all.equal( timeAlign(a, "months", direction=-1),
  	timeCalendar( d=1, m=7, y=1998, 
		    format = "%a %02m/%02d/%Y %02H:%02M:%02S.%03N",
		    zone = "PST"))
}
{
  # test timeAlign function # 10

    all.equal( timeAlign(a, "quarters"),
  	timeCalendar( d=1, m=10, y=1998, 
		    format = "%a %02m/%02d/%Y %02H:%02M:%02S.%03N",
		    zone = "PST"))
}
{
  # test timeAlign function # 11

    all.equal( timeAlign(a, "years", direction=-1),
  	timeCalendar( d=1, m=1, y=1998, 
		    format = "%a %02m/%02d/%Y %02H:%02M:%02S.%03N",
		    zone = "PST"))
}

{
  # test timeSeq function
    all.equal( timeSeq( "1/1/1995", length = 10 ), 
	       timeCalendar( d=1:10, y = rep(1995,10)))   
}
{
  # test timeSeq function # 2
    all.equal( timeSeq( to = "1/1/1995", length = 10, by = "months" ),
	       timeCalendar( m = c(4:12,1), y = c(rep(1994,9),1995)))
    
}
{
  # test timeSeq function # 3
    all.equal( timeSeq( from = "1/1/1995", to = "1/10/1995", length = 10 ),
	       timeCalendar( d=1:10, y = rep(1995,10),
                            format=timeDateOptions("time.out.format.notime")[[1]]))
}
{
  # test timeSeq function # 4

    all.equal( timeSeq( "1/1/1995", length = 10, by = "weekdays", 
	align.by=TRUE ),
	       timeCalendar( d=c(2:6,9:13), y = rep(1995,10 )))
}
{
  # test timeSeq function # 5

    all.equal( timeSeq( "1/1/1995", length = 10, by = "years" ),
	       timeCalendar( y = 1995:2004 ))
}

{
  # test is.na
  b <- timeCalendar( y = c( 1998, NA, 2005, NA ), 
		     h = c( 14, 16, NA, NA ))
  all( is.na(b) == c(FALSE,TRUE,TRUE,TRUE))
}

{
  # test sort and match
  a <- c( 1, 5, 3, 7, 2, 10, 4, 6 )
  b <- as( a, "timeDate" )
  ( all( sort( b ) == as( sort( a ), "timeDate" )) &&
    all( match( b, b + 1, nomatch=0 ) == match(a,a+1,nomatch=0)))
}

{
  # test all formatting specs
  b <- timeCalendar( c(1,5), c(23,12), c(1998,2005), c(14,1), 
		   c(5,44), c(6,29), c(624,319), format = 
 "%m/%d/%Y %H:%02M:%02S.%03N %a %A %b %B %C %I %N %p %q %Q %Z %z %% %5B %05d %1N %02N" )

  all( as(b,"character") == 
c( "1/23/1998 14:05:06.624 Fri Friday Jan January 98 2 624 PM 1 I GMT GMT % Janua 00023 6 62",
 "5/12/2005 1:44:29.319 Thu Thursday May May 5 1 319 AM 2 II GMT GMT %   May 00012 3 31" ))
}


{
  # test subscripting
  b <- as( 1:3, "timeDate" )
  b[3] <- b[2]
  b[4] <- 5.2
  b[5] <- "1/20/1960"
  ( all( b@columns[[1]] == c(1,2,2,5,19)) &&
    all( b@columns[[2]] == c(0,0,0,.2*3600*24*1000,0)))
}

{
  # test numeric coercion and back
  b <- timeCalendar( c(1,5), c(23,12), c(1998,2005), c(14,1), 
		     c(5,44), c(6,29), c(624,319))
  all.equal( b, as( as( b, "numeric" ), "timeDate" ))
}

{
  # test days, weekdays, etc
  b <- timeCalendar( c(1,5), c(23,12), c(1998,2005), c(14,1), 
		     c(5,44), c(6,29), c(777,3))
  ( all( as.character(days(b)) == c("23","12") ) &&
    all( as.character(weekdays(b)) == c( "Fri", "Thu" )) &&
    all( as.character(months( b )) == c( "Jan", "May" )) &&
    all( as.character(quarters(b)) == c( "1Q", "2Q" )) &&
    all( as.character(years(b)) == c( "1998", "2005" )) &&
    all( hours(b) == c(14,1)) &&
    all( minutes(b) == c( 5,44)) &&
    all( seconds(b) == c( 6.777, 29.003 )))
}

{
  # test time zones
  # input in GMT
  b <- timeCalendar( c(1,5), c(23,12), c(1998,2005), c(14,1), 
		     c(5,44), c(6,29), c(777,3))

  # a as if inputs were PST to begin with
  a <- timeZoneConvert( b, "PST/PDT" )
  a@format <- "%z"
  # display b in PST
  b@time.zone <- "PST/PDT"
 
  ( all( hours( b ) == c( 6, 18 )) &&
    all( as.character(days(b)) == c( "23", "11" )) &&
    all( hours( a ) == c( 14, 1 )) &&
    all( as( a, "character" ) == c( "PST", "PDT" )))
}

{
  # test user-defined time zone creation
  all.equal( timeZoneR(), new("timeZoneR"))
}

{
  # test user-defined time zones

  # time zone to with daylight time 
  # starting last Sunday in April, end last Sunday in Oct.
  b <- timeZoneR( offset = 3600,
		  yearfrom=c( -1, 2000), yearto=c( 1999, -1 ), 
		  hasdaylight=c( TRUE, TRUE ), dsextra=c( 3600, 3600 ), 
		  monthstart=c( 4, 4 ), codestart=c( 2, 2 ), 
		  daystart=c( 0, 0 ), xdaystart=c(0,0),
		  timestart=c( 7200, 7200 ), 
		  monthend=c( 10, 10 ), codeend=c( 2, 2 ), 
		  dayend=c( 0, 0 ), xdayend=c(0,0),
		  timeend=c(3*3600, 3*3600))
  oldlist <- timeZoneList(mytz=b)

  a <- timeCalendar( c(1,5), c(23,12), c(1998,2005), c(14,1), 
		     c(5,44), c(6,29), c(777,3), zone="mytz",
	format="%m %d %Y %H %M %S %N %Z" )
  
  all( as( a, "character") == 
       c("1 23 1998 14 5 6 777 mytz", "5 12 2005 1 44 29 3 mytz"))
}
{
  # test user time zones continued -- conversion back to GMT
  a@time.zone <- "GMT"
  all( as( a, "character") == 
       c("1 23 1998 13 5 6 777 GMT", "5 11 2005 23 44 29 3 GMT"))
}

{
  # test century option
  a <- function( century, str )
  {
    oldopt <- timeDateOptions( time.century = century )
    on.exit( timeDateOptions( oldopt ))
    timeDate( str )
  }

  ( all( a( 1900, c( "1/1/05", "1/1/55", "1/1/98" )) == 
       timeDate( c( "1/1/1905", "1/1/1955", "1/1/1998" ))) &&
    all( a( 1925, c( "1/1/05", "1/1/55", "1/1/98" )) == 
       timeDate( c( "1/1/2005", "1/1/1955", "1/1/1998" ))) &&
    all( a( 1975, c( "1/1/05", "1/1/55", "1/1/98" )) == 
       timeDate( c( "1/1/2005", "1/1/2055", "1/1/1998" ))))
}

{
  # test format palette -- output
  a <- timeDate( "01/03/1998 14:04:32", zone="PST" )
  b <- names( format.timeDate )
  all( sapply( 1:length(b), 
	 function(n, nms, thedate )
	 {
	   thedate@format <- format.timeDate[[n]]$output
	   as( thedate, "character" ) == nms[n]
	 }, b, a ))
  
}   

{
  # test format palette -- input
  b <- names( format.timeDate )
  all( sapply( 1:length(b),
	 function(n, nms, zone )
	 {
	   x <- timeDate( nms[n], in.format = format.timeDate[[n]]$input,
			 format=format.timeDate[[n]]$output,
			 zone=zone)
	   as( x, "character" ) == nms[n]
	 }, b, "PST" ))
}

{
  # cleanup
  timeZoneList(oldlist)
  timeDateOptions(save.timeDateOptions)
  TRUE
}
