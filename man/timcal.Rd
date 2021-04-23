\name{timeCalendar}
\alias{timeCalendar}
\title{
  Constructor Function for \code{timeDate} Objects 
}
\description{
Constructs a time object corresponding to calendar dates 
and/or times of day. 
}
\usage{
timeCalendar( m=NULL, d=NULL, y=NULL, h=NULL, min=NULL,
			   s=NULL, ms=NULL, format, zone )
}
\arguments{
\item{m}{
calendar months (1-12). The default is  1. 
}
\item{d}{
calendar days (1-31). The default is 1. 
}
\item{y}{
calendar years (e.g. 1997). The default is 1960. 
}
\item{h}{
hours of the days (0-23). The default is 0. 
}
\item{min}{
minutes of the days (0-59). The default is 0. 
}
\item{s}{
seconds of the days (0-59). The default is 0. 
}
\item{ms}{
milliseconds of the days (0-999). The default is 0. 
}
\item{format}{
the output format string to apply to the returned object. The default is  
from \code{timeDateOptions("time.out.format")}. 
}
\item{zone}{
the time zone of the input date and time. Also stored in the result.  
The default is from \code{timeDateOptions("time.zone")}. 
}
}
\value{
returns a \code{timeDate} object corresponding to the input. 
}
\details{
If none of \code{m}, \code{d}, \code{y}, \code{h}, \code{min}, \code{s}, and \code{ms} are supplied, 
this function returns a time vector of length zero.  If more than 
one is supplied, they must all have compatible lengths. Shorter 
inputs are used cyclically to comprise the maximum length, but 
they must be even multiples.  

The arguments represent the date and time of day  
in the given time zone. Missing arguments are supplied to give a   
time of midnight, January 1, 1960. 

Leap seconds do not cause \code{NA} values, but can cause times of day to 
be off by a second in the days containing them. 
}
\seealso{
  \code{\link{timeDate}},  \code{\linkS4class{timeDate}}  class,
  \code{\link{mdy}}, \code{\link{format.timeDate}}.   
}
\examples{
timeCalendar(m = c(3, 4, 5), d = c(12, 15, 7), y = c(1998, 1997, 2004), 
     format = "\%b. \%d, \%Y") 
timeCalendar(h = c(9, 14), min = c(15, 23), format = "\%I:\%02M \%p") 
}
\keyword{chron}
