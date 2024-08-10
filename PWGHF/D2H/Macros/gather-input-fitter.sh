#!/bin/bash

MLHEP_DIR="/data8/majak/MLHEP"
OUTPUT_DIR="/data8/majak/MLHEP/input-d2h-fitter-hyp-ml-fdd"

RESDIR_PATTERN="${MLHEP_DIR}/results-2207-hyp-ml_fd_precise_bin23_"

for dir in ${RESDIR_PATTERN}*fd_0.70* ; do
  suffix=${dir##${RESDIR_PATTERN}}
  echo $suffix
  
  RESPATH="${OUTPUT_DIR}/projections_${suffix}.root"

  python gather_input_fitter.py \
    "${dir}/LHC22pp/Results/resultsdatatot/masshisto.root" \
    hmassfPt \
    ${RESPATH}
done
