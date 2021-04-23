\name{timeDateOptions}
\alias{timeDateOptions}
\alias{timeDateFormatChoose}
\title{
  Set or Return \code{timeDate} Options
}
\description{
Provides the means to set or view global options for working with
\code{timeDate} objects and classes.
}
\usage{
timeDateOptions(...)
}
\arguments{
  \item{\dots}{
    you can give a list or vector of character strings as the only
    argument, or you can give any number of arguments in the \code{name=value}
    form. Optionally, you can give no arguments.
    See the \bold{Value} and \bold{Side Effects} sections for explanation.
  }
}
\section{Common options}{
  \describe{
    \item{}{
      Use the following option names in a call to
      \code{timeDateOptions}
    }
  \item{ts.eps}{
    a small number specifying the time series comparison tolerance. This is used 
    throughout the time series functions for frequency comparisons. Frequencies 
    are considered equal if they differ in absolute value by less than \code{ts.eps}.
}\item{sequence.tol}{
a number specifying the tolerance for converting numeric vectors to numeric sequences. 
If a numeric vector is an arithmetic sequence to within \code{sequence.tol}, it can 
be converted to a sequence.
}\item{time.in.format}{
a character string specifying the format for reading \code{timeDate} objects
from character strings using the \code{as} and \code{timeDate} functions.
The default value is
\code{"\%m[/][.]\%d[/][,]\%y [\%H[:\%M[:\%S[.\%N]]][\%p][[(]\%3Z[)]]]"},
which reads a wide variety of date strings.
To use the European day/month/year format, set this to
\code{"\%d[/][.]\%m[/][,]\%y [\%H[:\%M[:\%S[.\%N]]][\%p][[(]\%3Z[)]]]"}.
The elements of this format string are described in the
documentation for the \code{timeDate} class
(\code{class.timeDate}).
}\item{time.out.format}{
a character string specifying the format for printing \code{timeDate} objects
to character strings.
The default value is \code{"\%02m/\%02d/\%04Y \%02H:\%02M:\%02S.\%03N"}.
To use the European day/month/year format, set this to
\code{"\%02d/\%02m/\%04Y \%02H:\%02M:\%02S.\%03N"}.
The elements of this format string are described in the
documentation for the \code{timeDate} class
(\code{class.timeDate}).
}\item{time.out.format.notime}{
a character string specifying the format for printing
\code{timeDate} objects when the
\code{time.zone} option is set to GMT
and the time of every element of the \code{timeDate} object is midnight.
See \code{timeDate} for more information.
}\item{time.month.name}{
a 12-element character vector giving the names of the months.
}\item{time.month.abb}{
a 12-element character vector giving the abbreviations for the names of the months.
}\item{time.day.name}{
a 7-element character vector giving the names of the days of the week, starting with Sunday.
}\item{time.day.abb}{
a 7-element character vector giving the abbreviations for the names of the days of the week, starting with Sunday.
}\item{time.century}{
an integer indicating the first year of a 100-year span. This value is used to interpret and print two-digit years. 
For example, if \code{time.century=1950}, the year 50 is interpreted as 1950 and the year 49 is interpreted as 2049. 
If \code{time.century=1900}, the year 0 means 1900 and the year 99 means 1999.
}\item{time.am.pm}{
a 2-element character vector giving strings for printing "AM" and "PM" in time objects.
}\item{time.zone}{
a character string specifying the default time zone when none is given in a time object.
}\item{tspan.in.format}{
a character string specifying the format for reading \code{timeSpan} objects from character strings using the \code{as} and \code{timeSpan} functions.
}\item{tspan.out.format}{
a character string specifying the format for printing \code{timeSpan} objects to character strings.
}
}
}
\section{Default values}{
The default values for some of the common options listed above are as
follows. Options that have never been set have the value \code{NULL}
\preformatted{
   sequence.tol=1e-6
   time.am.pm=c("AM", "PM")
   time.century=1930
   time.in.format="[\%m[/][.]\%d[/][,]\%y] [\%H[:\%M[:\%S[.\%N]]][\%p][[(]\%3Z[)]]]"
   time.out.format="\%02m/\%02d/\%Y \%02H:\%02M:\%02S.\%03N"
   time.day.abb=c("Sun", "Mon", ..., "Sat")
   time.day.name=c("Sunday", "Monday", ..., "Saturday")
   time.month.abb=c("Jan", "Feb", ..., "Dec")
   time.month.name=c("January", "February", ..., "December")
   time.zone="GMT"
   tspan.in.format=paste("[\%yy[ear[s]][,]] [\%dd[ay[s]][,]]",
     "[\%Hh[our[s]][,]] [\%Mm[in[ute][s]][,]] [\%Ss[ec[ond][s]][,]]",
     "[\%NM[s][S]]")
   tspan.out.format="\%dd \%Hh \%Mm \%Ss \%NMS",
   ts.eps=1e-5
 }
 }
\value{
a list, even if the list is of length 1.
\itemize{
\item If no arguments are given, \code{timeDateOptions} returns
a list of current values for all options.
\item If a character vector is given as the only argument,
\code{timeDateOptions} returns a list of current values for the options
named in the character vector.
\item If an object of mode \code{"list"} is given as the only argument,
its components become the values for options with the corresponding names.
\code{timeDateOptions} returns a list of the option values
before they were modified.
Usually, the list given as an argument is the return value
of a previous call to \code{timeDateOptions}.
\item If arguments are given in \code{name=value} form,
the values of the specified options are changed and \code{timeDateOptions}
returns a list of the option values before they were modified.
}
}
\section{Side Effects}{
If \code{timeDateOptions} is called with either a list as the single argument
or with one or more arguments in \code{name=value} form,
the options specified are changed or created.
The options are stored in a list in a local environment
within the \code{splusTimeDate} package.
Any modifications to the options disappear when the current session ends.
The next session will start with the default value of the options.
}
\details{
To see all the \code{timeDate} options and their current values,
call \code{timeDateOptions} with no arguments i.e. \code{timeDateOptions()}

To set \code{timeDateOptions} temporarily in a function,
call \code{timeDateOptions} as you normally would from the command line.
To ensure that your function finishes cleanly
and does not produce any side effects,
use \code{on.exit} with the return value from
your call to \code{timeDateOptions}.)
}
\examples{
timeDateOptions(time.zone="PST")
}
\keyword{environment}
