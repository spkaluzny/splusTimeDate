\name{mdy}
\alias{mdy}
\alias{hms}
\alias{wdydy}
\title{
  Return Various Periods from a Time Vector 
}
\description{
Returns data frames containing various periods from a time vector as  
integers. 
}
\usage{
mdy(x) 
hms(x) 
wdydy(x)
}
\arguments{
\item{x}{
the \code{time} object to extract calendar periods from. 
}
}
\value{
returns a data frame containing the periods as integers.   
}
\details{
\itemize{
\item \code{mdy} returns a data frame with 
three columns: \itemize{
\item \code{month} (1 - 12). 
\item \code{day} (1 - 31).
\item \code{year} (for example, 1998).
}
\item \code{hms} returns a data frame with four columns:  
\itemize{
\item \code{hour} (0 - 23).
\item \code{minute} (0 - 59).
\item \code{second} (0 - 59).
\item \code{ms} (0 - 999). 
}
\item \code{wdydy} returns a data frame with three columns: 
\itemize{
\item \code{weekday} (0 - 6, with 0 meaning Sunday and 6 meaning Saturday).  
\item \code{yearday} (0 - 366).
\item \code{year} (for example, 1998). 
}
}
}
\seealso{
\code{\link{hours}},  \code{\link{days}}.   
}
\examples{
x <- timeDate(c("1/1/1998 3:05:23.4", "5/10/2005 2:15:11.234 PM")) 
mdy(x) 
hms(x) 
wdydy(x) 
}
\keyword{chron}

