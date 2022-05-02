\name{timeSpan}
\alias{timeSpan}
\title{
  Constructor Function For \code{timeSpan} Class 
}
\description{
Constructs an object of class \code{timeSpan}. 
}
\usage{
timeSpan(charvec, in.format, format, julian, ms)
}
\arguments{
\item{charvec}{
the character vector to parse. 
}
\item{in.format}{
the time span input format for parsing. Defaults to  
\code{timeDateOptions("tspan.in.format")}. 
}
\item{format}{
the output format to apply to the returned object. Defaults to  
\code{timeDateOptions("tspan.out.format")}. 
}
\item{julian}{
the integer vector of days of the time span.  Can be a non-integer 
if \code{ms} is missing, in which case the fractional part represents 
fractions of days. 
}
\item{ms}{
an integer vector of milliseconds of the time span. 
}
}
\value{
returns a \code{timeSpan} object constructed from the input. If you provide 
no arguments, returns the default (empty) \code{timeSpan} object. 
}
\details{
You can call this function with no arguments. If you supply any arguments, 
at least one of \code{charvec}, \code{julian}, or \code{ms} must be present. 
\itemize{
\item If \code{charvec} is given, the \code{in.format} is used 
to parse \code{charvec} into time spans, and the \code{julian} and 
\code{ms} arguments are ignored (with a warning to the user if they are present).   
\item If \code{format} is given, it is put into the \code{format} slot of the output. 
\item If \code{julian} and/or \code{ms} are provided instead of \code{charvec}, these 
values are put into the \code{timeSpan} as the number of days and milliseconds, 
respectively, of the time span.  
}
}
\seealso{
\code{\linkS4class{timeSpan}}  class, \code{\link{format.timeSpan}}.   
}
\examples{
timeSpan() 
timeSpan(c( "378d 21h 04min 36s 365MS", "378 d", "1y, 13d, 21h 4MS")) 
timeSpan(julian=c(398, 399, 400), ms=c(298392, 3, 0)) 
}
\keyword{chron}
