\name{groupVec-class}
\alias{groupVec-class}
\alias{[,groupVec-method}
\alias{[<-,groupVec,ANY,ANY,ANY-method}
\alias{[<-,groupVec,ANY,ANY,groupVec-method}
\alias{[<-,groupVec,ANY,ANY,list-method}
\alias{[[,groupVec-method}
\alias{[[<-,groupVec-method}
\alias{all_equal_list,groupVec-method}
\alias{c,groupVec-method}
\alias{duplicated,groupVec-method}
\alias{is.finite,groupVec-method}
\alias{is.infinite,groupVec-method}
\alias{is.na,groupVec-method}
\alias{is.nan,groupVec-method}
\alias{length,groupVec-method}
\alias{length<-,groupVec-method}
\alias{names,groupVec-method}
\alias{rep,groupVec-method}
\alias{show,groupVec-method}
\alias{summary,groupVec-method}
\alias{unique,groupVec-method}
\alias{which.na,groupVec-method}
%
\title{
  Group Vector Class 
}
\docType{class}
\description{
\code{groupVec} is a class that groups a list of vectors so that 
they can act like a single atomic vector.
}
\section{Objects from the Class}{
  Create objects using calls of the form \code{new("groupVector",
    ...)} or \code{groupVector}.
}
\section{Slots}{
\describe{
\item{columns}{
(\code{list}) the list of vector columns. 
}
\item{names}{
(\code{character}) the column names. 
}
\item{classes}{
(\code{character}) the column classes. 
}
}
}
\details{
\code{groupVec} is a class for objects that act as vectors but 
are actually made up of a collection of vectors of the same length, and 
which are accessed together and would usually be expected to be printed 
as if they were simple atomic vectors.  

The \code{groupVec} class manages the basic vector operations, such 
as subscripting and length. Extending classes must manage the meaning 
of the collection of vectors and more specific operations such as  
character coercion. Extending classes can have other information  
(although probably not the same length) in other slots. Subscripting 
on a \code{groupVec}-extended class carries this information along unchanged. 

A \code{groupVec} object also has the following characteristics:

\itemize{
\item A valid \code{groupVec} object holds a list of vector "columns" with names and 
classes, where the vectors in the list correspond one-to-one with the 
names and classes.
\item Each vector satisfies an \code{is} relationship with its 
class.
\item All the vectors are the same length.  
}

The object is intended to be used as a base class for defining other 
classes with fixed column names and classes but could also be used 
as a more dynamic list.  

The difference between a \code{groupVec} and a \code{data.frame}, \code{matrix}, or other 
rectangular structure is that a \code{groupVec} is meant to hold objects 
whose columns really should not be considered as separate observations. 
For example, the \code{timeDate} class is a \code{groupVec} whose columns are 
the date and time of day in GMT, with additional slots for output format and 
time zone.  Because the date can affect the time of day in the local time 
zone (if there is daylight savings time), and the time of day can 
affect the date in the local time zone (if the time zone conversion 
crosses a date boundary), and because each time and date in the  
\code{timeDate} object is displayed as a single character string containing  
both time and date, the time and date columns are not meant to be 
accessed separately, and in fact the user does not even have to know 
that they are stored separately.   

The objects in \code{groupVec} columns do not have to be atomic vectors. They 
can be any class that has a well-defined length.  This design allows one 
\code{groupVec} object to hold other \code{groupVec} objects, lists, \code{data.frames}, 
any type of R vectors, and user-defined types in its columns.  

If type checking is not desired for a column, the column class can be set to  
\code{"ANY"}, because any R object has an \code{is} relationship with the special 
\code{"ANY"} class.  
}
\section{GroupVec functions}{
  \itemize{
  \item \code{groupVec}: The \code{groupVec} constructor function.
  \item \code{groupVecValid}: a \code{groupVec} class validity function. 
  \item \code{groupVecExtValid}: a validity function that user-defined extending  
  classes with set column names and column classes can use for 
  checking validity.
  }
  Although the slots of a \code{groupVec} can be accessed directly, it is not 
  recommended.  Instead, there are several access functions that you can  
  use on the left or right side of assignments, and that preserve the 
  validity of \code{groupVec} objects. These functions are:
  \itemize{
  \item \code{groupVecColumn}, for accessing a single column.
  \item \code{groupVecData}, for accessing the entire data 
  list.
  \item \code{groupVecNames}, for accessing the column names.
  \item \code{groupVecClass}, for accessing the column classes.
  }
  See individual function documentation for more information.
  }
\section{Methods}{
  Several methods have been defined for \code{groupVec} objects for standard 
  vector operations. All operate on \code{groupVec} objects
  as if they were atomic vectors: 
  \itemize{
  \item subscripting with \code{[]} and \code{[[]]} on the
  left or right side of an assignment.
  \item \code{length} on the left or right side of an assignment. 
  \item \code{c}, and \code{is.na}   
}
}
\seealso{
\code{\link{groupVecValid}},  \code{\link{groupVecExtValid}},  \code{\link{groupVec}}  function, \code{\link{groupVecColumn}},   \code{\link{groupVecData}},  \code{\link{groupVecNames}},  \code{\link{groupVecClasses}}.   
}
\keyword{classes}
