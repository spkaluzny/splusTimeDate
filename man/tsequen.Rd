\name{timeSequence}
\alias{timeSequence}
\title{
Create a Time Sequence Object 
}
\description{
Constructs a \code{timeSequence} object.  
}
\usage{
timeSequence(from, to, by, length.out, k.by=1, align.by=FALSE,  
             extend=FALSE, week.align=NULL, holidays=timeDate(), exceptions, 
             additions, format, zone )
}
\arguments{
If any arguments are supplied, exactly three of \code{from}, \code{to}, 
\code{by}, and \code{length.out} must be supplied.  
\item{from}{
the starting value of the sequence: a \code{timeDate} object (or number or 
character string representing one). 
}
\item{to}{
the ending value of the sequence: a \code{timeDate} object (or number or 
character string representing one). 
}
\item{by}{
the spacing between successive values in the sequence.  This can be 
a \code{timeSpan}, \code{timeRelative}, or \code{numeric} value,  
in which case \code{k.by} is ignored. 
Alternatively, it can be one  
of the following character strings giving the 
time units of intervals between values in the sequence: 
\preformatted{
   "milliseconds"
   "seconds"
   "minutes"
   "hours"
   "days"
   "weekdays"
   "bizdays"
   "weeks"
   "months"
   "quarters"
   "years"
}  
}
\item{length.out}{
the length of the sequence before additions and exceptions. 
}
\item{k.by}{
a non-zero integer giving the width of the interval between consecutive 
values in the sequence in terms of the units given in \code{by}.  Ignored if 
\code{by} is not a character string. 
}
\item{align.by}{
a logical value. If \code{TRUE}, adjusts the sequence so that each element is on a whole number 
of the \code{by * k.by} units. For example, if the units are 2 months, the sequence is 
on only the first of January, March, May, and so on.
Ignored if \code{by} is not a character string. 
}
\item{extend}{
a logical value. If \code{TRUE} and \code{align.by} is also \code{TRUE},
instead of making the entire sequence lie between \code{from} and \code{to},
make it extend just past \code{from} and \code{to}  
to the next aligned values.
For example, if \code{from} is January 15th and the sequence is by 1 month units,
and if \code{extend} is \code{FALSE}, the sequence starts on February 1st
If \code{extend} is \code{TRUE}, the sequence starts on January 1st.
Ignored if \code{by} is not a character string. 
}
\item{week.align}{
if \code{by} is \code{"weeks"},  
you can supply a character 
string (or a number 0 to 6, with 0 being Sunday)  
to specify a weekday to use.  The character string must 
be sufficient to make a unique case-insensitive match to the strings 
in \code{timeDateOptions("time.day.name")}.  
The sequence is adjusted so all its elements fall on the
given weekday. If \code{align.by} 
is \code{TRUE}, then it is also adjusted
to start at midnight.

In either case, the \code{extend} argument is used to decide which
direction to adjust the day. This argument is ignored if 
\code{by} is not a character string, or if it
is not \code{"weeks"}. 
}
\item{holidays}{
the holidays for business day sequences (ignored if \code{by} is not 
a character string).
}
\item{exceptions}{
an event object giving time periods when sequence should not have any values.
These are applied after the sequence is created from from/to/by/length.out. 
}
\item{additions}{
additional times/dates to put in the sequence. 
}
\item{format}{
the time/date output format for printing. 
}
\item{zone}{
the time zone for the sequence. 
}
}
\value{
returns a time sequence object as defined by the arguments.
Note that the \code{timeSeq} function is similar,
except that it returns a time/date vector.
This function returns a compact time sequence 
object that retains information about the sequence. 
}
\seealso{
\code{\link{timeSeq}},  \code{\linkS4class{timeSequence}},
\code{\link{holidays}},
\code{\link{timeEvent}}
}
\examples{
timeSequence("1/1/1992", "12/1/1992", by = "months") 
timeSequence("1/3/1992", "12/5/1992", by = "months", align.by=TRUE) 
timeSequence("1/1/1992", "1/31/1992", by = "weeks", align.by=TRUE,  
             week.align="Mon")
timeSequence("1/1/1992", "12/31/1992", by="weekdays", exceptions=holidays(1992))
timeSequence("1/1/1992", "1/1/1995", by="months", exceptions=timeEvent("1/1/1993", "12/31/1993"))
}
\keyword{chron}
