#!/bin/bash

MLHEP_DIR="/data8/majak/MLHEP"
OUTPUT_DIR="${MLHEP_DIR}/input-fd_batch_0624/d2h-fitter-sigmas3"

RESDIR_PATTERN="/data8/majak/d2h-fitter/100624/results-fdd-refinedbkg2_sigmas_0.0100_0.0110_0.0136_0.0157_0.0190_0.0230_"

for dir in ${RESDIR_PATTERN}* ; do
  suffix=${dir##${RESDIR_PATTERN}}
  echo $suffix

  cp "${dir}/fits.root" "${OUTPUT_DIR}/yields-fd_${suffix}.root"
done
