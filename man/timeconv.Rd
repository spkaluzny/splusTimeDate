\name{timeConvert}
\alias{timeConvert}
\title{
  Convert from one time zone to another.
}
\description{
Converts a time/date object from one time zone to another.
}
\usage{
timeConvert(x, to.zone, from.zone)
}
\arguments{
\item{x}{
the time/date object to convert.
}
\item{to.zone }{
the time zone to convert to (character).
}
\item{from.zone }{
the time zone to convert from (character). For \code{timeDate} objects, this is
ignored. Instead, the value is taken from the \code{time.zone} slot of the \code{timeDate} object.
}
}
\value{
returns the time/date object reflecting the converted time zone.
}
\details{
The \code{timeDate} stores times/dates as their equivalent time in GMT, 
with a time zone (used for printing and other operations) stored in the
\code{time.zone} slot. Therefore, to convert time zones, this function simply
puts the new time zone into the \code{time.zone} slot.
}
\seealso{
\code{\link{timeZoneConvert}}.
}
\examples{
timeDateOptions(time.zone="GMT",
  time.in.format="\%m/\%d/\%Y [\%H:\%M]",
  time.out.format="\%m/\%d/\%Y \%02H:\%02M (\%Z)")
date1 <- timeDate("3/22/2002 12:00", zone="PST")
date1 
## 3/22/2002 12:00 (PST)
date2 <- timeConvert(date1, "EST")
date2 # converted to EST
## 3/22/2002 15:00 (EST)
}
\keyword{chron}
