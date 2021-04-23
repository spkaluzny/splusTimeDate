\name{timeDefaults}
\alias{timeDefaults}
\title{
  Time Class Internal Functions 
}
\description{
Creates a list of defaults for reading/printing times and dates. 
}
\usage{
timeDefaults()
}
\value{
returns a list containing the following components: 

\item{month.name}{
the names of the calendar months. 
}
\item{month.abb }{
the abbreviations for the calendar months. 
}
\item{day.name}{
the names of the weekdays. 
}
\item{day.abb}{
the abbreviations for the weekdays. 
}
\item{am.pm}{
strings for printing/reading AM and PM. 
}
\item{century}{
the first year of 100-year stretch to use for the current century  
in reading/printing dates, e.g. 1900. 
}
\item{zone}{
the time zone. 
}
}
\details{
The list components are read from their corresponding options,  
which are the component names with \code{"time."} prepended. For example, the  
weekday names come from \code{timeDateOptions("time.day.name")}. 
}
\seealso{
\code{\link{timeDateOptions}},  \code{\linkS4class{timeDate}}  class.  
}
\examples{
timeDefaults() 
}
\keyword{internal}
