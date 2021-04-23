\name{is.monthend}
\alias{is.monthend}
\title{
  The End of Month Day Information
}
\description{
Indicates whether a \code{"timeDate"} object  falls
on a month end.
}
\usage{
is.monthend(x)
}
\arguments{
\item{x}{
a \code{"timeDate"} object
}
}
\value{
returns a logical vector indicating which element of \code{x} 
is an end-of-month day.
}
\seealso{
\code{\link{holidays}},
\code{\link{weekdays}}.
}
\examples{
is.monthend(timeDate(c("1/1/1958", "1/31/1958", "2/10/1958")))
}
\keyword{programming}
\keyword{manip}
