"timealign" <- function(origpos, newpos, how, error.how, matchtol = 0) {
    .time_align(as(origpos, "timeDate"), as(newpos, "timeDate"),
                as(c(how, error.how), "character"), as(matchtol, "numeric")+0)
}
.timealign <- timealign
"numalign" <- function(origpos, newpos, how, error.how, matchtol = 0) {
    .num_align(as(origpos, "numeric")+0, as(newpos, "numeric")+0,
               as(c(how, error.how), "character"), as(matchtol, "numeric")+0)
}
.numalign <- numalign
