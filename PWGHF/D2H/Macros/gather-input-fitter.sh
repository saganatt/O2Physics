#!/bin/bash

MLHEP_DIR="/data8/majak/MLHEP"
OUTPUT_DIR="/data8/majak/MLHEP/input-d2h-fitter-23082024"

RESDIR_PATTERN="${MLHEP_DIR}/results-2308-hyp-ml_"
PERM_PATTERN="bkg_"

for dir in ${RESDIR_PATTERN}${PERM_PATTERN}* ; do
  suffix=${dir##${RESDIR_PATTERN}}
  echo $suffix
  
  RESPATH="${OUTPUT_DIR}/projections_${suffix}.root"

  python gather_input_fitter.py \
    "${dir}/LHC22pp/Results/resultsdatatot/masshisto.root" \
    hmassfPt \
    ${RESPATH}
done
