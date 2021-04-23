\name{timeSpan-class}
\alias{timeSpan-class}
\alias{+,positionsCalendar,timeSpan-method}
\alias{+,timeSpan,timeSpan-method}
\alias{+,timeSpan,positionsCalendar-method}
\alias{+,timeSpan,missing-method}
\alias{-,positionsCalendar,timeSpan-method}
\alias{-,timeSpan,timeSpan-method}
\alias{-,timeSpan,missing-method}
\alias{/,timeSpan,timeSpan-method}
\alias{Compare,numeric,timeSpan-method}
\alias{Compare,timeSpan,numeric-method}
\alias{Math2,timeSpan-method}
\alias{Math,timeSpan-method}
\alias{Ops,ANY,timeSpan-method}
\alias{Ops,timeSpan,ANY-method}
\alias{Ops,timeSpan,timeSpan-method}
\alias{Summary,timeSpan-method}
\alias{as.character,timeSpan-method}
\alias{coerce,numeric,timeSpan-method}
\alias{coerce,timeSpan,integer-method}
\alias{coerce,timeSpan,numeric-method}
\alias{coerce,character,timeSpan-method}
\alias{coerce,timeSpan,character-method}
\alias{cor,ANY,timeSpan-method}
\alias{cor,timeSpan,ANY-method}
\alias{cor,timeSpan,timeSpan-method}
\alias{cumsum,timeSpan-method}
\alias{cut,timeSpan-method}
\alias{diff,timeSpan-method}
\alias{format,timeSpan-method}
\alias{hms,timeSpan-method}
\alias{hours,timeSpan-method}
\alias{minutes,timeSpan-method}
\alias{seconds,timeSpan-method}
\alias{match,ANY,timeSpan-method}
\alias{match,character,timeSpan-method}
\alias{match,timeSpan,ANY-method}
\alias{match,timeSpan,character-method}
\alias{match,timeSpan,timeSpan-method}
\alias{max,timeSpan-method}
\alias{mean,timeSpan-method}
\alias{median,timeSpan-method}
\alias{min,timeSpan-method}
\alias{quantile,timeSpan-method}
\alias{range,timeSpan-method}
\alias{show,timeSpan-method}
\alias{sort.list,timeSpan-method}
\alias{sort,timeSpan-method}
\alias{summary,timeSpan-method}
\alias{sum,timeSpan-method}
\alias{timeCeiling,timeSpan-method}
\alias{timeTrunc,timeSpan-method}
\alias{timeFloor,timeSpan-method}
\alias{var,ANY,timeSpan-method}
\alias{var,timeSpan,ANY-method}
%
\title{
  Time Span Class
}
\docType{class}
\description{
The \code{timeSpan} class represents time spans.
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
  }
}
\details{
The \code{timeSpan} class is constructed to hold a vector of time spans.
It extends the \code{groupVec} and \code{groupVecVirtual} classes,
as well as \code{timeInterval}.

The \code{groupVec} portion of the time class object holds a
day portion, stored as an integer vector of the number of full days in each
time span, and a time portion, stored as a vector of the number of
milliseconds in each time span.  The \code{groupVec} column names are
\code{"julian.day"} and \code{"milliseconds"},
and the column classes are \code{integer}.
The user can directly change the format specified by the \code{format} 
slot (see below), but it is not recommended to change 
the \code{groupVec} slots directly.
}
\section{Time span functions}{
You can create objects of class \code{timeSpan} by using either the 
\code{new} function (in which case they are set up to have length 0 and 
the default format from \code{timeDateOptions("tspan.out.format")}), 
or by using the \code{timeSpan} function.

\code{as} relationships are established for \code{timeSpan} objects to coerce them to
and from \code{character}, \code{numeric}, and \code{integer}.  

For numbers, the integer part is the number of days, and the fractional part is the
fraction of the day given by the number of milliseconds divided by the
number of milliseconds in a day. Adding or subtracting numbers to or from  
\code{timeSpan} objects works as though the \code{timeSpan} is 
converted to a number, the operation is performed, and the number is 
converted back to a \code{timeSpan}.

Multiplication and division by numbers are also defined.
You can add, subtract, and divide two \code{timeSpan} objects.
(For division, the result is a number.)  You can add or subtract  
a \code{timeSpan} object to or from a \code{timeDate} object.

Only a few other mathematical functions make sense for \code{timeSpan} objects. 
These are \code{floor}, \code{ceiling}, \code{min}, \code{max}, \code{sum}, \code{mean}, and \code{range}.
Multiplication, division, and operations that do not
make sense directly for \code{timeSpan} objects result in
numbers, via automatic coercion to class numeric.
In addition to these mathematical
operations, all of the standard comparison operators have methods for
comparing two \code{timeSpan} objects.
}
\section{Input formats}{
Input format strings are used in the conversion of character strings
to \code{timeSpan}
objects. They are read in from left to right, and each format
specification encountered is parsed, and the resulting amount
of time added to the time span.
If the entire input string is not matched by the format string,
an NA is inserted into the time span vector.  (To skip characters in a
string, use \%c or \%w.)
\describe{
\item{*}{
anything not in this list matches itself explicitly.
}
\item{\%c}{
any single character, which is skipped. This can be used with 
widths and delimiters such as "\%3c" (to skip 3 characters) 
and "\%\$c" (to skip the rest of the string).
}
\item{\%d }{
input number of days as integer.
}
\item{\%H }{
input number of hours as integer.
}
\item{\%M}{
input number of minutes as integer.
}
\item{\%N}{
input number of milliseconds as integer.
}
\item{\%S}{
input number of seconds as integer.
}
\item{\%w}{
a whitespace-delimited word, which is skipped (no width or
delimiter specification. For that, use \%c).
}
\item{\%W}{
input number of weeks as integer.
}
\item{\%y}{
input number of 365-day years as integer.
}
\item{\%(digits)(char)}{
For one or more digits between "\%" and the
specification character, these are parsed as an integer, and specify
the field width to be used. The following (digits) characters are
scanned for the specified item.
}
\item{\%:(delim)(char)}{
For a colon and any single character
between a "\%" and the specification character, the field is taken to
be all the characters up to but not including the given delimiter
character.  The delimiter itself is not scanned or skipped by the
format.
}
\item{\%$(char)}{
For a \$ between a \% and a specification character,
the field goes to the end of the input string.
}
\item{whitespace}{
whitespace (spaces, tabs, carriage returns, etc.) is ignored in
the input format string.  In the string being
parsed, any amount of white space can appear between elements of the
date/time.  Thus, the parse format string " \%H:\%M: \%S " will parse "5: 6:45".
}
\item{[\dots]}{
specify optional specification.  Text and specifications
within the brackets may optionally be included.  This does not
support fancy backtracking between multiple optional specs.
}
\item{\%\%,\%[,\%]}{
the \%, [, and ] characters, which must be matched in
the input string.
}
}
}
\section{Output formats}{
Output formats are used to convert \code{timeSpan} objects to character strings
and are stored in the \code{format} slot of the object. During output,
if a given field width is too short to hold the output,
the output string becomes "NA".  Note that since time spans can
be positive or negative, you should use care in specifying field widths.
You can use the following format specifications:
\describe{
\item{*}{
anything not in this list matches itself explicitly (including
whitespace, unlike in input formats).
}
\item{\%d}{
total number of days in span as integer.
}
\item{\%D}{
number of days subtracting off full 365-day years as integer: 1-364.
}
\item{\%E}{
number of days subtracting off 7-day weeks as integer: 1-6.
}
\item{\%H}{
number of hours subtracting off days as integer, 0-23.
}
\item{\%M}{
number of minutes subtracting off hours as integer: 0-59.
}
\item{\%N}{
number of milliseconds in span, subtracting off seconds as integer.
}
\item{\%S}{
number of seconds subtracting off minutes as integer: 0-59.
}
\item{\%s}{
number of seconds subtracting off days as integer.
}
\item{\%W}{
number of 7-day weeks in time span as integer.
}
\item{\%y}{
number of 365-day years in span as integer.
}
\item{\%\%}{
the \% character.
}
\item{\%(digits)(char)}{
if there are one or more digits between "\%" and the
specification character, these are parsed as an integer and specify
the field width to be used. The value is printed right-justified,
using (digits) characters. If (digits) begins with zero, the field
is left-padded with zeros if it is a numeric field; otherwise, it is
left-padded with spaces. If the value is too long for the field
width, the output string becomes "NA" for that time span.
}
}
}
\seealso{
\code{\linkS4class{groupVec}}  class, \code{\linkS4class{timeDate}}  class, \code{\link{timeDateOptions}},  \code{\link{timeSpan}}   function, \code{\link{format.timeSpan}}.   
}
\examples{
## The default format for input is initially:
##   "[\%yy[ear[s]][,]] [\%dd[ay[s]][,]] [\%Hh[our[s]][,]]
##      [\%Mm[in[ute][s]][,]] [\%Ss[ec[ond][s]][,]] [\%NM[s][S]]"
##This allows reading strings such as
##   "378d 21h 04min 36s 365MS", "378 d", "-1y, -13d, -21h -4m"
##The default format for output is initially:
##   "\%dd \%Hh \%Mm \%Ss \%NMS"
##This results in output such as:
##   "378d 21h 4m 36s 365MS"   "-378d -21h -4m -36s -365MS"
}
\keyword{chron}
\keyword{classes}
