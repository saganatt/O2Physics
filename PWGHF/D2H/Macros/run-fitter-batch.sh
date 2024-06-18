#!/bin/bash

INPUT_DIR="/data8/majak/MLHEP/input-d2h-fitter"
INPUT_PATTERN="${INPUT_DIR}/projections_"

CONFIG="config_massfitter"
CONFIG_EXT="${CONFIG}.json"

for dir in ${INPUT_PATTERN}* ; do
  echo $dir

  suffix=${dir##${INPUT_PATTERN}}
  echo $suffix
  
  ROOT_EXT=".root"
  suffix_no_ext=${suffix%%${ROOT_EXT}}
  echo $suffix_no_ext

  RESDIR="/data8/majak/d2h-fitter/100624/results-probs-bkg_${suffix_no_ext}"
  mkdir "${RESDIR}"

  CUR_CFG="${CONFIG}_${suffix_no_ext}_edit.json"
  cp "${CONFIG_EXT}" "${CUR_CFG}" 
  sed -i "s/%infile%/projections_${suffix}/g" "${CUR_CFG}" || exit 1

  sed -i "s/%outdir%/results-probs-bkg_${suffix_no_ext}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf%/${suffix_no_ext}/g" "${CUR_CFG}" || exit 1

  root -b -l -q -x "HFInvMassFitter.cxx" runMassFitter.C\(\"${CUR_CFG}\"\)
done
