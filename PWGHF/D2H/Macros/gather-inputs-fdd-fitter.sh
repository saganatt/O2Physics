#!/bin/bash

MLHEP_DIR="/data8/majak/MLHEP"
OUTPUT_DIR="${MLHEP_DIR}/input-fd-precise-22082024"

RESDIR_PATTERN="/data8/majak/d2h-fitter/220824/results-hyp-ml_"

for dir in ${RESDIR_PATTERN}*-rebin+1 ; do
  suffix=${dir##${RESDIR_PATTERN}}
  echo $suffix

  cp "${dir}/fits.root" "${OUTPUT_DIR}/yields-${suffix}.root"
done
