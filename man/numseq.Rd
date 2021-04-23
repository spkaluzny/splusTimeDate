\name{numericSequence}
\alias{numericSequence}
\title{
  Constructor for \code{numericSequence} Class 
}
\description{
Constructs \code{numericSequence} objects. 
}
\usage{
numericSequence(from, to, by, length.)
}
\arguments{
  \item{}{
    At least three of the four arguments must be supplied, unless the 
    function is called with no arguments.
    }
  \item{from}{
    start of the sequence. 
  }
  \item{to}{
    end of the sequence. 
  }
  \item{by}{
    increment for the sequence. 
  }
  \item{length.}{
    length of the sequence; a non-negative integer. 
  }
}
\value{
returns a \code{numericSequence} object with properties given by the 
arguments, or the default \code{numericSequence} if no arguments are supplied. 
}
\seealso{
\code{\linkS4class{numericSequence}}  class.  
}
\examples{
numericSequence() 
# The following all produce a sequence running from 1 to 10: 
numericSequence(1, 10, 1) 
numericSequence(1, by = 1, length = 10) 
numericSequence(1, 10, length = 10) 
numericSequence(to = 10, by = 1, length = 10) 
}
\keyword{classes}
