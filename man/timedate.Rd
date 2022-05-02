\name{timeDate}
\alias{timeDate}
\title{
  Constructor Function for timeDate Objects 
}
\description{
Constructs a \code{timeDate} object from a character vector, a vector of julian days, or a vector of milliseconds, or constructs an empty \code{timeDate} object. 
}
\usage{
timeDate(charvec, in.format, 
         format, zone, 
         julian, ms, in.origin=c(month=1, day=1, year=1960))
}
\arguments{
    \item{charvec}{
      a character vector to read the times from. 
    }
    \item{in.format}{
      the input format string for \code{charvec}. The default
      value is \code{timeDateOptions("time.in.format")}. (Click
      \code{class.timeDate} in the \strong{SEE ALSO} section 
      for the list of allowable input format strings. 
    }
    \item{format}{
      the output format stored in the result. The default is \code{timeDateOptions("time.out.format")}. 
    }
    \item{zone}{
      the time zone stored in the result. The default value is \code{timeDateOptions("time.zone")}. 
    }
    \item{julian}{
      an integer vector of the number of days since \code{in.origin}.
      If \code{ms} is missing, this argument can also be a numeric whose fractional part gives the fraction of the day. 
    }
    \item{ms}{
      an integer vector of milliseconds since midnight. 
    }
    \item{in.origin}{
      the origin for the \code{julian} argument. This should be a vector with \code{month}, \code{day}, and \code{year} components. 
    }
}
\value{
returns a \code{timeDate} object derived from the inputs. 
}
\details{
One of \code{charvec}, \code{julian}, or \code{ms} must be supplied,
unless the function is called with no arguments. 
\itemize{
\item If \code{charvec} is supplied, then \code{timeDate} reads the times from the \code{charvec} character strings using the format string from \code{in.format}. 
This conversion uses the time zone supplied in \code{zone}. 
\item If \code{charvec} is not supplied, then \code{timeDate} uses \code{julian} and/or \code{ms} to construct the time vector. These are copied directly to 
the internals of the time object without considering the time zone (that is, they must be given in GMT, or the user must call \code{timeZoneConvert} afterwards). 
}
Leap seconds do not cause \code{NA} values, but it can cause times of day to be off by a second on the days that contain them. 

For information about possible values for the \code{in.format} and \code{format} arguments, see the documentation for the \code{timeDate} class (\code{class.timeDate}).
}
\seealso{
\code{\link{timeCalendar}}, 
\code{\linkS4class{timeDate}}, 
\code{\link{format.timeDate}}, 
\code{\link{timeZoneConvert}}. 
}
\examples{
timeDate() 
timeDate(c("1/1/97", "2/1/97", "mar 1, 1997")) 
timeDate(c("1 PM", "2 PM", "3 AM"), in.format = "\%H \%p",  
      format = "\%I \%p") 
timeDate(julian = 36, ms = 876393,  
      in.origin = c(month=1,day=1,year=1998)) 

## Get today's date in yyyymmdd format
timeDate(date(), in.format="\%w \%m \%d \%H:\%M:\%S \%Y",
      format="\%Y\%02m\%02d")
}
\keyword{chron}
