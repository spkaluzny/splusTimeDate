\name{internal}
\alias{.numalign}
\alias{.timealign}
\title{
Align Functions for Package Developers
}
\description{
Time and number alignment functions for package developers.
}
\usage{
.numalign(origpos, newpos, how, error.how, matchtol = 0)
.timealign(origpos, newpos, how, error.how, matchtol = 0)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{origpos}{
a vector of original position values.
}
  \item{newpos}{
a vector of new positions to align or interpolate it to.
}
  \item{how}{
specifies how to treat unmatched positions. Must be one of the following:
\tabular{ll}{
\code{"NA"} \tab Insert a row of \code{NA}. \cr  
\code{"drop"} \tab Drop that position entirely. \cr 
\code{"nearest"} \tab Use the row with the nearest position. \cr 
\code{"before"} \tab Use the data from the row whose position  
is just before the unmatched position. \cr 
\code{"after"} \tab Use the data from the row whose position is  
just after the unmatched position. \cr 
\code{"interp"} \tab Interpolate linearly between \code{"before"} and \code{"after"}. 
}
}
  \item{error.how}{
specifies available actions when an out of bounds error occurs. (Such an error can occur 
when \code{how} is  \code{"before"}, \code{"after"}, or \code{"interp"}). Must be one of
the following: 
\tabular{ll}{
\code{"NA"} \tab Insert a row of \code{NA}. \cr 
\code{"drop"} \tab Drop that position entirely. \cr 
\code{"nearest"} \tab Use the row with the nearest position. 
}
}
  \item{matchtol}{
the tolerance for matching positions.
}

}
\value{
an object like \code{pos}.
}
\examples{
.numalign(1:3, 2, how=NA, error=NA)
.timealign(5:7, 8, how=NA, error=NA)
}
\keyword{internal}
