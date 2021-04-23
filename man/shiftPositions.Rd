\name{shiftPositions}
\alias{shiftPositions}
\title{
  Shift a Positions Object
}
\description{
Returns a \code{positions} object similar to the input but shifted in time.
}
\usage{
shiftPositions(x, k=1)
}
\arguments{
\item{x}{
a vector or \code{timeDate} object.
Missing values (\code{NA}s) are allowed.
}
\item{k}{
the number of positions the input series is to shift for the new series.
\itemize{
\item If \code{k} is a positive value, the resulting series is shifted forwards in time.
\item If \code{k} is a negative value, the resulting series is shifted backwards in time.
}
If \code{k} is a non-integer value, it is rounded to the nearest integer before the shift is applied.
}
}
\value{
returns a vector or \code{timeDate} object with
the positions shifted by \code{k} steps.
}
\seealso{
\code{\link{lag}}
}
\examples{
shiftPositions(1:10, 1)
x <- as(1:10, "timeDate")
shiftPositions(x)
}
\keyword{chron}
