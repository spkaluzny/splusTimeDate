\name{timeEvent}
\alias{timeEvent}
\title{
  Constructor Function For \code{timeEvent} Objects 
}
\description{
Constructs a \code{timeEvent} object. 
}
\usage{
timeEvent(start., end., IDs)
}
\arguments{
  \item{start.}{
    a time/date object giving start times of events. 
    The function can be called with no arguments, but if any are  
    supplied, \code{start.} is required.
  }
  \item{end.}{
    a time/date object giving end times of events.  If missing, defaults to  
    one day after \code{start.}, minus 1 millisecond. 
  }
  \item{IDs}{
    the  names or numbers identifying individual events in the object.   
    If missing, defaults to empty strings.  
  }
}
\value{
returns a \code{timeEvent} object derived from the inputs. 
}
\details{
The \code{start.}, \code{end.} and \code{IDs} are put into the corresponding 
columns of a new \code{timeEvent} object.  
}
\seealso{
\code{\linkS4class{timeEvent}}
}
\examples{
timeEvent() 
timeEvent(holiday.Christmas( 1990:2010 ), ID = 1990:2010) 
}
\keyword{chron}
