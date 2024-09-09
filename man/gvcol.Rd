\name{groupVecColumn}
\alias{groupVecColumn}
\alias{groupVecColumn<-}
\title{
  \code{groupVec} Class - Data Access 
}
\description{
Accesses the columns of a \code{groupVec} object. 
}
\usage{
groupVecColumn(object, col.name)
}
\arguments{
\item{object}{
the object to access. 
}
\item{col.name}{
the name of columns to access. 
}
}
\value{
returns the data in the given columns.  
\itemize{
\item If only one column is supplied, 
the result is the column
\item If more than one name is supplied, 
the result is a list of columns. 
} 
}
\details{
The function finds the given column by comparing \code{col.name} 
to the \code{names} slot of \code{object}, and then extracts the corresponding 
vectors from the list in the \code{columns} slot of \code{object}.  

You can use this function on the left side of an assignment to replace the given 
columns with new values.   
\itemize{
\item If you replace one column, supply the value as a vector.
\item If you replace multiple columns, supply the values in a list. 
}
The new data is coerced to the \code{columns} class using 
\code{as} before the replacement, and the column lengths are checked. 
}
\seealso{
\code{\link{groupVecData}},  \code{\link{groupVecNames}},  \code{\link{groupVecClasses}},  \code{\linkS4class{groupVec}}  class.  
}
\examples{
obj <- new("groupVec") 
groupVecNames(obj) <- "colname1" 
groupVecColumn(obj, "colname1") <- c(1, 2, 3) 
groupVecColumn(obj, "colname1") 
}
\keyword{methods}
