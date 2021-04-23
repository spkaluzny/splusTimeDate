\name{groupVecNames}
\alias{groupVecNames}
\alias{groupVecNames<-}
\title{
  \code{groupVec} Class Data Access 
}
\description{
Accesses the column names of a \code{groupVec} object. 
}
\usage{
groupVecNames(object)
}
\arguments{
\item{object}{
the object to access. 
}
}
\value{
returns a vector of the column names of the \code{object} data. 
}
\details{
The function returns the \code{names} slot of the \code{object}. 

You can use it on the left side of an assignment to replace the names vector 
with a new value.  In that case, the replacement value is coerced to 
class \code{character} using \code{as}.  Replacing the 
names with a vector of a different length causes  
the \code{classes} and \code{columns} slots of \code{object} 
to be extended or truncated to the new length. (The default class 
is \code{numeric} for the extension.) 
}
\seealso{
\code{\link{groupVecColumn}},  \code{\link{groupVecClasses}},  \code{\link{groupVecData}},  \code{\link{groupVec}}  class  
}
\examples{
obj <- new("groupVec") 
groupVecNames(obj) <- c("colname1", "colname2") 
groupVecNames(obj) 
}
\keyword{methods}
