#!/bin/bash

MLHEP_DIR="/data8/majak/MLHEP"
OUTPUT_DIR="/data8/majak/MLHEP/input-d2h-fitter"

RESDIR_PATTERN="${MLHEP_DIR}/results-bkg_0624_"

for dir in ${RESDIR_PATTERN}* ; do
  suffix=${dir##${RESDIR_PATTERN}}
  echo $suffix
  
  RESPATH="${OUTPUT_DIR}/projections_${suffix}.root"

python gather_input_fitter.py \
  "${dir}/LHC22pp/Results/resultsdatatot/masshisto.root" \
  hmassfPt \
  ${RESPATH}

done
