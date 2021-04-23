\name{timeRelative-class}
\alias{timeRelative-class}
\alias{*,numeric,timeRelative-method}
\alias{*,timeRelative,numeric-method}
\alias{+,positionsCalendar,timeRelative-method}
\alias{+,timeRelative,timeRelative-method}
\alias{+,timeRelative,positionsCalendar-method}
\alias{-,positionsCalendar,timeRelative-method}
\alias{-,timeRelative,timeRelative-method}
\alias{-,timeRelative,missing-method}
\alias{Math2,timeRelative-method}
\alias{Math,timeRelative-method}
\alias{Ops,ANY,timeRelative-method}
\alias{Ops,timeRelative,ANY-method}
\alias{Summary,timeRelative-method}
\alias{[,timeRelative-method}
\alias{[<-,timeRelative,ANY,ANY,ANY-method}
\alias{[[,timeRelative-method}
\alias{[[<-,timeRelative-method}
\alias{c,timeRelative-method}
\alias{coerce,character,timeRelative-method}
\alias{coerce,timeRelative,character-method}
\alias{duplicated,timeRelative-method}
\alias{is.na,timeRelative-method}
\alias{is.nan,timeRelative-method}
\alias{length,timeRelative-method}
\alias{length<-,timeRelative-method}
\alias{logb,timeRelative-method}
\alias{match,ANY,timeRelative-method}
\alias{match,timeRelative,ANY-method}
\alias{show,timeRelative-method}
\alias{summary,timeRelative-method}
\alias{which.na,timeRelative-method}
\title{
  Relative Time Class
}
\docType{class}
\description{
The \code{timeRelative} class represents relative times.
}
\section{Slots}{
  \describe{
    \item{Data}{
      (\code{character}) a string vector representing the relative time.
    }
    \item{holidays}{
      (\code{positionsCalendar}) a vector of holiday dates.
    }
  }
}
\details{
The \code{timeRelative} class stores a representation of relative
times. Unlike \code{timeSpan}, which stores absolute
time differences, the \code{timeRelative} class stores relative times
in units such as weekdays, months, and business days, whose
absolute time value depends on the \code{timeDate} object with which they are
combined.

Both \code{timeRelative} and \code{timeSpan} extend the
virtual \code{timeInterval} class.

The \code{Data} slot in a \code{timeRelative} object holds a character
vector that represents the relative time. Each element of the
vector is a character string consisting of whitespace-separated
fields in the following form:

\tabular{lll}{
\tab \tab \code{"[+-][a]#abb"}
}

This vector is composed of a required sign (either \code{"+"} or \code{"-"}),
followed by an optional \code{"a"} that, if present, means to align the result
(see below; it is also possible to specify \code{0} if aligning),
followed by a positive integer and one of the relative time field abbreviations
from the following list:
\describe{
\item{ms}{
add/subtract milliseconds. \code{"a"} aligns to the nearest # milliseconds within
the second, where # must be a divisor of 1000 and less than 1000
(for example, 500 aligns to even seconds or 1/2 seconds). 0 is not allowed.
}
\item{sec}{
add/subtract seconds. \code{"a"} aligns to nearest # seconds within
the minute, where # must be a divisor of 60 and less than 60
(for example, 15 aligns to 0, 15, 30, or 45 seconds past the minute).
 0 goes to the beginning of the current second, independent of sign.
}
\item{min}{
add/subtract minutes. \code{"a"} aligns to nearest # minutes within
the hour, where # must be a divisor of 60 and less than 60
(for example, 15 aligns to 0, 15, 30, or 45 minutes after the hour).
0 goes to the beginning of the current minute, independent of sign.
}
\item{hr}{
add/subtract hours. \code{"a"} aligns to nearest # hours within
the day, where # must be a divisor of 24 and less than 24
(for example, 6 aligns to midnight, 6AM, noon, or 6PM).
0 goes to the beginning of the current hour, independent of sign.
}
\item{day}{
add/subtract days. \code{"a"} aligns to nearest # days within
the month, starting with the first, where # must be a less than the
number of days in the month (for example, 2 aligns to the 1st, 3rd, 5th,
and so on, with the time midnight). 0 goes to the beginning of the
current day, independent of sign.
}
\item{wkd}{
add/subtract weekdays. \code{"a"} causes the first added or subtracted
weekday possibly to be a fraction of a day to move to the next
or previous midnight on a weekday morning, and then whole
additional days are added or subtracted to make up # weekdays.
0 goes to the beginning of the day, or the closest weekday before
if it is not a weekday, independent of sign.
}
\item{biz}{
add/subtract business days (weekdays that are not holidays).
\code{"a"} causes the first added or subtracted
business day possibly to be a fraction of a day to move the next
or previous midnight on a business day morning, and then whole
additional days are added or subtracted to make up # business days.
0 goes to the beginning of the day, or the closest business day before
if it is not a business day, independent of sign.
}
\item{sun}{
add/subtract Sundays. \code{"a"} causes the first added or subtracted
Sunday possibly to be a fraction of a day or week
to move the next or previous
midnight on a Sunday morning, and then whole additional
weeks are added or subtracted to make up # Sundays.
0 goes to the beginning of the day, or the closest Sunday
before if it is not the right day, independent of sign.
}
\item{mon}{
add/subtract Mondays. \code{"a"} causes the first added or subtracted
Monday possibly to be a fraction of a day or week
to move the next or previous
midnight on a Monday morning, and then whole additional
weeks are added or subtracted to make up # Mondays.
0 goes to the beginning of the day, or the closest Monday
before if it is not the right day, independent of sign.
}
\item{tue}{
add/subtract Tuesdays. \code{"a"} causes the first added or subtracted
Tuesday possibly to be a fraction of a day or week
to move the next or previous
midnight on a Tuesday morning, and then whole additional
weeks are added or subtracted to make up # Tuesdays.
0 goes to the beginning of the day, or the closest Tuesday
before if it is not the right day, independent of sign.
}
\item{wed}{
add/subtract Wednesdays. \code{"a"} causes the first added or subtracted
Wednesday possibly to be a fraction of a day or week
to move the next or previous
midnight on a Wednesday morning, and then whole additional
weeks are added or subtracted to make up # Wednesdays.
0 goes to the beginning of the day, or the closest Wednesday
before if it is not the right day, independent of sign.
}
\item{thu}{
add/subtract Thursdays. \code{"a"} causes the first added or subtracted
Thursday possibly to be a fraction of a day or week
to move the next or previous
midnight on a Thursday morning, and then whole
additional weeks are added or subtracted to make up # Thursdays.
0 goes to the beginning of the day, or the closest Thursday
before if it is not the right day, independent of sign.
}
\item{fri}{
add/subtract Fridays. \code{"a"} causes the first added or subtracted
Friday possibly to be a fraction of a day or week
to move the next or previous
midnight on a Friday morning, and then whole
additional weeks are added or subtracted to make up # Fridays.
0 goes to the beginning of the day, or the closest Friday
before if it is not the right day, independent of sign.
}
\item{sat}{
add/subtract Saturdays. \code{"a"} causes the first added or subtracted
Saturday possibly to be a fraction of a day or week
to move the next or previous
midnight on a Saturday morning, and then whole
additional weeks are added or subtracted to make up # Saturdays.
0 goes to the beginning of the day, or the closest Saturday
before if it is not the right day, independent of sign.
}
\item{wk}{
add/subtract weeks. \code{"a"} is not allowed.
}
\item{tdy}{
add/subtract "ten-day" periods of months (ten-day periods
begin on the first, 11th, and 21st of the month but not the 31st).
Without  \code{"a"}, the day number of the result is 1, 11, or 21, adding
# partial or entire ten-day periods to get there.
If \code{"a"} is used, # must be either 1, 2, or 3, and the time will be midnight.
0 goes to the beginning of the current ten-day period, independent of sign.
}
\item{mth}{
add/subtract months. \code{"a"} aligns to nearest # months within
the year, starting with January, and # must be a divisor of 12 and
less than 12. (For example, 3 aligns to Jan 1, Apr 1, Jul 1, Oct 1 at midnight.)
0 goes to the beginning of the current month, independent of sign.
}
\item{qtr}{
add/subtract quarters. \code{"a"} aligns to nearest # quarters within
the year, and # must be either 1 or 2.
(For example, 2 aligns to Jan 1 or Jul 1 at midnight.)
0 goes to the beginning of the current quarter, independent of sign.
}
\item{yr}{
add/subtract years. \code{"a"} aligns to nearest # years.
(For example, 5 aligns to Jan 1 at midnight in 1995, 2000, 2005, and so on.)
0 goes to the beginning of the current year, independent of sign.
}
}
When relative time objects are added to time/date objects,
the fields from an element of the relative time object are parsed
and added to the corresponding element of the  time/date object
from left to right. (If either the time/date object or the relative time object
is shorter than the other, it is reused cyclically in the standard S manner.)

For example, for a date/time of May 13, 2012 4:32 PM (a Sunday),
the relative time element is: \code{"+a3hr +12hr -1day"}.

\itemize{
\item The first field of the relative time specifies adding up to three hours, and
aligning to the nearest three-hour boundary. This operation advances the time to 6:00 PM.
\item The second field specifies adding twelve hours, which advances the time
to 6:00 AM on May 14.
\item The third field specifies subtracting a whole day, which leaves us at 6:00 AM on May 13.
}

You can add relative time objects to time/date objects, or you can
subtract relative time objects from time/date objects. Also, you can add
them to each other, subtract them from each other, or multiplt them by integers.
When they are negated, the sign of each field is reversed.
When they are added together, they are concatenated,
so that if \code{x} is a time/date object
and \code{y} and \code{z} are relative time objects,
\code{(x + y) + z == x + (y + z)};
however, \code{y + z} is not the same as \code{z + y}.
}
\section{Notes}{
\enumerate{
\item All alignment and other operations are done in the local time
zone of the time/date object.
\item The \code{holidays} slot of the relative time object is used
to define which dates besides weekends are not business days;
these dates are taken in the \code{holidays} slots time zone.
\item When adding units of time without the \code{"a"} flag in the field,
the smaller units of time are not changed; for example, when adding days,
the time of day stays the same, and when adding months,
the day of the month and the time of day stay the same.
}
}
\seealso{
\code{\linkS4class{timeDate}} class, \code{\linkS4class{timeSpan}} class,
\code{\link{timeRelative}} function.
}
\keyword{chron}
\keyword{classes}
