\name{timeAlign}
\alias{timeAlign}
\title{
  Alignment of Times 
}
\description{
Aligns a time vector to a time unit specified as in \code{timeSeq}. 
}
\usage{
timeAlign(x, by="days", k.by=1, direction=1, week.align=NULL,  
          holidays=timeDate())
}
\arguments{
\item{x}{
a time/date object. 
}
\item{by}{
one of the following character strings, giving the units to align to: 
\itemize{
\item \code{"milliseconds"}
\item \code{"seconds"}
\item \code{"minutes"}
\item \code{"hours"}
\item \code{"days"}, 
\item \code{"weekdays"}
\item \code{"bizdays"}
\item \code{"weeks"}
\item \code{"months"}
\item \code{"quarters"}  
\item \code{"years"}
}
}
\item{k.by}{
a non-zero integer giving the number of the \code{by} units to align to. 
Ignored for \code{"weekdays"}, \code{"bizdays"}, and \code{"weeks"}. 
}
\item{direction}{
either 1 or -1, to align to the next or previous time that is an 
integer number of the \code{k.by * by} units.  
}
\item{week.align}{
if not \code{NULL}, and \code{by} is \code{"weeks"}, you can supply a character 
string (or an integer, 0 to 6 with 0 being Sunday)  
to specify a weekday to align to. The character string must 
be sufficient to make a unique case-insensitive match to the strings 
in \code{timeDateOptions("time.day.name")}. 
}
\item{holidays}{
dates of holidays for business day alignment. 
}
}
\value{
returns a time object whose elements are  
moved up or down (according to \code{direction}), so that  
they lie on integer multiples of \code{k.by * by} units of time, 
starting from the beginning of the next larger time unit  
(for example, if \code{by="days"}, 
then align to multiples of \code{k.by} days added to the first of the 
month. If \code{by="hours"}, align to multiples of \code{k.by} hours since 
midnight).
 
\strong{Notes} 
\itemize{
\item for \code{"weeks"}, \code{"weekdays"}, and \code{"bizdays"},  
\code{k.by} is assumed to be 1 and ignored.
\item \code{"weeks"} without \code{week.align} is equivalent to \code{"days"}. 
\item \code{k.by} should be a divisor of the number of 
\code{by} units in the next larger time unit, or \code{NA} values 
result. 
}
}
\seealso{
\code{\link{timeSeq}}. 
}
\examples{
x <- timeDate(c("2/11/1992 22:34", "7/8/1995 08:32"),  
       format="\%a \%02m/\%02d/\%Y \%02H:\%02M") 
# move to beginning of month 
timeAlign(x,"months",direction=-1) 
# move to beginning of next month 
timeAlign(x,"months",direction=1) 
# move to next multiple of 3 hours 
timeAlign(x,"hours",3) 
# move to next Friday 
timeAlign(x,"weeks", week.align="Friday") 
}
\keyword{chron}
