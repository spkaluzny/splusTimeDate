\name{timeRelative}
\alias{timeRelative}
\title{
  Constructor Function for timeRelative Class
}
\description{
Construct a \code{timeRelative} object.
}
\usage{
timeRelative(x, holidays., by, k.by=1, align.by=FALSE, week.day=NULL)
}
\arguments{
\item{x}{
a character string vector representing relative times.
}
\item{holidays.}{
a time/date or time sequence object giving holiday dates.
}
\item{by}{
as an alternate to providing a character string vector,
you can provide \code{by},
\code{k.by}, \code{align.by},
and \code{week.day} if you need to construct
a \code{timeRelative} object.

\code{by} is one of the following character strings,
giving the time units for the relative time:
\preformatted{
   "milliseconds"
   "ms"
   "seconds" or "sec"
   "minutes" or "min"
   "hours" or "hr"
   "days" or "day"
   "weekdays" or "wkd"
   "bizdays" or "biz"
   "weeks" or "wk"
   "tdy" (for 10-day periods in a month)
   "months" or "mth"
   "quarters" or "qtr"
   "years" or "yr"
   }

To add or subtract specific days of the week, use 
\preformatted{
   "sun"
   "mon"
   "tue"
   "wed"
   "thu"
   "fri"
   "sat"
   }

See \code{timeRelative-class} for more information on these arguments.
}
\item{k.by}{
a non-zero integer specifying the number of \code{by} time units in the relative time.
}
\item{align.by}{
a logical value. If \code{TRUE}, specifies using alignment (see documentation on the
relative time class).
}
\item{week.day}{
if not \code{NULL}, and \code{by} is \code{"weeks"}, you can supply a character
string (or an integer 0 to 6, with 0 being Sunday) to specify a weekday  
for the relative time. The character string must
be sufficient to make a unique case-insensitive match to the strings
in \code{timeDateOptions("time.day.name")}.
}
}
\value{
returns a \code{timeRelative} object with the given strings
as data, and holidays, if given.
Otherwise, the strings default to empty, and the holidays to no holidays.
}
\seealso{
\code{\linkS4class{timeRelative}} 
}
\examples{
# Create a relative time object that you could add to a time/date object
# to take each element to the third Friday of the month
rtobj <- timeRelative("-a0mth -1fri +3fri")
timeDate(c("1/5/1998", "2/26/1998"), format = "\%a \%m/\%d/\%Y") + rtobj
# Create a relative time object for 3 minutes
timeRelative(by="minutes", k.by=3)
}
\keyword{chron}
