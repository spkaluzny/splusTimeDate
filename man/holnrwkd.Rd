\name{holiday.nearest.weekday}
\alias{holiday.nearest.weekday}
\title{
  Holiday Generating Functions 
}
\description{
Moves calculated holiday dates (or any dates) to the nearest weekday, 
if they are on weekends. 
}
\usage{
holiday.nearest.weekday(dates.)
}
\arguments{
\item{dates.}{
the dates to move to the nearest weekday. Must be a \code{timeDate} object. 
}
}
\value{
returns a vector of dates that is the same as the input 
vector wherever its dates lie on weekdays, and which has 
Sundays moved to Monday, and Saturdays moved to Friday for 
weekend dates. 
}
\seealso{
\code{\link{holiday.fixed}},  \code{\link{holidays}}.   
}
\examples{
holiday.nearest.weekday(holiday.Christmas(1994:2005)) 
}
\keyword{chron}

