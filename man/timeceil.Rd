\name{timeCeiling}
\alias{timeCeiling}
\alias{timeFloor}
\alias{timeTrunc}
\title{
Rounding Functions for timeDate Objects
}
\description{
Rounds a time to the nearest day.
}
\usage{
timeCeiling(x)
timeFloor(x)
timeTrunc(x)
}
\arguments{
  \item{x}{
    an object of class \code{positionsCalendar}
  }
}
\value{
  returns a \code{positionsCalendar} object rounded to current or next day.
  \item{\code{timeFloor} and \code{timeTrunc}}{round a \code{positionsCalendar}
  object to the beginning of the day.}
 \item{\code{timeCeiling}}{rounds a time to the beginning of the next day.}
}
\seealso{
\code{\link{ceiling}}, \code{\link{floor}}, \code{\link{trunc}}, \code{\link{positionsCalendar-class}}
}
\examples{
x <- timeDate(date(), in.format="\%w \%m \%d \%H:\%M:\%S \%Y")
timeCeiling(x)
timeFloor(x)
}
\keyword{ arith }
