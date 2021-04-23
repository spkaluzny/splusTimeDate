\name{groupVecClasses}
\alias{groupVecClasses}
\alias{groupVecClasses<-}
\title{
  \code{groupVec} Class Data Access 
}
\description{
  Accesses or replacees the column classes vector of a \code{groupVec} object. 
}
\usage{
groupVecClasses(object)
}
\arguments{
  \item{object}{
    the object to access. 
  }
}
\value{
  returns a vector of classes of \code{object} data. 
}
\details{
This function returns the \code{classes} slot of \code{object}. 

It can also be used on the left side of an assignment to replace the \code{classes} 
slot with a new value.  In that case, the data in the object columns 
are coerced to the new classes.  Also, replacement of the 
classes with a vector of a different length causes  
the \code{names} and \code{columns} slots of \code{object} 
to be extended or truncated to the new length. 
}
\seealso{
\code{\link{groupVecColumn}},  \code{\link{groupVecNames}},  \code{\link{groupVecData}},  \code{\linkS4class{groupVec}}  class.  
}
\examples{
obj <- new("groupVec") 
groupVecClasses(obj) <- c("numeric", "character") 
groupVecClasses(obj) 
}
\keyword{methods}
% docclass is function
