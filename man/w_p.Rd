% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/RcppExports.R
\name{w_p}
\alias{w_p}
\title{Weighting function according to Prelec (1998)}
\usage{
w_p(p, o, delta_l, gamma_l, delta_g, gamma_g)
}
\arguments{
\item{p}{numeric specifying the to be transformed probability.}

\item{gamma_l, gamma_g}{numeric specifying the exponential factor
(sensitivity) of the weighting function for the loss (l) and gain
(g) domain.}
}
\value{
a decision weight
}
\description{
\code{w_p} is the weighting function proposed by Tverksy & Kahneman
  (1992). It transforms probabilities into decision weights.
}
\references{
Prelec, D. (1998). The probability weighting function.
  Econometrica 66, 497–527.
}
