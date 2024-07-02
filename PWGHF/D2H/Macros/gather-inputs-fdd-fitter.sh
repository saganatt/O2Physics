#!/bin/bash

MLHEP_DIR="/data8/majak/MLHEP"
OUTPUT_DIR="${MLHEP_DIR}/input-fd_precise_0624/d2h-fitter"

RESDIR_PATTERN="/data8/majak/d2h-fitter/100624/results-fdd-precise_"

for dir in ${RESDIR_PATTERN}* ; do
  suffix=${dir##${RESDIR_PATTERN}}
  echo $suffix

  cp "${dir}/fits.root" "${OUTPUT_DIR}/yields-fd_${suffix}.root"
done
