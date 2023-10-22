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

#' Read either a TSV or feather file.
#' 
#' If the file "<namefragment>.feather" exists, read it and return the read object.
#' If the file "<namefragment>.feather" does not exist, read "<nameframement>.txt"
#' into an object of type data.table. Write the data.table to a file named 
#' "<namefragment>.feather" and return the object.
#'
#' @param namefragment basename of the file to be read
#'
#' @return a data.table
#' @export
#'
read_data <- function(namefragment)
{
  featherfile <- paste0(namefragment, ".feather")
  if (file.exists(featherfile)) return(feather::read_feather(featherfile))
  x <- data.table::fread(paste0(namefragment, ".txt"))
  feather::write_feather(x, featherfile)
  return(x)
}