#!/bin/bash

MLHEP_DIR="/data8/majak/MLHEP"
OUTPUT_DIR="${MLHEP_DIR}/input-fd_batch_0824_debug/d2h-fitter"

RESDIR_PATTERN="/data8/majak/d2h-fitter/220724/results-hyp-ml_"

for dir in ${RESDIR_PATTERN}0.25_0.30_fd_*-fixed-sigma-812-fit ; do
  suffix=${dir##${RESDIR_PATTERN}}
  echo $suffix

  cp "${dir}/fits.root" "${OUTPUT_DIR}/yields-fd_${suffix}.root"
done
