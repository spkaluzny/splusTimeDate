\name{holidays}
\alias{holidays}
\title{
  Holiday Generating Function 
}
\description{
Generates a collection of holidays. 
}
\usage{
holidays(years, type = "USFederal", move = FALSE)
}
\arguments{
\item{years}{
the years for which to generate holidays. 
}
\item{type}{
the names of holidays to generate. 
}
\item{move}{
a logical value. If \code{TRUE}, move the holidays to the nearest weekday.
}
}
\value{
returns a time/date object containing the specified holidays, in chronological order. 
The time of day in the returned value is midnight in the time zone
given by \code{timeDateOptions("time.zone")}.
}
\details{
This function calls the \code{holiday.xxx} functions,
where \code{xxx} takes on the value of each of the strings
in the \code{type} argument.
If these functions do not exist, an error occurs.

After calling  the \code{holiday.xxx} functions,
\code{holidays} calls \code{holiday.nearest.weekday}  
if \code{move} is \code{TRUE}. This moves the holidays so they occur 
on weekdays. \code{move} can also be given as a logical vector,
in which case each element applies to the corresponding element of \code{type}.
}
\seealso{
\code{\link{holiday.AllSaints}}, \code{\link{holiday.nearest.weekday}},
\code{\link{holiday.fixed}}.   
}
\examples{
## Generate Christmas, New Years, and Veterans day, moving Christmas 
## and New Years to the nearest weekday 
holidays(1994:2005, c("Christmas", "NewYears", "Veterans"),  
	 c(TRUE, TRUE, FALSE)) 
}
\keyword{chron}
