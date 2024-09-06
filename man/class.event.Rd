\name{timeEvent-class}
\alias{timeEvent-class}
\alias{==,timeEvent,timeEvent-method}
\alias{Math2,timeEvent-method}
\alias{Math,timeEvent-method}
\alias{Ops,ANY,timeEvent-method}
\alias{Ops,timeEvent,ANY-method}
\alias{Summary,timeEvent-method}
\alias{[<-,timeEvent,ANY,ANY,timeEvent-method}
\alias{coerce,positionsCalendar,timeEvent-method}
\alias{show,timeEvent-method}
\alias{summary,timeEvent-method}
%
\title{
  Event Class 
}
\docType{class}
\description{
The \code{timeEvent} class represents events that occur at specific  
calendar times.  It is useful for one-time events (for example, the Gulf 
War), recurring events (for example, holidays or market opening and closing times), 
and multiple related events (for example, the numbered Olympic games,  
OPEC meetings, or hurricanes). 
}
\section{Objects from the Class}{
  Create objects using calls of the form \code{new("timeEvent",
    ...)} or \code{timeEvent}.
}

\section{Slots}{
\describe{
  \item{columns}{
    (\code{list}) (from \code{groupVec}). Always should be a list with
    three elements.
  }
  \item{names}{
    (\code{character}) (from \code{groupVec}). Always  
    \code{c("start", "end", "IDs")}. 
  }
  \item{classes}{
    (\code{character}) (from \code{groupVec}). Always 
    \code{c("positionsCalendar", "positionsCalendar", "ANY")}. 
  }
  }
}
\details{
The \code{timeEvent} class is set up to hold vectors of starting and ending 
times of events, as well as an identifier for each event, which can be 
stored in any vector object. These three vectors are stored as 
columns of a \code{groupVec}.  The \code{timeEvent} class extends the \code{groupVec} class.

Create objects of class \code{timeEvent}  
either by using the \code{timeEvent} function, or by 
coercing any \code{positionsCalendar} object to \code{timeEvent} using \code{as}. 
}
\seealso{
\code{\linkS4class{groupVec}}  class,
\code{\linkS4class{positionsCalendar}}  
class, \code{\link{timeEvent}}  function.  
}
\keyword{classes}
\keyword{chron}
