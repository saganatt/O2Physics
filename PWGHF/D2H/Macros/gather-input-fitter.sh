#!/bin/bash

MLHEP_DIR="/data8/majak/MLHEP"
OUTPUT_DIR="/data8/majak/MLHEP/input-d2h-fitter-012025"

RESDIR_PATTERN="${MLHEP_DIR}/results-24012025-hyp-ml-luigi-cuts_"
PERM_PATTERN="fd_"

for dir in ${RESDIR_PATTERN}${PERM_PATTERN}* ; do
  suffix=${dir##${RESDIR_PATTERN}}
  echo $suffix
  
  RESPATH="${OUTPUT_DIR}/projections_${suffix}.root"

  python gather_input_fitter.py \
    "${dir}/LHC23pp_pass4/Results/resultsdatatot/masshisto.root" \
    hmassfPt \
    ${RESPATH}
done
