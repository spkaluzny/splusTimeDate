\name{timeZoneC}
\alias{timeZoneC}
\title{
  Constructor Function for \code{timeZoneC} Class 
}
\description{
Constructs a \code{timeZoneC} object. 
}
\usage{
timeZoneC(name)
}
\arguments{

\item{name}{
the official name of a built-in time zone object.  
Should not be a vector of names. 
The default is \code{"utc"}. 
}
}
\value{
returns a \code{timeZoneC} object with the given name, or the default 
zone name if none is given. 
}
\details{
The \code{timeZoneC} class holds a reference to one of the 
built-in C time zone objects.  See the documentation on 
the \code{timeZoneC} class for more information.  
}
\seealso{
\code{\link{timeZoneList}},  \code{\link{timeZoneR}},  \code{\linkS4class{timeZone}}  class.  
}
\examples{
timeZoneC() 
timeZoneC("us/pacific") 
}
\keyword{chron}
