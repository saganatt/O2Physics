#!/bin/bash

INPUT_DIR="/data8/majak/MLHEP/input-d2h-fitter-hyp-ml"
INPUT_PATTERN="${INPUT_DIR}/projections"

CONFIG="config_massfitter"
CONFIG_EXT="${CONFIG}.json"

for dir in ${INPUT_PATTERN}* ; do
  echo $dir

  suffix=${dir##${INPUT_PATTERN}}
  echo $suffix

  ROOT_EXT=".root"
  suffix_no_ext=${suffix%%${ROOT_EXT}}
  echo $suffix_no_ext

  IFS='_' read -ra probs <<< "${suffix_no_ext}"
  echo "${probs[0]} ${probs[1]} ${probs[2]} ${probs[3]} ${probs[4]} ${probs[5]}"

  RESDIR="results-hyp-ml${suffix_no_ext}"
  RESPATH="/data8/majak/d2h-fitter/180724/${RESDIR}"
  mkdir "${RESPATH}"

  CUR_CFG="${CONFIG}_${suffix_no_ext}_edit.json"
  cp "${CONFIG_EXT}" "${CUR_CFG}"

  sed -i "s/%indir%/${INPUT_DIR//\//\\/}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%infile%/projections${suffix}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%outdir%/${RESPATH//\//\\/}/g" "${CUR_CFG}" || exit 1

  #sed -i "s/%sf12%/${probs[0]}/g" "${CUR_CFG}" || exit 1
  #sed -i "s/%sf24%/${probs[1]}/g" "${CUR_CFG}" || exit 1
  #sed -i "s/%sf46%/${probs[2]}/g" "${CUR_CFG}" || exit 1
  #sed -i "s/%sf68%/${probs[3]}/g" "${CUR_CFG}" || exit 1
  #sed -i "s/%sf812%/${probs[4]}/g" "${CUR_CFG}" || exit 1
  #sed -i "s/%sf1224%/${probs[5]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf12%/0.00/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf24%/0.00/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf46%/0.00/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf68%/0.00/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf812%/0.00/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf1224%/0.00/g" "${CUR_CFG}" || exit 1

  root -b -l -q -x "HFInvMassFitter.cxx" runMassFitter.C\(\"${CUR_CFG}\"\)
done

# ${RESPATH//\//\\/} is parameter expansion with additional parameters pattern and string: ${parameter/pattern/string}
# Pattern is /\/, starting / means that all occurences of the pattern should be replaced
# String is \\/, i.e., \/ in the final RESPATH -- slash escaped
