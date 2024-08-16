#!/bin/bash

MLHEP_DIR="/data8/majak/MLHEP"
OUTPUT_DIR="${MLHEP_DIR}/input-fd_batch_0824/d2h-fitter"

RESDIR_PATTERN="/data8/majak/d2h-fitter/140824/results-hyp-ml_"

for dir in ${RESDIR_PATTERN}bkg_0.10_0.70*-old-mlhep-range-sigma155 ; do
  suffix=${dir##${RESDIR_PATTERN}}
  echo $suffix

  cp "${dir}/fits.root" "${OUTPUT_DIR}/yields-fd_${suffix}.root"
done
