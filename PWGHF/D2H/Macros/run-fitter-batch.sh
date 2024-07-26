#!/bin/bash

INPUT_DIR="/data8/majak/MLHEP/input-d2h-fitter-hyp-ml-fdd-precise"
INPUT_PATTERN="${INPUT_DIR}/projections"

CONFIG="config_massfitter"
CONFIG_EXT="${CONFIG}.json"

for fd in $(seq 0.01 0.02 0.03; seq 0.07 0.02 0.13; seq 0.17 0.02 0.23; seq 0.27 0.02 0.27; seq 0.31 0.02 0.33; seq 0.37 0.02 0.43; seq 0.47 0.02 0.53; seq 0.57 0.02 0.63; seq 0.67 0.02 0.73; seq 0.77 0.02 0.83) ; do
#for dir in ${INPUT_PATTERN}* ; do
dir=( ${INPUT_PATTERN}*${fd}.root )
  echo $dir

  suffix=${dir[@]##${INPUT_PATTERN}}
  echo $suffix

  ROOT_EXT=".root"
  suffix_no_ext=${suffix%%${ROOT_EXT}}
  echo $suffix_no_ext

  IFS='_' read -ra probs <<< "${suffix_no_ext}"
  echo "bkg ${probs[0]} ${probs[1]}"
  echo "fd ${probs[3]} ${probs[4]} ${probs[5]} ${probs[6]} ${probs[7]} ${probs[8]}"

  RESDIR="results-hyp-ml${suffix_no_ext}-fixed-sigma"
  RESPATH="/data8/majak/d2h-fitter/220724/${RESDIR}"
  mkdir "${RESPATH}"

  CUR_CFG="${CONFIG}_${suffix_no_ext}_edit.json"
  cp "${CONFIG_EXT}" "${CUR_CFG}"

  sed -i "s/%indir%/${INPUT_DIR//\//\\/}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%infile%/projections${suffix}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%outdir%/${RESPATH//\//\\/}/g" "${CUR_CFG}" || exit 1

  sed -i "s/%bkg812%/${probs[0]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%bkg1224%/${probs[1]}/g" "${CUR_CFG}" || exit 1

  sed -i "s/%sf12%/${probs[3]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf24%/${probs[4]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf46%/${probs[5]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf68%/${probs[6]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf812%/${probs[7]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf1224%/${probs[8]}/g" "${CUR_CFG}" || exit 1

  root -b -l -q -x "HFInvMassFitter.cxx" runMassFitter.C\(\"${CUR_CFG}\"\)
done

# ${RESPATH//\//\\/} is parameter expansion with additional parameters pattern and string: ${parameter/pattern/string}
# Pattern is /\/, starting / means that all occurences of the pattern should be replaced
# String is \\/, i.e., \/ in the final RESPATH -- slash escaped
