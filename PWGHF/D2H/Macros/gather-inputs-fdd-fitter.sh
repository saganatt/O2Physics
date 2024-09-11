#!/bin/bash

MLHEP_DIR="/data8/majak/MLHEP"
OUTPUT_DIR="${MLHEP_DIR}/input-fd-10092024"

RESDIR_PATTERN="/data8/majak/d2h-fitter/100924/results-hyp-ml_"
PERM_PATTERN="-fixed-sigma"

for dir in ${RESDIR_PATTERN}*${PERM_PATTERN} ; do
  suffix=${dir##${RESDIR_PATTERN}}
  echo $suffix

  cp "${dir}/fits.root" "${OUTPUT_DIR}/yields-${suffix}.root"
done
