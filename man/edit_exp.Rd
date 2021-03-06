% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/RcppExports.R
\name{edit_exp}
\alias{edit_exp}
\title{Edit experiences}
\usage{
edit_exp(ss, prob, add_n = FALSE)
}
\arguments{
\item{ss}{a list of samples as generated by, e.g., \link{sampl_n}.}

\item{prob}{a problem table as produced by \link{p_arrange}.}

\item{frequency}{}
}
\value{
a problem table matching \code{prob}.
}
\description{
\code{edit_exp} converts the collected samples into a problem table of
  the same format as the original problem table. See link{p_arrange}.
}
