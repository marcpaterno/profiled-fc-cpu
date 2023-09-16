scale_x_log10_nice <- function(...)
{
  scale_x_log10(breaks=scales::trans_breaks("log10", function(x) 10^x),
                labels=scales::trans_format("log10", scales::math_format(10^.x)))
}

scale_y_log10_nice <- function(...)
{
  scale_y_log10(breaks=scales::trans_breaks("log10", function(x) 10^x),
                labels=scales::trans_format("log10", scales::math_format(10^.x)))
}