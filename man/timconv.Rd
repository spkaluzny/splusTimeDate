\name{timeZoneConvert}
\alias{timeZoneConvert}
\title{
  Convert Time Zones 
}
\description{
Convert a \code{timeDate} object to a new time zone. 
}
\usage{
timeZoneConvert(x, zone)
}
\arguments{
\item{x}{
the time vector object to convert. 
}
\item{zone}{
the time zone to convert to. 
}
}
\value{
returns the converted \code{timeDate} object. 
}
\details{
Internally, all \code{timeDate} objects are
stored as times in GMT with an associated
\code{time.zone} slot.  Conversion to the
"local" time zone is done only for the purpose of displaying the
\code{timeDate} object.

Modifying the \code{time.zone} slot directly
alters the time zone of the object, but not the time itself. (It
represents the same instant in a different part of the world.) When
displayed, the time component differs according to the time
difference betwwen the time zones before and after the change.

The function \code{timeZoneConvert} modifies the
time zone \emph{and} the actual time by the time difference between
the new and old time zones. As a result, the printed display of the
\code{timeDate} object remains the same (other
than any displayed time zone information).  This is useful when
reading in data from a file without specific time zone information
(which, by default, is created with a GMT time zone), and then
converting it to a different local time zone without changing the
printed appearance of the dates and times.
}
\seealso{
\code{\link{timeConvert}},
\code{\link{timeDate}},
\code{\link{timeCalendar}},
\code{\link{holidays}},
\code{\link{timeZoneList}},
\code{\linkS4class{timeDate}}.
}
\examples{
timeDateOptions(time.zone="GMT",
  time.in.format="\%m/\%d/\%Y [\%H:\%M]",
  time.out.format="\%m/\%d/\%Y \%02H:\%02M (\%Z)")
date1 <- timeDate("3/22/2002 12:00", zone="GMT")
date1 
# 3/22/2002 12:00 (GMT)
date2 <- timeZoneConvert(date1, "PST")
date2 # appears the same as date1, except for zone
# 3/22/2002 12:00 (PST)
date1 - date2 # these times are 8 hours apart

# modifying the time.zone slot does not change
# the actual time, just the display
date3 <- date2
date3@time.zone <- "EST"
date3 # displays as 3 hours later
# 3/22/2002 15:00 (EST)
date2-date3 # but the difference is zero
# 0d 0h 0m 0s 0MS
}
\keyword{chron}
