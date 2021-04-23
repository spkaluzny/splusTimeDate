{
  library(splusTimeDate)
  TRUE
}

{
  # test event creation
  a <- new("timeEvent")
  b <- timeEvent()
  validObject(a)
  all.equal(a,b)
}

{
  # test timeEvent function
  atm <- timeCalendar( 1:10 )
  btm <- timeDate( jul = 10 * ( 1:10 ))
  a <- timeEvent( atm )
  b <- timeEvent( btm, btm + 5, letters[1:10] )

  ( all( groupVecColumn( a, "start" ) == atm ) &&
    all( groupVecColumn( a, "end" ) < atm + 1 ) &&
    all( groupVecColumn( b, "start" ) == btm ) &&
    all( groupVecColumn( b, "end" ) == btm + 5 ) &&
    all( groupVecColumn( b, "IDs" ) == letters[1:10] ))
}

{
  # test subscripting
  b <- a
  b[3] <- a[2] 
  btm <- atm
  btm[3] <- btm[2]
  all.equal( a[2], timeEvent( atm[2] ))
}
{
  # subscript test part 2
    all.equal( b, timeEvent( btm ))
}

{
  # test coercion from time
  all.equal( as( atm, "timeEvent" ), a )
}

{
  # summary method:
  te02 <- timeEvent(start = timeCalendar(m=2, d=4:8, y=2013, h=8),
    end = timeCalendar(m=2, d=5:9, y=2013, h=11:15),
    ID = c("Mo", "Tu", "We", "Th", "Fr"))
  ste02 <- summary(te02)
  all(ste02[,1] ==
    c("02/04/2013 08:00:00.000", "02/08/2013 08:00:00.000", "0")) &&
    all(ste02[,2] ==
      c("02/05/2013 11:00:00.000", "02/09/2013 15:00:00.000", "0")) 
}
