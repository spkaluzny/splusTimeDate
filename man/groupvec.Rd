\name{groupVec}
\alias{groupVec}
\title{
  \code{groupVec} Constructor 
}
\description{
Constructor function for \code{groupVec} class objects. 
}
\usage{
groupVec(names, classes, columns)
}
\arguments{
  \item{}{
    The function can be called with no arguments, but if it is called 
    with any arguments, \code{names} must be supplied.
    }
  \item{names}{
    the column names for the object. 
  }
  \item{classes}{
    the column classes for the object.  If not supplied, it is
    initialized as \code{ANY}, replicated to the length of \code{names}. 
  }
  \item{columns}{
    the list for column data.  If not supplied, it is initialized 
    to \code{new(class)} for each class in \code{classes}. 
  }
}
\value{
  returns a new \code{groupVec} object constructed from the arguments, 
  or an empty one if no arguments are supplied. 
}
\details{
  This function creates a new \code{groupVec} object, putting  
  the \code{names} (if supplied) into the \code{names} slot,  
  the \code{classes} (if supplied) into the \code{classes} slot,  
  and the \code{columns} (if supplied) into the \code{columns} slot.   
}
\seealso{
\code{\linkS4class{groupVec}}
}
\examples{
groupVec() 
groupVec(c("my.nums", "my.chars"), c("numeric", "character")) 
groupVec(c("my.nums", "my.chars"), c("numeric", "character"), 
	  list(c(1, 2, 3), c("a", "b", "c"))) 
}
\keyword{methods}

