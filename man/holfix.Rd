\name{holiday.fixed}
\alias{holiday.fixed}
\alias{holiday.weekday.number}
\title{
  Holiday Generating Functions 
}
\description{
Generates holidays occuring on fixed dates (\code{holiday.fixed})  
or on given weekdays of 
given months (\code{holiday.weekday.number}), for example, the 1st Monday in September. 
}
\usage{
holiday.fixed(years, month, day) 
holiday.weekday.number(years, month, weekday, index)
}
\arguments{
\item{years}{
the desired year(s) for which to generate holidays (for example, 1997:2000). 
}
\item{month}{
the month of the holiday (1-12). 
}
\item{day}{
the day of the month of the holiday (1-31). 
}
\item{weekday}{
the weekday of the holiday (0-6, 0 is Sunday). 
}
\item{index}{
the occurrence of \code{weekday} in the month (1-5, or -1 for the last) 
for the holiday. 
}
}
\value{
returns a time/date object containing the specified holiday in the  
specified years. 
}
\details{
\itemize{
\item The \code{holiday.fixed} function generates holidays, like Christmas, that 
occur on a specified date every year.  
\item The \code{holiday.weekday.number} 
function generates holidays that occur on an indexed weekday of  
a given month every year, such as Labor day in the U.S., which  
is the first Monday of September. Years in which the given dates do 
not exist (for example, the 5th Friday in various months) are excluded from 
the output (as opposed to generating \code{NA}). 
}
}
\seealso{
\code{\link{holiday.AllSaints}},  \code{\link{holidays}},  \code{\link{holiday.nearest.weekday}}   
}
\examples{
# Generate Christmas 
holiday.fixed(1994:2005, 12, 25) 
# Generate Memorial Day (last Monday in May) 
holiday.weekday.number(1994:2005, 5, 1, -1) 
# Generate Thanksgiving (4th Thursday in November) 
holiday.weekday.number(1994:2005, 11, 4, 4) 
}
\keyword{chron}

