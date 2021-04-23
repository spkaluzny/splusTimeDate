\name{hours}
\alias{hours}
\alias{minutes}
\alias{seconds}
\alias{yeardays}
\title{
  Return Various Periods from a Time Vector 
}
\description{
Extracts as numbers various time-of-day periods from a time vector. 
}
\usage{
hours(x) 
minutes(x) 
seconds(x) 
yeardays(x)
}
\arguments{
\item{x}{
the \code{timeDate} object from which to extract periods. 
}
}
\value{
returns a numeric vector of hours, minutes, seconds, or year-days 
of \code{x} for the respective function.  
\itemize{
\item Hours are integers between 0 and 23.
\item Minutes are integers between 0 and 59.
\item Seconds are numbers including the fractions of a second.  
\item Yeardays are the day number of the year (a number between 1 and 366). 
}
}
\seealso{
\code{\link{days}},  \code{\link{hms}}.   
}
\examples{
x <- timeDate(c("1/1/1998 3:05:23.4", "5/10/2005 2:15:11.234 PM")) 
hours(x) 
minutes(x) 
seconds(x) 
yeardays(x) 
}
\keyword{chron}

