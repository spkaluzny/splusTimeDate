\name{timeZoneR}
\alias{timeZoneR}
\title{
  Constructor Function for \code{timeZoneR} Class 
}
\description{
Construct a \code{timeZoneR} object. 
}
\usage{
timeZoneR(offset=0, yearfrom=integer(0), yearto=integer(0),  
          hasdaylight=logical(0), dsextra=integer(0), 
          monthstart=integer(0), codestart=integer(0), 
          daystart=integer(0), xdaystart=integer(0), 
          timestart=integer(0), monthend=integer(0),  
          codeend=integer(0), dayend=integer(0),  
          xdayend=integer(0), timeend=integer(0), rules)
}
\arguments{
\item{}{\strong{Note}: The arguments other than \code{offset} and \code{rules} define the components 
of the rules for when to go on daylight savings.  Each is a  
vector with one component for each rule. 
}
\item{offset}{
the offset from GMT (in seconds) when not on daylight savings time. 
}
\item{yearfrom}{
the starting years for rules, or -1 to start at the beginning of time. 
}
\item{yearto}{
the ending years for rules, or -1 to end at the end of time. 
}
\item{hasdaylight}{
specifies whether daylight savings time is used in each year range. 
}
\item{dsextra}{
the offsets (in seconds) to add to the regular offset when daylight time is  
in effect. 
}
\item{monthstart}{
the months (1-12) in which daylight savings time starts (can be after  
\code{monthend} for the southern hemisphere). 
}
\item{codestart}{
the codes telling how to interpret \code{daystart} and \code{xdaystart} to  
calculate the days within the month for starting daylight 
savings time.  
\tabular{ll}{
1: \tab start on the \code{daystart} (1-31) day of the month. \cr
2: \tab start on the last \code{daystart} (0-Sunday through 6-Saturday) weekday  
in the month.   \cr
3: \tab start on the first \code{daystart} weekday (0-Sunday through 6-Saturday)  
on or after the \code{xdaystart} (1-31) day of the month. \cr
4: \tab start on the last \code{daystart} weekday (0-Sunday through 6-Saturday)  
on or before the \code{xdaystart} (1-31) day of the month. 
}
}
\item{daystart}{
see \code{codestart}. 
}
\item{xdaystart}{
see \code{codestart}. 
}
\item{timestart}{
the seconds after midnight local standard time to start daylight savings time,
on the day specified by \code{codestart} and other arguments.
}
\item{monthend}{
the months (1-12) in which daylight savings time ends (can be before 
\code{monthstart} for the southern hemisphere). 
}
\item{codeend}{
the codes specifying interpreting \code{dayend} and \code{xdayend} to  
calculate the day within the month for ending daylight 
savings time.  
\tabular{ll}{
1: \tab end on the \code{dayend} (1-31) day of the month. \cr
2: \tab end on the last \code{dayend} (0-Sunday through 6-Saturday) weekday  
in the month.   \cr
3: \tab  end on the first \code{dayend} weekday (0-Sunday through 6-Saturday)  
on or after the \code{xdayend} (1-31) day of the month. \cr
4: \tab  end on the last \code{dayend} weekday (0-Sunday through 6-Saturday)  
on or before the \code{xdayend} (1-31) day of the month. \cr
}
}
\item{dayend}{
see \code{codeend}. 
}
\item{xdayend}{
see \code{codeend}. 
}
\item{timeend}{
the seconds after midnight local standard time to end daylight savings time,
on the day specified by \code{codeend} and other arguments.
}
\item{rules}{
a data frame of rules encoding when to go on daylight savings time.  
(Overrides all other arguments except \code{offset}, if it is not missing,  
and must contain columns corresponding to those arguments in that 
order, or be completely empty.  Note that the columns should all be 
integer. See \code{timeZoneR} class documentation for more information.) 
}
}
\value{
returns a \code{timeZoneR} object with the given name, or the default time zone 
if no arguments are supplied. 
}
\details{
The \code{timeZoneR} class provides to users a way to define time zones.  
To use the defined time zones in \code{timeDate} objects, they must also be added 
to the time zone list. (See \code{timeZoneList} for more information.) 
}
\seealso{
\code{\link{timeZoneList}},  \code{\link{timeZoneC}},  \code{\linkS4class{timeZone}}  class.  
}
\examples{
timeZoneR() 
timeZoneR(offset=3*3600) 
# time zone with daylight time that changed to daylight time on the  
# last Sunday in April and last Sunday in September through 1989,  
# and then on the 1st Sunday in May and October thereafter.
# Each time change occurs at 2AM local standard time.
timeZoneR( offset = 3600, 
		  yearfrom=c( -1, 1990), yearto=c( 1989, -1 ),  
		  hasdaylight=c( TRUE, TRUE ), dsextra=c( 3600, 3600 ),  
		  monthstart=c( 4, 5 ), codestart=c( 2, 3 ),  
		  daystart=c( 0, 0 ), xdaystart=c(0,1), 
		  timestart=c( 2*3600, 2*3600 ),  
		  monthend=c( 9, 10 ), codeend=c( 2, 3 ),  
		  dayend=c( 0, 0 ), xdayend=c(0,1), 
		  timeend=c(2*3600, 2*3600)) 
}
\keyword{chron}
