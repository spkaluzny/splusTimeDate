\name{timeZoneList}
\alias{timeZoneList}
\title{
  Time Zone List 
}
\description{
Returns or modifies the time zone list. 
}
\usage{
timeZoneList(...)
}
\arguments{

\item{\dots}{
(see below) 
}
}
\value{
returns the value of the time zone list before the function call is  
returned.  If arguments are given, it is returned invisibly. 
}
\section{Side Effects}{
If arguments are given, they are used to modify the current value  
of \code{.time.zone.list}, which is assigned in the \code{splusTimeDate} package environment.
It is like \code{timeDateOptions}, where if you want your entries to the time zone list to persist in  
subsequent sessions, you should use \code{timeZoneList} in \code{.First}. 
}
\details{
The time zone list is a named list whose names are the character strings that 
are recognized when you convert strings to time objects, and 
whose elements are the corresponding time zone objects. (See the 
documentation for class \code{timeZone}.)  The  
\code{timeZoneList} function provides an easy way for the user to define 
the character strings to use for input of given time zones, and 
to define user-defined time zones for use in \code{timeDate} objects. 
For example, a North American user would 
probably want to recognize "EST" as the US or Canadian time zone  
known as Eastern Standard Time, whereas an Australian user might prefer to  
have "EST" refer to Eastern Australian time. 
The \code{timeZoneList} function has the following behavior: 
\itemize{
\item If no arguments are given, the current time zone list 
is returned. 
\item If a single list is given as the argument,  
its named components are added to the time zone list. 
\item If multiple named arguments are given, they are added to the list.  
}
In either of the two latter 
cases, the elements to be added to the list must be time zone objects. 
The default time zone list has the following built-in zone components. 
(See documentation on the \code{timeZone} class for more information.) 
\describe{
\item{Atlantic}{
can/atlantic (Standard/daylight Canadian Atlantic time) 
}
\item{ADT}{
can/atlantic 
}
\item{AST}{
can/atlantic 
}
\item{Halifax}{
can/atlantic 
}
\item{PuertoRico}{
st/atlantic (Atlantic Standard Time, Puerto Rico and Virgin Islands)
}
\item{Eastern}{
us/eastern (Standard/daylight US Eastern time) 
}
\item{EST}{
us/eastern 
}
\item{EDT}{
us/eastern 
}
\item{EST5EDT}{
us/eastern 
}
\item{EST/EDT}{
us/eastern 
}
\item{Indiana}{
st/eastern (Standard only US/Canadian Eastern time) 
}
\item{Toronto}{
can/eastern (Standard/daylight Canadian Eastern time) 
}
\item{Central}{
us/central (Standard/daylight US Central time) 
}
\item{CST}{
us/central 
}
\item{CDT}{
us/central 
}
\item{CST6CDT}{
us/central 
}
\item{CST/CDT}{
us/central 
}
\item{Chicago}{
us/central 
}
\item{Winnipeg}{
can/central (Standard/daylight Canadian Central time) 
}
\item{Mountain}{
us/mountain (Standard/daylight US Mountain time) 
}
\item{MST}{
us/mountain 
}
\item{MDT}{
us/mountain 
}
\item{MST7MDT}{
us/mountain 
}
\item{MST/MDT}{
us/mountain 
}
\item{Denver}{
us/mountain 
}
\item{Arizona}{
st/mountain (Standard only US/Canadian Mountain time) 
}
\item{Edmonton}{
can/mountain (Standard/daylight Canadian Mountain time) 
}
\item{Pacific}{
us/pacific (Standard/daylight US Pacific time) 
}
\item{PST}{
us/pacific 
}
\item{PDT}{
us/pacific 
}
\item{PST8PDT}{
us/pacific 
}
\item{PST/PDT}{
us/pacific 
}
\item{Vancouver}{
can/pacific (Standard/daylight Canadian Pacific time) 
}
\item{Alaska}{
us/alaska (Standard/daylight US Alaska time) 
}
\item{AKST}{
us/alaska 
}
\item{AKDT}{
us/alaska 
}
\item{AKST/AKDT}{
us/alaska 
}
\item{Aleutian}{
us/hawaii (Standard/daylight US Hawaii/Aleutian time) 
}
\item{HST}{
st/hawaii (Standard only US Hawaii/Aleutian time) 
}
\item{Hawaii}{
st/hawaii 
}
\item{Midway}{
st/samoa (Standard time for Samoa) 
}
\item{Samoa}{
st/samoa 
}
\item{SST}{
st/samoa 
}
\item{Japan}{
st/japan (Standard time for Japan) 
}
\item{Tokyo}{
st/japan 
}
\item{JST}{
st/japan 
}
\item{China}{
st/china (Standard time for China and Western Australia) 
}
\item{HongKong}{
hongkong (Standard/daylight time for Hong Kong) 
}
\item{Singapore}{
singapore (Standard time for Singapore, reflecting changed zones in 1982) 
}
\item{Sydney}{
aust/nsw (Standard/summer time for New South Wales, Australia) 
}
\item{Hobart}{
aust/tasmania (Standard/summer time for Tasmania, Australia) 
}
\item{Melbourne}{
aust/victoria (Standard/summer time for Victoria, Australia) 
}
\item{Adelaide}{
aust/south (Standard/summer time for South Australia) 
}
\item{Darwin}{
st/caustralia (Standard only time for Central Australia) 
}
\item{Perth}{
aust/western (Standard/daylight time for Western Australia) 
}
\item{Auckland}{
newzealand (Standard time for New Zealand) 
}
\item{NZST}{
newzealand 
}
\item{NZDT}{
newzealand 
}
\item{Marshall}{
st/newzealand (Marshall Islands Standard Time)
}
\item{Wake}{
st/newzealand (Wake Islands Standard Time)
}
\item{IDLE}{
st/newzealand (International Date Line East)
}
\item{Chamorro}{
st/eaustralia (Chamorro Standard Time - Guam and Northern Mariana Islands)
}
\item{ChST}{
st/eaustralia (Chamorro Standard Time - Guam and Northern Mariana Islands)
}
\item{Yap}{
st/eaustralia (Yap Time)
}
\item{YAPT}{
st/eaustralia (Yap Time)
}
\item{Caroline}{
st/caroline (Line Islands Time - Caroline and other Line Islands)
}
\item{LINT}{
st/caroline (Line Islands Time - Caroline and other Line Islands)
}
\item{UTC}{
utc (Greenwich Mean Time/Universal Coordinated Time) 
}
\item{GMT}{
utc 
}
\item{GDT}{
britain (Standard time for Great Britain) 
}
\item{London}{
britain (Standard time for Great Britain) 
}
\item{BST}{
britain 
}
\item{WET}{
europe/west (Standard/summer time for EU members, Western zone) 
}
\item{Wes}{
europe/west (Standard/summer time for EU members, Western zone) 
}
\item{WEST}{
europe/west 
}
\item{WET/WEST}{
europe/west 
}
\item{WED}{
europe/west 
}
\item{WEDT}{
europe/west 
}
\item{CET}{
europe/central (Standard/summer time for EU members, Central zone) 
}
\item{CEST}{
europe/central 
}
\item{MET}{
europe/central 
}
\item{MEST}{
europe/central 
}
\item{MET/MEST}{
europe/central 
}
\item{EET}{
europe/east (Standard/summer time for EU members, Eastern zone)
}
\item{EEST}{
europe/east 
}
\item{EET/EEST}{
europe/east 
}
}
}
\seealso{
\code{\link{timeZoneC}},  \code{\link{timeZoneR}},  \code{\linkS4class{timeZone}}  class.  
}
\examples{
# return the entire time zone list 
timeZoneList() 
# define the string "PDT8PST" to mean US Pacific time 
timeZoneList(PDT8PST = timeZoneC("us/pacific")) 
# define a time zone for a small island 1/2 hour east of GMT 
timeZoneList(small.island = timeZoneR(offset=1800)) 
}
\keyword{chron}
% docclass is function
% Converted by mySd2Rd version 47442.
