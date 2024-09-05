\name{days}
\alias{days}
\alias{weekdays}
\alias{months}
\alias{quarters}
\alias{years}
\title{
  Return Various Periods from a Time or Date Object 
}
\description{
Creates an ordered factor from a vector of dates according to various 
calendar periods. 
}
\usage{
days(x) 
weekdays(x, abbreviate = TRUE) 
months(x, abbreviate = TRUE) 
quarters(x, abbreviate = TRUE) 
years(x)
}
\arguments{
  \item{x}{
    a \code{dates} object. 
  }
  \item{abbreviate}{
    a logical flag. If \code{TRUE}, abbreviations should be returned for  
    days of the week, month names, or quarters. Default is \code{TRUE}. 
  }
}
\value{
returns an ordered factor corresponding to days, weekdays, months,  
quarters, or years of \code{x} for the respective function. 
}
\details{
\itemize{
\item The levels of \code{days} are the days of the month,
\code{1} through \code{31}.
\item The levels of \code{weekdays} are \code{Sunday} through \code{Saturday}.
\item The levels of \code{months} are \code{January} through \code{December}.
\item The levels of \code{quarters} are I < II < III < IV
if \code{abb=F} and 1Q < 2Q < 3Q < 4Q if \code{abbreviate=T}.
\item The levels of \code{years} are exactly the years of the dates 
in \code{x}. Years in the range of \code{x}
that are not in \code{x} itself are not interpolated.   
}
These functions are generic.

The default method works on  \code{dates} objects
and other objects that can be coerced to \code{dates} objects.

There are also methods for \code{timeDate}, which take weekday and month names
and abbreviations from  \code{timeDateOptions(c("time.day.name",
"time.day.abb", "time.month.name", "time.month.abb"))}. 
}
\seealso{
\code{\link{as.Date}}, \code{\link{julian}},
\code{\link{timeDate}}, \code{\link{hours}}, \code{\link{yeardays}},
\code{\link{hms}}  
}
\examples{
## Creates a sample dates object of julian dates 
x <- c(10, 11, 12, 42, 44, 45, 101, 102, 212, 213, 214, 300) 
dts <- as.Date(x, origin="1960-1-1")
## Alternatively, create a sample timeDate object 
dts <- as(x, "timeDate") 
## identifies a weekday or month with each julian day 
weekdays(dts) 
months(dts) 
## Produces barplot of # of days in x appearing on particular 
## weekday or month 
plot(weekdays(dts)) 
plot(months(dts)) 
## Produces boxplots of julian date by weekday or month 
plot(weekdays(dts), x) 
plot(months(dts), x) 
}
\keyword{chron}
