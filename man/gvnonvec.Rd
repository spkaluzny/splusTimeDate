\name{groupVecNonVec}
\alias{groupVecNonVec}
\title{
  \code{groupVec} Extended Class Validation 
}
\description{
Checks whether all slots of an object are either not 
vectors or have \code{length} <= 1 for use in validation checks. 
}
\usage{
groupVecNonVec(object, exceptSlots)
}
\arguments{
\item{object}{
the object whose slots are to be checked. 
}
\item{exceptSlots}{
if present, do not check these slots. 
}
}
\value{
returns \code{TRUE} if the slots in the object are atomic or are not \code{vector} objects (excluding  
the slots in \code{exceptSlots}); otherwise, returns a descriptive string. 
}
\details{
The function loops through all the slots of the object, and for each 
one checks to see that the data in it is either not a \code{vector} class, 
or if it is a \code{vector} class, that it has \code{length} <= 1. 
}
\seealso{
\code{\link{groupVecExtValid}}. 
}
\examples{
setClass("myclass", representation("groupVec", a = "numeric")) 
obj <- new("myclass") 
groupVecNonVec(obj) 
obj@a <- 1:5 
groupVecNonVec(obj) 
groupVecNonVec(obj, "a") 
}
\keyword{methods}
