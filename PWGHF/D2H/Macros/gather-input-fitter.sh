#!/bin/bash

MLHEP_DIR="/data8/majak/MLHEP"
OUTPUT_DIR="/data8/majak/MLHEP/input-d2h-fitter-10092024"

RESDIR_PATTERN="${MLHEP_DIR}/results-2308-hyp-ml_"
PERM_PATTERN="fd_precise_1224_split_"

for dir in ${RESDIR_PATTERN}${PERM_PATTERN}*0.60_0.60* ; do
  suffix=${dir##${RESDIR_PATTERN}}
  echo $suffix
  
  RESPATH="${OUTPUT_DIR}/projections_${suffix}.root"

  python gather_input_fitter.py \
    "${dir}/LHC22pp/Results/resultsdatatot/masshisto.root" \
    hmassfPt \
    ${RESPATH}
done
