#!/bin/bash

MLHEP_DIR="/data8/majak/MLHEP"
OUTPUT_DIR="${MLHEP_DIR}/input-fd-23082024"

RESDIR_PATTERN="/data8/majak/d2h-fitter/230824/results-hyp-ml_"
PERM_PATTERN="fixed-sigma"

for dir in ${RESDIR_PATTERN}fd_precise_pt-weight*${PERM_PATTERN} ; do
  suffix=${dir##${RESDIR_PATTERN}}
  echo $suffix

  cp "${dir}/fits.root" "${OUTPUT_DIR}/yields-${suffix}.root"
done
