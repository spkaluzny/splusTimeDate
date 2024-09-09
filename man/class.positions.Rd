\name{positions-class}
\alias{positions-class}
\alias{positionsNumeric-class}
\alias{positionsCalendar-class}
\alias{timeInterval-class}
\alias{+,numeric,positionsCalendar-method}
\alias{+,positionsCalendar,numeric-method}
\alias{-,positionsCalendar,numeric-method}
\alias{-,positionsCalendar,positionsCalendar-method}
\alias{Compare,positionsCalendar,positionsCalendar-method}
\alias{Math,positionsCalendar-method}
\alias{Math2,positionsCalendar-method}
\alias{Ops,ANY,positionsCalendar-method}
\alias{Ops,positionsCalendar,ANY-method}
\alias{Ops,positionsCalendar,positionsCalendar-method}
\alias{Summary,positionsCalendar-method}
\alias{cor,ANY,positionsCalendar-method}
\alias{cor,positionsCalendar,ANY-method}
\alias{cor,positionsCalendar,positionsCalendar-method}
\alias{cut,positionsCalendar-method}
\alias{days,positionsCalendar-method}
\alias{diff}
\alias{diff,positionsCalendar-method}
\alias{floor,positionsCalendar-method}
\alias{hms,positionsCalendar-method}
\alias{hours,positionsCalendar-method}
\alias{match,ANY,positionsCalendar-method}
\alias{match,positionsCalendar,ANY-method}
\alias{match,positionsCalendar,positionsCalendar-method}
\alias{max,positionsCalendar-method}
\alias{mdy,positionsCalendar-method}
\alias{mean,positionsCalendar-method}
\alias{median,positionsCalendar-method}
\alias{min,positionsCalendar-method}
\alias{minutes,positionsCalendar-method}
\alias{months,positionsCalendar-method}
\alias{quantile,positionsCalendar-method}
\alias{range,positionsCalendar-method}
\alias{seconds,positionsCalendar-method}
\alias{sort,positionsCalendar-method}
\alias{sort.list,positionsCalendar-method}
\alias{timeCeiling,positionsCalendar-method}
\alias{timeFloor,positionsCalendar-method}
\alias{timeTrunc,positionsCalendar-method}
\alias{trunc,positionsCalendar-method}
\alias{var,positionsCalendar-method}
\alias{wdydy,positionsCalendar-method}
\alias{weekdays,positionsCalendar-method}
\alias{yeardays,positionsCalendar-method}
\alias{years,positionsCalendar-method}
\alias{ceiling,positionsCalendar-method}
\alias{match,character,positionsCalendar-method}
\alias{match,positionsCalendar,character-method}
\alias{quarters,positionsCalendar-method}
\alias{var,ANY,positionsCalendar-method}
\alias{var,positionsCalendar,ANY-method}
\title{
  Virtual Classes for Time-Related Objects 
}
\docType{class}
\description{
The \code{positions}, \code{positionsNumeric}, and \code{positionsCalendar} classes 
are virtual classes that represent positions for \code{series} objects. 
The \code{timeInterval} class represents time intervals. 
}
\details{
\itemize{
\item The \code{positionsNumeric} class is a class union of
\code{numericSequence} and \code{numeric}, allowing representations of
numeric positions either by a sequence object or simply a vector.
\item The \code{positionsCalendar} is a \code{VIRTUAL} class and is
extended by \code{timeDate} and \code{timeSequence}.
\item The \code{positions} class is a class union of \code{positionsNumeric}
and \code{positionsCalendar}.  
\item The \code{timeInterval} class is extended
by \code{timeSpan} and \code{timeRelative}. 
}
}
\seealso{
\code{\linkS4class{timeDate}}  class and \code{\linkS4class{timeSequence}}  class.
}
\keyword{classes}
\keyword{chron}
\keyword{ts}

