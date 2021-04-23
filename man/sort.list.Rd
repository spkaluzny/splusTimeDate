\name{sort.list}
\alias{sort.list}
\title{
Vector of Indices That Sort Data
}
\description{
The \code{sort.list} function from the \code{base} package is made generic in
the \code{splusTimeDate} package with methods for S4 classes:
\code{"numericSequence"}, \code{"positionsCalendar"}, and \code{"timeSpan"}.
}
\usage{
sort.list(x, partial = NULL, na.last = TRUE, decreasing = FALSE,
    method)
}
\arguments{
  \item{x}{
%%     ~~Describe \code{x} here~~
}
  \item{partial}{
%%     ~~Describe \code{partial} here~~
}
  \item{na.last}{
%%     ~~Describe \code{na.last} here~~
}
  \item{decreasing}{
%%     ~~Describe \code{decreasing} here~~
}
  \item{method}{
%%     ~~Describe \code{method} here~~
}
}
\value{
an integer vector containing the indices of the sorted data elements.
That is, the first integer is the subscript of the smallest data element
(if \code{decreasing = FALSE}).
By default, the returned value has the same length as the input,
but it might be shorter if \code{na.last=NA}.
}
\keyword{internal}
