{
  library(splusTimeDate)
  TRUE
}

{
	save.timeDateOptions <- timeDateOptions()
	timeDateOptions(time.zone="GMT")
	TRUE
}
{
  # test holiday.weekday.number function
  a <- holiday.weekday.number( c(1995, 1996, 1997, 1998, 1999, 2000, 2001),
      5, 1, -1 )
  b <- holiday.weekday.number( c(1995, 1996, 1997, 1998, 1999, 2000, 2001),
      5, 1, 1 )
  ( all( a == timeCalendar( y = c(1995, 1996, 1997, 1998, 1999, 2000, 2001),
       m = 5, d = c( 29, 27,26, 25, 31, 29, 28 ))) &&
    all( b == timeCalendar( y = c(1995, 1996, 1997, 1998, 1999, 2000, 2001),
       m = 5, d = c( 1, 6, 5, 4, 3, 1, 7 ))))
}

{
  # test holiday.fixed and nearest weekday function
  a <- holiday.fixed( c(1995, 1996, 1997, 1998, 1999, 2000, 2001), 2, 14 )
  b <- holiday.nearest.weekday( a )

  ( all( a == timeCalendar( y = c(1995, 1996, 1997, 1998, 1999, 2000, 2001),
       m = 2, d = 14 )) && 
    all( b == timeCalendar( y = c(1995, 1996, 1997, 1998, 1999, 2000, 2001),
       m = 2, d = c( 14, 14, 14, 13, 15, 14, 14 ))))
}

{
  # test holidays function
  a <- holidays( 1998 )
  b <- holidays( 1998, type = c( "Easter", "Anzac", "Australia", 
			     "May", "VE", "Canada", "Bastille", "AllSaints",
			     "Thanksgiving.Canada", "Victoria", 
			     "StPatricks" ))

  ( all( a == timeDate( c( "1/1/1998", "1/19/1998", "2/16/1998", "5/25/1998", 
	               "7/3/1998", "9/7/1998", "10/12/1998", "11/11/1998",
	               "11/26/1998", "12/25/1998" ))) &&
    all( b == timeDate( c( "1/26/1998", "3/17/1998", "4/12/1998", "4/25/1998", 
	               "5/1/1998", "5/8/1998", "5/18/1998", "7/1/1998", 
	               "7/14/1998", "10/12/1998", "11/1/1998" ))))
}

{
  # cleanup
  timeDateOptions(save.timeDateOptions)
  TRUE
}
