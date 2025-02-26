#!/bin/bash

MLHEP_DIR="/data8/majak/MLHEP"
OUTPUT_DIR="/data8/majak/MLHEP/input-d2h-fitter-012025"

RESDIR_PATTERN="${MLHEP_DIR}/results-24022025-"
PERM_PATTERN="newtrain_fd_"

for dir in ${RESDIR_PATTERN}${PERM_PATTERN}0.000* ; do
  suffix=${dir##${RESDIR_PATTERN}}
  echo $suffix
  
  RESPATH="${OUTPUT_DIR}/projections-${suffix}.root"

  python gather_input_fitter.py \
    "${dir}/LHC23pp_pass4/Results/resultsdatatot/masshisto.root" \
    hmassfPt \
    ${RESPATH}
done
