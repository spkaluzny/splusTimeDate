\name{timeDate-class}
\alias{timeDate-class}
\alias{[,timeDate-method}
\alias{[<-,timeDate,ANY,ANY,timeDate-method}
\alias{as.character,timeDate-method}
\alias{coerce,Date,timeDate-method}
\alias{coerce,character,timeDate-method}
\alias{coerce,POSIXct,timeDate-method}
\alias{coerce,POSIXlt,timeDate-method}
\alias{coerce,numeric,timeDate-method}
\alias{coerce,timeDate,character-method}
\alias{coerce,timeDate,integer-method}
\alias{coerce,timeDate,numeric-method}
\alias{format,timeDate-method}
\alias{show,timeDate-method}
\alias{summary,timeDate-method}
\alias{shiftPositions,timeDate-method}
\alias{timeConvert,timeDate-method}
%
\title{
  Time and Date Class
}
\docType{class}
\description{
The \code{timeDate} class represents times and dates.
}
\section{Slots}{
\describe{
\item{columns}{
(\code{list}) (from \code{groupVec}).
}
\item{names}{
(\code{character}) (from \code{groupVec}).
}
\item{classes}{
(\code{character}) (from \code{groupVec}).
}
\item{format}{
(\code{character}) output format string.
}
\item{time.zone}{
(\code{character}) time zone string.
}
}
}
\details{
This class holds a vector of times and/or dates.
It extends the \code{groupVec} class, as well as the
\code{positionsCalendar} class (see the documentation for the \code{positions} class).

The \code{groupVec} portion of a time object holds a
date portion, stored as a vector of the days since January 1, 1960, and a
time portion, stored as a vector of the number of milliseconds since
midnight, GMT.  The \code{groupVec} column names are \code{"julian.day"} and
\code{"milliseconds"}, and the column classes are \code{integer}.

The string in the \code{time.zome} slot must be one of the names 
from the time zone list (see the \code{timeZoneList}
documentation).  Because times are stored
internally always in GMT, the time-zone string is used for
printing and for converting to calendar times and dates
(month/day/year, hour/minute/second/millisecond). You can change these directly. 
You can also change the format directly, but we do not recommend changing the 
\code{groupVec} slots directly.
}
\section{Time functions}{
Objects of \code{class.time} can be created using the \code{new} function, in
which case they are constructed to have length 0 and the default format and
zone from \code{timeDateOptions("time.out.format")} and \code{timeDateOptions("time.zone")}
respectively.  Alternatively, they can be created using the \code{timeDate} and
\code{timeCalendar} functions.

There are \code{as} relationships set up for \code{timeDate} objects to coerce them to
and from \code{character}, \code{numeric}, and \code{integer}.  

For numbers, the integer part is the julian day, and the fractional part is the
fraction of the day given by the number of milliseconds divided by the
number of milliseconds in a day, in GMT.   

Addition of numbers to time objects and subtraction of numerics from time objects works as
though the time were converted to a number, the operation were
performed, and the number was converted back to a
time. Their subtraction results in a \code{timeSpan} object, and a
\code{timeSpan} object can be added to or subtracted from a time.

Only a few other mathematical functions make sense for time objects:
\code{floor}, \code{ceiling}, \code{min}, \code{max}, \code{mean}, and \code{range}.
Multiplication, division, and other operations that do not
make sense for times and dates (in the absence of an origin) result in
numbers, via automatic coercion to class \code{numeric}.

Note that while conversion to/from numerics is always in GMT, \code{floor} 
and \code{ceiling} take account of
the time zone to ouput time objects whose time is midnight in their
time zone, and whose date is no later than the input time's date for
\code{floor}, and no earlier for \code{ceiling}.  In addition to these mathematical
operations, all of the standard comparison operators have methods for
comparing two time objects.

There are also functions for time objects that pick out particular
parts.  See \code{days}, \code{hours}, and \code{mdy} for more information.

Various options are used by the time class, primarily for printing to and
reading from character strings.  See \code{timeDateOptions} for documentation.
}
\section{Formatting}{
The input and output format specifications look familiar to C and IMOX 
programmers and are patterned on the
\code{strptime} function under Solaris.
}
\section{Input formats}{
Input format strings are used to convert character strings to time objects.
When reading time objects, the default of January 1, 1960, Midnight GMT
is supplied, and the input format specifications below can be used
to override this default time.  They are read in from left to right.
If the entire input string is not matched by the format string, or if
the resulting time or date is not valid, an \code{NA} is inserted into the time
vector.  (To skip characters in a string, use \%c or \%w.)

\strong{NOTE}: If you are reading a time zone from your
character string, the notation used for the time zone in your character
string must be one of the components of the time zone list.  See
documentation for \code{timeZoneList} for more information.

\describe{
\item{*}{
anything not in this list matches itself explicitly.
}
\item{\%c}{
any single character, which is skipped. This is useful for skipping entries like days of the week, which, if
abbreviated, could be skipped by "\%3c" (see also \%w). To skip the rest of the string, use "\%\$c".
}
\item{\%d}{
input day, within a month, as integer.
}
\item{\%H}{
input hour as integer.
}
\item{\%m}{
input month as integer or as alpha string.  If an alpha
string, case does not matter, and any substring of a month in
\code{timeDateOptions("time.month.name")} that distinguishes it from the other
months is accepted.
}
\item{\%M}{
input minute as integer.
}
\item{\%n}{
input milliseconds as integer, without considering field width as in \%N.
}
\item{\%N}{
input milliseconds as integer.  A field width (either given
explicitly or inferred from input string) of 1 or 2 causes input
of 10ths or 100ths of a second instead, as if the digits were
following a period.  Field widths greater than 3 are likely to
result in illegal input.
}
\item{\%p}{
accepts strings from \code{timeDateOptions("time.am.pm")}, defining \code{am} and \code{pm}, with
matching as for months.  If \code{pm} is given, and the hour is before 13, the time is
bumped into the afternoon.  If \code{am} is given, and the hour is 12, the time
is bumped into the morning.  Note that this modifies previously-parsed hours only.
}
\item{\%S}{
input seconds as integer.
}
\item{\%w}{
a whitespace-delimited word, which is skipped (no width or
delimiter specification. For that, use \%c).
}
\item{\%y}{
input year as integer.  If less than 100,
\code{timeDateOptions("time.century")} is used to determine the actual year.
}
\item{\%Y}{
input year as integer, without considering the century.
}
\item{\%Z}{
a time zone string.  Accepts a whitespace-delimited word, unless
another delimiter or width is specified.  The legal time zones are
the names of \code{timeZoneList()}.
}
\item{\%(digits)(char)}{
if there are one or more digits between "\%" and the
specification character, these are parsed as an integer, and specify
the field width to be used.  The following (digits) characters are
scanned for the specified item.
}
\item{\%:(delim)(char)}{
if there is a colon and any single character
between a "\%" and the specification character, the field is taken to
be all the characters up to but not including the given delimiter
character.  The delimiter itself is not scanned or skipped by the
format.
}
\item{\%$(char)}{
If there is a \$ between a \% and a specification character,
the field goes to the end of the input string.
}
\item{whitespace}{
whitespace (spaces, tabs, carriage returns, and so on) is ignored in
the input format string.  In the string being
parsed, any amount of white space can appear between elements of the
date/time.  Thus, the parse format string " \%H:\%M: \%S " parses "5: 6:45".
}
\item{[\dots]}{
specify optional specification.  Text and specifications
within the brackets optionally can be included.  This does not
support fancy backtracking between multiple optional specs.
}
\item{\%\%,\%[,\%]}{
the \%, [, and ] characters, which must be matched in
the input string.
}
}
}
\section{Output formats}{
Output formats are used to convert time objects to character strings.
They are stored in the format slot of the time object. During output,
if a given field width is too short to hold the output, if that
output field is a character field, the leftmost characters are
printed, but if it is a numeric field, the output string becomes "NA".
The following format specifications can be used:
\describe{
\item{*}{
anything not in this list matches itself explicitly (including
whitespace, unlike in input specs).
}
\item{\%a}{
print abbreviated weekday ("Mon", and so on) from timeDateOptions("time.day.abb").
}
\item{\%A}{
print full weekday ("Monday", and so on) from timeDateOptions("time.day.name").
}
\item{\%b}{
print month as abbreviation, from timeDateOptions("time.month.abb").
}
\item{\%B}{
print month as full name, from timeDateOptions("time.month.name").
}
\item{\%C}{
print year within century as integer: 0-99.
}
\item{\%d}{
print day within month as integer: 1-31.
}
\item{\%D}{
print day within year as integer: 1-366.
}
\item{\%H}{
print hour (24-hour clock) as integer, 0-23.
}
\item{\%I}{
print hour (12-hour clock) as integer, 1-12.
}
\item{\%m}{
print month as integer: 1-12.
}
\item{\%M}{
print minutes as integer: 0-59.
}
\item{\%N}{
print milliseconds as integer.  It is a good idea to pad with zeros if this is
after a decimal point!  A width of less than 3 causes printing
of 10ths or 100ths of a second instead: 0-999.
}
\item{\%p}{
print  "am" or "pm", using strings from timeDateOptions("time.am.pm").
}
\item{\%q}{
print quarter of the year, as integer: 1-4.
}
\item{\%Q}{
print quarter of the year, as Roman numeral: I-IV.
}
\item{\%S}{
print seconds as integer: 0-59 (60 for leap second).
}
\item{\%y}{
print year as two-digit integer if year is in century specified
by timeDateOptions("time.century"), otherwise full year.
}
\item{\%Y}{
print full year as integer (see also \%C).
}
\item{\%Z}{
print the time zone string from the objects \code{time.zone} slot.
}
\item{\%z}{
print the time zone string from the objects \code{time.zone} slot,
using the part before the first \code{"/"} character if it is standard
time, and the part after the first \code{"/"} character if it is daylight
savings time (that is, if the time zone is \code{"PST/PDT"}).  If there is no
\code{"/"} character, the entire time zone is used for both.
}
\item{\%\%}{
print the \% character
}
\item{\%(digits)(char)}{
if there are one or more digits between "\%" and the
specification character, these are parsed as an integer, and specify
the field width to use.  The value is printed, right justified
using (digits) characters.  If (digits) begins with zero, the field
is left-padded with zeros if it is a numeric field, otherwise it is
left-padded with spaces. If a numeric value is too long for the field
width, the field is replaced with asterix "*" characters to indicate
overflow; character strings can be abbreviated by specifying short fields.
}
}
}
\note{
The calendar follows the conventions of the British Empire, which
changed from Julian to Gregorian calendars in September of 1752.
Calendar dates prior to 1920 were different in many countries.  See
the "Calendar FAQ" posted regularly to Usenet news groups soc.history,
sci.astro, sci.answers, soc.answers, and news.answers, and to a web
site at http://www.pip.dknet.dk/~c-t/calendar.html for more
information on the history of calendars around the world.
The time objects allow days with leap seconds, but calculated times
of day for days containing leap seconds might be off by a second; they
are treated as though the leap second occurred at the very end of the day,
because there is currenly no provision in the splusTimeDate package
for keeping track of leap seconds.
}
\seealso{
\code{\linkS4class{groupVec}}  class, \code{\linkS4class{timeSpan}}  class, \code{\link{timeDateOptions}},  \code{\link{timeDate}}   function, \code{\link{timeCalendar}},  \code{\link{format.timeDate}}.   
}
\examples{
## The default format for input is initially:
##    "\%m[/][.]\%d[/][,]\%y [\%H[:\%M[:\%S[.\%N]]][\%p][[(]\%3Z[)]]]"
## This allows reading strings such as
##    "Jan 22 1997", "January 22, 1997", "1/22/97", "1/22/97 2PM"
## The default format for output is initially:
##    "\%02m/\%02d/\%Y \%02H:\%02M:\%02S.\%03N"
## Another choice would be
##    "\%A \%B \%d, \%Y \%I:\%02M \%p"
## These would result in the following output:
##    "01/22/1997 14:34:45.025" and "Thursday January 22, 1997 2:34 PM"
}
\keyword{chron}
\keyword{classes}
