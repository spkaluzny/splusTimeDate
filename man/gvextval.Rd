\name{groupVecExtValid}
\alias{groupVecExtValid}
\title{
  \code{groupVec} Extended Class Validation 
}
\description{
Checks the validity for classes that extend the \code{groupVec} class. 
}
\usage{
groupVecExtValid(object, names, classes, checkrest = FALSE)
}
\arguments{
\item{object}{
the object to be validated. 
}
\item{names}{
a character vector containing correct column names. 
}
\item{classes}{
a character vector containing correct column classes. 
}
\item{checkrest}{
a logical value. If \code{TRUE} (the default), checks that the non-\code{groupVec} 
slots have \code{length} <= 1. 
}
}
\value{
returns \code{TRUE} if \code{object} is valid; otherwise returns a descriptive string. 
}
\details{
This function checks to see whether an object is a valid \code{groupVec} 
extending object.  These are the steps in this process:  
\enumerate{
\item The \code{groupVecValid} function is called to verify that \code{object} is a valid  
\code{groupVec} object.   
\item The column names in the \code{names} slot of \code{object} are  
checked against the \code{names} argument, and the column classes in  
the \code{classes} slot of \code{object} are checked against the \code{classes}  
argument. 
\item If \code{checkrest} is \code{true}, the \code{groupVecNonVec} function is called to 
check whether the non-\code{groupVec} slots of \code{object} have \code{length} 
<= 1. 
}
}
\seealso{
\code{\link{groupVecValid}},  \code{\link{groupVecNonVec}},  \code{\link{groupVec}}  class  
}
\examples{
setClass("myclass", representation(a = "numeric"), contains="groupVec",
   prototype=prototype(names="nums", classes="numeric",
      columns=list(numeric(0)), a=numeric(0)))
setValidity("myclass",  
   function(object) groupVecExtValid(object, "nums", "numeric", FALSE)) 
obj <- new("myclass") 
obj@a <- 1:5 
validObject(obj) 
groupVecExtValid(obj, "nums", "numeric", TRUE) 
}
\keyword{methods}
