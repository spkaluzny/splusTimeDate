\name{groupVecValid}
\alias{groupVecValid}
\title{
  \code{groupVec} Object Validation 
}
\description{
Checks the validity for \code{groupVec} objects. 
}
\usage{
groupVecValid(object)
}
\arguments{
\item{object}{
the object to be validated. 
}
}
\value{
returns a logical value. If \code{TRUE}, the \code{object} is valid; otherwise, it returns a descriptive string. 
}
\details{
This function validates a \code{groupVec} object by checking whether:  
\enumerate{
\item The \code{object} is actually a \code{groupVec} (or extending class).  
\item The length of the \code{names} slot matches the 
length of the \code{classes} slot, and that both match the list length of 
the \code{columns} slot.  
\item The classes of the vectors comprising the \code{columns} slots 
list have an \code{is} relationship with the corresponding 
class names from the \code{classes} slot.  
\item All the vectors in the \code{columns} slot list are the same length. 
}
}
\seealso{
\code{\link{groupVecExtValid}},  \code{\linkS4class{groupVec}}  class  
}
\examples{
obj <- new("groupVec") 
groupVecValid(obj) 
}
\keyword{methods}
