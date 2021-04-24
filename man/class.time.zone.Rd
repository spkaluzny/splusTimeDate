\name{timeZone-class}
\alias{timeZone-class}
\alias{timeZoneC-class}
\alias{timeZoneR-class}
\alias{coerce,character,timeZoneC-method}
\alias{show,timeZoneC-method}
\alias{summary,timeZoneC-method}
\alias{show,timeZoneR-method}
\alias{summary,timeZoneR-method}
\title{
  Time Zone Classes 
}
\docType{class}
\description{
The \code{timeZone} classes represent time zones.
}
\section{'timezone' slots}{
  \code{timeZone} is a virtual class and has no slots. 
}
\section{'timezonec' slots}{
  \describe{
    \item{name}{
      (\code{character}) the name of a built-in time zone. 
    }
  }
}
\section{'timezones' slots}{
  \describe{
    \item{offset}{
      (\code{integer}) the offset from GMT (in seconds) when not on daylight savings time. 
    }
    \item{rules}{
      (\code{data.frame}) rules encoding when to go on daylight savings time (see below). 
    }
  }
}
\details{
The \code{timeZone} class is a virtual class for time zones.  All 
time zones classes have an \code{is} relationship with \code{timeZone}. 

The \code{timeZoneC} class is a placeholder for a built-in time 
zone, and it has only one slot, which is the official name of the zone; 
it extends \code{timeZone}. 

The \code{timeZoneR} class is for user-defined time zones, and also extends 
\code{timeZone}. 
}
\section{Built-in zones}{
The splusTimeDate package contains built-in time zones for
the 24 standard time zones around the world.
We also include daylight savings time in various areas, 
and standard time for Central Australia, which is 1/2 hour off 
Eastern Australia.  Currently, the correct daylight savings areas  
provided are: 
\itemize{
\item US (1967 and beyond).
\item Canada (1974 and beyond).
\item New Zealand (1976 and beyond).
\item Australia (1973 and beyond).
\item Great Britain (1972 and beyond).
\item European Union (1977 and beyond).
\item Hong Kong (1970 and beyond).
}
Also, we provide a special time zone 
for Singapore, which was 7:30 ahead of GMT until May of 1982, when 
it changed over to 8:00.  

The official names of the time zones, in order around the world, 
are shown below along with their offset from Universal Coordinated Time 
(UTC, also known as GMT). 
\describe{
\item{st/newzealand}{
Standard time for New Zealand, UTC East 12 hours. 
}
\item{newzealand}{
Standard/summer time for New Zealand.  
}
\item{st/caroline}{
Standard time for Caroline, UTC East 11 hours. 
}
\item{st/eaustralia     }{
Standard time for Eastern Australia, UTC East 10 hours. 
}
\item{aust/nsw          }{
Standard/summer time for New South Wales, Australia. 
}
\item{aust/tasmania     }{
Standard/summer time for Tasmania, Australia. 
}
\item{aust/victoria     }{
Standard/summer time for Victoria, Australia. 
}
\item{st/caustralia    }{
Standard time for Central Australia, UTC East 9:30 hours. 
}
\item{aust/south        }{
Standard/summer time for South Australia.
}
\item{st/japan          }{
Standard time for Japan, UTC East 9 hours. 
}
\item{st/china          }{
Standard time for China, UTC East 8 hours. 
}
\item{aust/western      }{
Standard/summer time for Western Australia. 
}
\item{hongkong}{
Standard/summer time for Hong Kong. 
}
\item{singapore}{
Standard time for Singapore, reflecting changed zones in 1982. 
}
\item{st/saigon         }{
Standard time for Saigon, UTC East 7 hours. 
}
\item{st/kazakh         }{
Standard time for Kazakh area, UTC East 6 hours.
}
\item{st/pakistan       }{
Standard time for Pakistan, UTC East 5 hours.
}
\item{st/caspian        }{
Standard time for Caspian Sea area, UTC East 4 hours. 
}
\item{st/moscow         }{
Standard time for Moscow, UCT East 3 hours. 
}
\item{st/eeurope        }{
Standard time in Eastern European zone, UTC East 2 hours. 
}
\item{europe/east       }{
Standard/summer time for EU members, Eastern zone. 
}
\item{st/ceurope        }{
Standard time in Central European zone, UTC East 1 hour. 
}
\item{europe/central    }{
Standard/summer time for EU members, Central zone. 
}
\item{utc}{
UTC (also known as GMT). 
}
\item{britain}{
Standard/summer time for Great Britain, 
}
\item{europe/west       }{
Standard/summer time for EU members, Western zone, 
}
\item{st/azores         }{
Standard time for Azores, UTC West 1 hour. 
}
\item{st/oscar          }{
Standard time for Oscar, UTC West 2 hours. 
}
\item{st/wgreenland     }{
Standard time for Western Greenland, UTC West 3 hours. 
}
\item{can/newfoundland  }{
Standard/daylight time for Newfoundland, Canada.
}
\item{st/atlantic       }{
Standard time for Atlantic time zone, UTC West 4 hours. 
}
\item{can/atlantic      }{
Standard/daylight Canadian Atlantic time.  
}
\item{st/eastern        }{
Standard time for Eastern time zone, UTC West 5 hours. 
}
\item{us/eastern        }{
Standard/daylight US Eastern time.  
}
\item{can/eastern       }{
Standard/daylight Canadian Eastern time.  
}
\item{st/central        }{
Standard time for Central time zone, UTC West 6 hours. 
}
\item{us/central        }{
Standard/daylight US Central time.  
}
\item{can/central       }{
Standard/daylight Canadian Central time.  
}
\item{st/mountain       }{
Standard time for Mountain time zone, UTC West 7 hours. 
}
\item{us/mountain       }{
Standard/daylight US Mountain time.  
}
\item{can/mountain      }{
Standard/daylight Canadian Mountain time . 
}
\item{st/pacific        }{
Standard time for Pacific time zone, UTC West 8 hours. 
}
\item{us/pacific        }{
Standard/daylight US Pacific time.  
}
\item{can/pacific       }{
Standard/daylight Canadian Pacific time . 
}
\item{st/alaska         }{
Standard time for Alaska/Yukon time, UTC West 9 hours. 
}
\item{us/alaska         }{
Standard/daylight US Alaska time. 
}
\item{can/yukon         }{
Standard/daylight Canadian Yukon time.  
}
\item{st/hawaii         }{
Standard time for Hawaii/Alleutian, UTC West 10 hours. 
}
\item{us/hawaii         }{
Standard/daylight US Hawaii/Alleutian time. 
}
\item{st/samoa          }{
Standard time for Samoa, UTC West 11 hours. 
}
}
}
\section{Using zones}{
You can use the time zones listed in \strong{Built-In Zones} for various operations  
on \code{timeDate} objects, including reading times from character data, 
writing times as character data, and converting between time zones.   
However, normally the names listed in the table are not used directly,  
because users want to use the names commonly used in their 
areas, such as CST for Central Standard Time in the US, Canada, or 
Australia.  The correspondence between "convenient" and "official" names 
is set up using the \code{timeZoneList} function. 
}
\section{Defining zones}{
Besides using the time zones listed in \strong{Built-In Zones}, users can also define  
their own time zones through use of the \code{timeZoneR} class.  This 
class allows specification of a time zone with an offset from GMT, 
in seconds, in the \code{offset} slot, and a data frame containing rules 
for when and how to go on daylight savings time in the \code{rules} slot.  
(If there is no daylight savings time in this time zone, the data frame  
should be empty or have 0 rows.)  

Each row in the rules data frame 
encodes a range of years' daylight savings rules; they must be in  
order to work properly.  The rules are encoded in the following 
columns of the data frame (which must be in order, and all of class 
integer): 

\tabular{ll}{
\code{yearfrom} \tab starting year for rules, or -1 to start at the beginning of time. \cr
\code{yearto} \tab ending year for rules, or -1 to end at the end of time.  \cr
\code{hasdaylight} \tab indicates whether daylight savings time is used in this year range. 
Note that this is an integer value, 1 or 0, for true or false.  \cr
\code{dsextra} \tab offset (in seconds) to add to the regular offset when daylight time is  
in effect.  \cr
\code{monthstart} \tab month (1-12) in which daylight savings time starts (may be before 
\code{monthend} for southern hemisphere).  \cr
\code{codestart} \tab code telling how to interpret \code{daystart}
and \code{xdaystart} to calculate the day
within the month for starting daylight savings time.\cr
 \tab 1 = start on the \code{daystart} (1-31) day of the month.\cr
 \tab 2 = start on the last \code{daystart} (0-Sunday through 6-Saturday) weekday  
in the month.\cr
 \tab 3 = start on the first \code{daystart} weekday (0-Sunday through 6-Saturday)
on or after the \code{xdaystart} (1-31) day of the month.\cr
 \tab 4 = start on the last \code{daystart} weekday (0-Sunday through 6-Saturday)  
on or before the \code{xdaystart} (1-31) day of the month.\cr
\code{daystart} \tab see \code{codestart}.\cr
\code{xdaystart} \tab see \code{codestart}.\cr
\code{timestart} \tab seconds after midnight local standard time to start daylight savings time,
on the day specified by \code{codestart}, and so on. \cr
\code{monthend} \tab month (1-12) in which daylight savings time ends (may be after  
\code{monthstart} for southern hemisphere).  \cr
\code{codeend} \tab code telling how to interpret \code{dayend}
and \code{xdayend} to calculate the day within the month
for ending daylight savings time.\cr
 \tab 1 =  end on the \code{dayend} (1-31) day of the month.\cr
 \tab 2 = end on the last \code{dayend} (0-Sunday through 6-Saturday) weekday  
in the month.\cr
 \tab 3 = end on the first \code{dayend} weekday (0-Sunday through 6-Saturday) 
on or after the \code{xdayend} (1-31) day of the month. \cr
 \tab 4 = end on the last \code{dayend} weekday (0-Sunday through 6-Saturday)  
on or before the \code{xdayend} (1-31) day of the month.\cr
\code{dayend} \tab see \code{codeend}.  \cr
\code{xdayend} \tab see \code{codeend}. For examples, see the \code{timeZoneR} function.  To use user-defined time 
zones, you must put them in the time zone list (see \code{timeZoneList} function). \cr
\code{timeend} \tab seconds after midnight local standard time to end daylight savings time,
on the day specified by \code{codeend}, etc. \cr
}
}
\section{References}{
Daylight savings boundaries and other time zone information are from the
Time Zone Database (often called tz or zoneinfo).
See: \url{https://www.iana.org/time-zones}.
}
\seealso{
\code{\link{timeZoneC}}  function, \code{\link{timeZoneR}}  function, \code{\link{timeZoneList}}  function.  
}
\keyword{chron}
\keyword{classes}
