% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/sw_Vegetation.R
\name{estimate_PotNatVeg_biomass}
\alias{estimate_PotNatVeg_biomass}
\title{Adjust mean monthly biomass values of grass and shrub functional groups by
climate relationships}
\usage{
estimate_PotNatVeg_biomass(
  tr_VegBiom,
  do_adjust_phenology = FALSE,
  do_adjust_biomass = FALSE,
  fgrass_c3c4ann = c(1, 0, 0),
  MAP_mm = 450,
  ref_temp,
  target_temp
)
}
\arguments{
\item{tr_VegBiom}{A data.frame with 12 rows (one for each month) and columns
\code{X.Biomass}, \code{X.Amount.Live}, \code{X.Perc.Live}, and
\code{X.Litter} where \code{X} are for the functional groups shrubs,
\code{X = Sh}; C3-grasses, \code{X = C3}; C4-grasses, \code{X = C4}; and
annuals, \code{X = Annual} containing default input values.}

\item{do_adjust_phenology}{A logical value. If \code{TRUE} then monthly
phenology is adjusted by temperature.}

\item{do_adjust_biomass}{A logical value. If \code{TRUE} then monthly biomass
is adjusted by precipitation.}

\item{fgrass_c3c4ann}{A numeric vector of length 3. Relative contribution
[0-1] of the C3-grasses, C4-grasses, and annuals functional groups. The sum
of \code{fgrass_c3c4ann} is 1.}

\item{MAP_mm}{A numeric value. Mean annual precipitation in millimeter of the
location.}

\item{ref_temp}{A numeric vector of length 12. Reference mean monthly
temperature values in degree Celsius under which \code{x} was
determined / is valid.}

\item{target_temp}{A numeric vector of length 12. Mean monthly
temperature values in degree Celsius of a target site / condition
for which \code{x} is to be adjusted.}
}
\value{
A list with two elements \code{grass}, \code{shrub}. Each element is
  a matrix with 12 rows (one for each month) and columns \code{Biomass},
  \code{Amount.Live}, \code{Perc.Live}, and \code{Litter}.
}
\description{
Adjust mean monthly biomass values of grass and shrub functional groups by
climate relationships
}
\section{Default inputs}{
 \itemize{
  \item Shrubs are based on location \var{\sQuote{IM_USC00107648_Reynolds}}
    which resulted in a vegetation composition of 70 \% shrubs and 30 \%
    C3-grasses. Default monthly biomass values were estimated for
    MAP = 450 mm yr-1.
  \item Grasses are based on location \var{\sQuote{GP_SGSLTER}}
    (shortgrass steppe) which resulted in 12 \% shrubs, 22 \% C3-grasses,
    and 66 \% C4-grasses. Default biomass values were estimated for
    MAP = 340 mm yr-1. }
}

\references{
Bradford, J.B., Schlaepfer, D.R., Lauenroth, W.K. & Burke, I.C.
  (2014). Shifts in plant functional types have time-dependent and regionally
  variable impacts on dryland ecosystem water balance. J Ecol, 102,
  1408-1418.
}
\seealso{
Function \code{\link{adjBiom_by_ppt}} is called
  if \code{do_adjust_biomass};
  function \code{\link{adj_phenology_by_temp}} is called
  if \code{do_adjust_phenology}.
}
