\name{holiday.AllSaints}
\alias{holiday.AllSaints}
\alias{holiday.Anzac}
\alias{holiday.Australia}
\alias{holiday.Bastille}
\alias{holiday.Canada}
\alias{holiday.Christmas}
\alias{holiday.Columbus}
\alias{holiday.Easter}
\alias{holiday.GoodFriday}
\alias{holiday.Independence}
\alias{holiday.Labor}
\alias{holiday.MLK}
\alias{holiday.May}
\alias{holiday.Memorial}
\alias{holiday.NewYears}
\alias{holiday.NYSE}
\alias{holiday.Presidents}
\alias{holiday.Remembrance}
\alias{holiday.StPatricks}
\alias{holiday.Thanksgiving}
\alias{holiday.Thanksgiving.Canada}
\alias{holiday.USFederal}
\alias{holiday.VE}
\alias{holiday.Veterans}
\alias{holiday.Victoria}
\title{
  Holiday Generating Functions
}
\description{
Generate specific holidays.
}
\usage{
holiday.AllSaints(years)
holiday.Anzac(years)
holiday.Australia(years)
holiday.Bastille(years)
holiday.Canada(years)
holiday.Christmas(years)
holiday.Columbus(years)
holiday.Easter(years)
holiday.GoodFriday(years)
holiday.Independence(years)
holiday.Labor(years)
holiday.MLK(years)
holiday.May(years)
holiday.Memorial(years)
holiday.NewYears(years)
holiday.NYSE(years)
holiday.Presidents(years)
holiday.Remembrance(years)
holiday.StPatricks(years)
holiday.Thanksgiving(years)
holiday.Thanksgiving.Canada(years)
holiday.USFederal(years)
holiday.VE(years)
holiday.Veterans(years)
holiday.Victoria(years)
}
\arguments{
\item{years}{
the years for which to generate holidays (for example, 1998:2005).
}
}
\value{
returns a time/date object with the given holidays. They are 
not guaranteed to be in any particular order.  

In 1971, many U.S. holidays were changed to fall on Mondays,
or the holiday(s) began more recently.  Because the \code{holiday} functions always return the modern definition for the holidays, the return values might be inaccurate for dates specified before 1971 or before the beginning date of the holidays.  
(The exception to that possible inaccuracy is the New York Stock Exchange (NYSE) holidays, which contains their stated holiday schedules as far back as 1885.)
}
\details{
\describe{
\item{holiday.AllSaints}{
All Saints Day: November 1st.
}
\item{holiday.Anzac}{
Anzac Day: April 25th.
}
\item{holiday.Australia}{
Australia Day: January 26th.
}
\item{holiday.Bastille}{
Bastille Day: July 14th.
}
\item{holiday.Canada}{
Canada Day: July 1st.
}
\item{holiday.Christmas}{
Christmas Day: December 25th.
}
\item{holiday.Columbus}{
Columbus Day: the 2nd Monday in October.
}
\item{holiday.Easter}{
Easter according to the Roman Catholic tradition.
}
\item{holiday.GoodFriday}{
Good Friday according to the Roman Catholic tradition.
}
\item{holiday.Independence}{
U.S. Independence Day: July 4th.
}
\item{holiday.Labor}{
U.S. Labor Day: the 1st Monday in September.
}
\item{holiday.MLK}{
U.S. Martin Luther King Jr. Day: the 3rd Monday in January.
}
\item{holiday.May}{
May Day, also known as Labour Day in some countries: May 1st.
}
\item{holiday.Memorial}{
U.S. Memorial Day: the last Monday in May.
}
\item{holiday.NewYears}{
New Years Day: January 1st.
}
\item{holiday.NYSE}{
New York Stock Exchange holidays, 1885-present, according to the historical and current (as of 1998) schedule, not including special-event closure days or partial-day closures.
}
\item{holiday.Presidents}{
U.S. Presidents Day: the 3rd Monday in February.
}
\item{holiday.Remembrance}{
same as holiday.Veterans.
}
\item{holiday.StPatricks}{
St. Patrick's Day: March 17th.
}
\item{holiday.Thanksgiving}{
U.S. Thanksgiving Day: the 4th Thursday in November.
}
\item{holiday.Thanksgiving.Canada}{
Canadian Thanksgiving Day: the 2nd Monday in October.
}
\item{holiday.USFederal}{
all the U.S. Federal holidays, which are New Years, MLK, Presidents, Memorial, Independence, Labor, Columbus, Veterans, Thanksgiving, and Christmas, all moved to the nearest weekday if they fall on a weekend.
}
\item{holiday.VE}{
Victory in Europe day: May 8th.
}
\item{holiday.Veterans}{
November 11th, known as Veterans Day in the U.S. and Remembrance Day in some other countries.
}
\item{holiday.Victoria}{
Canadian Victoria Day: the Monday on or preceeding the 24th of May.
}
}
}
\section{References}{
The following web sites were used to obtain information on the
dates of holidays:
  http://www.adfa.oz.au/~awm/anzacday/traditio.htm,
  http://www.effect.net.au/cuddlyk/myworld/history/ausday/ausday.html,
  http://fas.sfu.ca/canheritage/homepage/canid_hp/theme.htm,
  http://www.usis.usemb.se/Holidays/celebrate/intro.htm,
  http://www.gold.net/~cdwf/misc/easter.html,
  http://pages.citenet.net/users/ctmx1108/webcalend/web-cal-top.html
  http://www.smiley.cy.net/bdecie/Canada.html,
  http://shoga.wwa.com/~android7/holidays.htm,
  http://www.nyse.com
}
\seealso{
\code{\link{holiday.fixed}}, 
\code{\link{holiday.nearest.weekday}}, 
\code{\link{holidays}}. 
}
\examples{
holiday.Christmas(1994:2005)
}
\keyword{chron}

