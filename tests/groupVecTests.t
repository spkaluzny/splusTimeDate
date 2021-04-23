{
  library(splusTimeDate)
  TRUE
}

{
  # test GroupVec creation
  testcols <- c( "nums", "ints" )
  testclasses <- c( "numeric", "integer" )
  testdata <- list( c(1.1, 2.2, 3.3), as.integer(c( 1,2,3 )))

  a <- new( "groupVec" )
  b <- groupVec()
  validObject(a)
  all.equal(a,b)
}

{
  # test groupVec function
  a <- groupVec( testcols, testclasses, testdata )
  validObject(a)
  all.equal(a@columns, testdata )
}

{
 # groupVec function test 2
   all.equal(a@names, testcols )
}

{
 # groupVec function test 3

   all.equal(a@classes, as( testclasses, "ANY" ))
}

{
  # test summary function
  a <- groupVec( testcols, testclasses, testdata )
  b <- summary(a)
  all.equal( dimnames(b), list( testcols, 
	       c("Slot/Column", "Length", "Class")))
}

{
  # summary function test part 2
   ( all( b[,2] == 3 ) &&
    all( b[,1] == "Column" ) &&
    all( b[,3] == format( testclasses )))
}

{
  # test groupVecColumn function
  a <- groupVec( testcols, testclasses, testdata )
  b <- groupVecColumn( a, testcols[1] )
  groupVecColumn( a, testcols[1] ) <- testdata[[1]] + 5 
  all.equal( b, testdata[[1]] )
}

{
  # groupVecColumn function test 2
    all.equal( groupVecColumn(a, testcols[1]), testdata[[1]] + 5 )
}

{
  # test groupVecClasses function
  a <- groupVec( testcols, testclasses, testdata )
  b <- groupVecClasses(a)
  all.equal( as(b,"character"), testclasses )
}

{
  # groupVecClasses function test 2
  groupVecClasses(a) <- rep( testclasses, 2 )
  all.equal( as(groupVecClasses(a),"character"), rep( testclasses,2))
}

{
  # test groupVecNames function
  a <- groupVec( testcols, testclasses, testdata )
  b <- groupVecNames(a)
  groupVecNames(a) <- rep(testcols,2)
  all.equal( b, testcols )
}

{
  # groupVecNames function test 2
    all.equal( groupVecNames(a), rep( testcols, 2 ))
}

{
  # test groupVecData function
  a <- groupVec( testcols, testclasses, testdata )
  b <- groupVecData(a)
  groupVecData(a) <- rep(testdata, 2)
  all.equal( b, testdata )
}

{
  # groupVecData function test 2
    all.equal( groupVecData(a), rep( testdata, 2 ))
}
{
  # test subscripting
   a <- groupVec( testcols, testclasses, testdata )
   b <- a[2]
   all.equal( groupVecData(b), lapply(testdata,function(x) x[2] ))
}
{
  # subscripting test 2
   a[2] <- a[3]
   all.equal( a[2], a[3] )
}

{
  # test length 
   a <- groupVec( testcols, testclasses, testdata )
   b <- length( a )
   length( a ) <- 2
   (( b == 3 ) && ( length( a ) == 2 ))
}

{
  # test is.na
  a <- groupVec( testcols, testclasses, testdata )
  groupVecData( a ) <- list( c( 2.2, 3.3, NA ), as.integer(c( 1, NA, 2 )))
  all( is.na(a) == c( FALSE, TRUE, TRUE ))
}

{
  # test concat
   a <- groupVec( testcols, testclasses, testdata )
   a <- c( a, a )
   all.equal( groupVecColumn(a,testcols[1]), rep( testdata[[1]], 2 ))
}

{
  # concat test 2
     all.equal( groupVecColumn(a,testcols[2]), rep( testdata[[2]], 2 ))
}
