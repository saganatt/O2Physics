#!/bin/bash

MLHEP_DIR="/data8/majak/MLHEP"
OUTPUT_DIR="/data8/majak/MLHEP/input-d2h-fitter-hyp-ml-fdd"

RESDIR_PATTERN="${MLHEP_DIR}/results-2207-hyp-ml_fd_precise_bkg_0.20"

for dir in ${RESDIR_PATTERN}* ; do
  suffix=${dir##${RESDIR_PATTERN}}
  echo $suffix
  
  RESPATH="${OUTPUT_DIR}/projections_bkg_0.20${suffix}.root"

  python gather_input_fitter.py \
    "${dir}/LHC22pp/Results/resultsdatatot/masshisto.root" \
    hmassfPt \
    ${RESPATH}
done
