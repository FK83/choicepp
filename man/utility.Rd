% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/RcppExports.R
\name{utility}
\alias{utility}
\title{Calculate the utility of an option}
\usage{
utility(opt, par, type)
}
\arguments{
\item{opt}{numeric vector specifying the outcomes and probabilities of an
option. The function expects a length of (number of outcomes * 2 + 1),
that is ordered according to max_loss, min_loss, max_gain, min_gain, and
whose last entry indicates the number of loss outcomes.}

\item{par}{numeric vector specifying the parameters of the CPT model. See
\link{v_wrapper} and \link{v_wrapper}.}

\item{type}{integer specifying the parameterization of the value and
weighting function. See \link{v_wrapper} and \link{v_wrapper}.}
}
\value{
a utility.
}
\description{
\code{utility} calculates the utility of an option based on a specific
  type of prospect theory. For details see \link{v_wrapper} and
  \link{w_wrapper}.
}
