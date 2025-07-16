#!/bin/bash

INPUT_DIR="/data8/majak/MLHEP/input-d2h-fitter-012025"
INPUT_PATTERN="${INPUT_DIR}/projections_"

CONFIG="config_massfitter"
CONFIG_EXT="${CONFIG}.json"
PERM_PATTERN="fd_"

RES_SUFFIX="-poly2-fixed-sigma-120-190-test-base"
#RES_SUFFIX="-figure"

for dir in ${INPUT_PATTERN}${PERM_PATTERN}0.755.root ; do
#for dir in ${INPUT_PATTERN}${PERM_PATTERN}0.7[0-9][0-9]* ; do
  echo $dir

  suffix=${dir##${INPUT_PATTERN}}
  echo $suffix

  ROOT_EXT=".root"
  suffix_no_ext=${suffix%%${ROOT_EXT}}
  echo $suffix_no_ext

  suffix_no_ext_ext=${suffix_no_ext##${PERM_PATTERN}}
  echo "sufix no ext ext ${suffix_no_ext_ext}"

  IFS='_' read -ra probs <<< "${suffix_no_ext_ext}"
  echo "fd ${probs[0]}"

  #echo "bkg ${probs[1]} ${probs[2]}"
  #echo "fd ${nprobs[0]} ${nprobs[1]} ${nprobs[2]} ${nprobs[3]} ${nprobs[4]} ${nprobs[5]} ${nprobs[6]} ${nprobs[7]} ${nprobs[8]} ${nprobs[9]} ${nprobs[10]} ${nprobs[11]}"

  RESDIR="results_${suffix_no_ext}${RES_SUFFIX}"
  RESPATH="/data8/majak/d2h-fitter/0125/${RESDIR}"
  mkdir "${RESPATH}"

  CUR_CFG="${CONFIG}_${suffix_no_ext}_edit.json"
  cp "${CONFIG_EXT}" "${CUR_CFG}"

  sed -i "s/%indir%/${INPUT_DIR//\//\\/}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%infile%/projections_${suffix}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%outdir%/${RESPATH//\//\\/}/g" "${CUR_CFG}" || exit 1

  sed -i "s/%sf01%/${probs[0]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf12%/${probs[0]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf23%/${probs[0]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf34%/${probs[0]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf45%/${probs[0]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf56%/${probs[0]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf67%/${probs[0]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf78%/${probs[0]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf810%/${probs[0]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf1012%/${probs[0]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf1216%/${probs[0]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf1624%/${probs[0]}/g" "${CUR_CFG}" || exit 1

  root -b -l -q -x "HFInvMassFitter.cxx" runMassFitter.C\(\"${CUR_CFG}\"\)
done

# ${RESPATH//\//\\/} is parameter expansion with additional parameters pattern and string: ${parameter/pattern/string}
# Pattern is /\/, starting / means that all occurences of the pattern should be replaced
# String is \\/, i.e., \/ in the final RESPATH -- slash escaped
