\name{groupVecData}
\alias{groupVecData}
\alias{groupVecData<-}
\title{
  \code{groupVec} Class Data Access 
}
\description{
Accesses the data list of a \code{groupVec} object. 
}
\usage{
groupVecData(object)
}
\arguments{
\item{object}{
the object to access. 
}
}
\value{
returns the data list of \code{object}. 
}
\details{
The function returns the \code{columns} slot of \code{object}.  

You can use it on the left side of an assignment, in which case the \code{columns} slot 
is replaced, with some validity checking.  Also, if 
the new value has a different length than the old one, the column names and 
classes are extended or truncated appropriately, with the column 
classes for new columns derived from the class of the new data in the columns. 
}
\seealso{
\code{\link{groupVecColumn}},  \code{\link{groupVecNames}},  \code{\link{groupVecClasses}},  \code{\linkS4class{groupVec}}  class.  
}
\examples{
obj <- new("groupVec") 
groupVecData(obj) <- list(c(1,2,3), c("a", "b", "c")) 
groupVecData(obj) 
}
\keyword{methods}
% docclass is function
