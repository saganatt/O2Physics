#!/bin/bash

# Copy the D2H mass fitter fits into a common directory for the cut variation macro.

MLHEP_DIR="/data8/majak/MLHEP"
OUTPUT_DIR="${MLHEP_DIR}/input-fd-012025"

RESDIR_PATTERN="/data8/majak/d2h-fitter/0125/results_"
PERM_PATTERN="-mix-fixed-sigma-240"

for dir in ${RESDIR_PATTERN}*${PERM_PATTERN} ; do
  suffix=${dir##${RESDIR_PATTERN}}
  echo $suffix

  cp "${dir}/fits.root" "${OUTPUT_DIR}/yields-${suffix}.root"
done
