#!/bin/bash

MLHEP_DIR="/data8/majak/MLHEP"
OUTPUT_DIR="${MLHEP_DIR}/input-fd-012025"

RESDIR_PATTERN="/data8/majak/d2h-fitter/0125/results_"
PERM_PATTERN="poly-fixed-sigma"

for dir in ${RESDIR_PATTERN}fd_0.[0-9][0-9][0-9]-${PERM_PATTERN} ; do
  suffix=${dir##${RESDIR_PATTERN}}
  echo $suffix

  cp "${dir}/fits.root" "${OUTPUT_DIR}/yields-${suffix}.root"
done
