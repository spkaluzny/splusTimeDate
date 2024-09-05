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
a vector or an object of class \code{"numericSequence"}, \code{"positionsCalendar"}, and \code{"timeSpan"}.
}
  \item{partial}{
vector of indices for partial sorting. (Non-NULL values are not implemented.)
}
  \item{na.last}{
for controlling the treatment of NAs.
If TRUE, missing values in the data are put last;
if FALSE, they are put first; if NA, they are removed.
}
  \item{decreasing}{
a logical value.
If \code{TRUE} the sort order is decreasing .
The default is \code{FALSE}.
}
  \item{method}{
a character value that specifies the method to be used,
partial matches are allowed.
The default ("auto") implies "radix"
for numeric vectors, integer vectors, logical vectors
and factors with fewer than 231231 elements.
Otherwise, it implies "shell".
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
