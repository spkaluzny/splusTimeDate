{
    library(splusTimeDate)
    TRUE
}
{
    tx01 <- timeSequence(from="1/1/2013", to="1/31/2013", by="days")
    rx01 <- range(tx01)
    rx01[1] == timeDate("1/1/2013") && rx01[2] == timeDate("1/31/2013")
}
{
    tx02 <- timeSequence(from="1/30/2013", to="2/2/2013", by="days")
    all(tx02 + 1 == timeSequence(from="1/31/2013", to="2/3/2013", by="days"))
}
{
    tx03a <- timeSequence(from="11/1/2008", to="7/1/2009", by="months")
    tx03b <- timeSequence(from="12/1/2008", to="8/1/2009", by="months")
    tx03c <- tx03b - tx03a
    tx03c[1] == as("30 days", "timeSpan") &&
        tx03c[4] == as("28 days", "timeSpan")
}
{
    # cut method on timeSeq
    td04 <- timeDate(c("02/14/2010 13:30:00.000",  "05/16/2010 13:30:00.000"))
    ts04a <- timeSeq(from=td04[1], to=td04[2], by="months")
    breakstd04 <- timeDate(c("1/1/1960", "04/10/2010", "1/1/2020"))
    cuttd04a <- cut(ts04a, breakstd04)
    all(table(cuttd04a) == c(2, 2))
}
{
    # cut method on timeSequence
    ts04b <- timeSequence(from=td04[1], to=td04[2], by="months")
    cuttd04b <- cut(ts04b, breakstd04)
    all.equal(cuttd04a, cuttd04b)
}
