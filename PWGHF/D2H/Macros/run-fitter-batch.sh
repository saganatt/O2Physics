#!/bin/bash

INPUT_DIR="/data8/majak/MLHEP/input-d2h-fitter-10092024"
INPUT_PATTERN="${INPUT_DIR}/projections_"

CONFIG="config_massfitter"
CONFIG_EXT="${CONFIG}.json"
PERM_PATTERN="1224_split_widerange_"

RES_SUFFIX="-fixed-sigma"

for dir in ${INPUT_PATTERN}${PERM_PATTERN}* ; do
  echo $dir

  suffix=${dir##${INPUT_PATTERN}}
  echo $suffix

  ROOT_EXT=".root"
  suffix_no_ext=${suffix%%${ROOT_EXT}}
  echo $suffix_no_ext

  suffix_no_ext_ext=${suffix_no_ext##${PERM_PATTERN}}
  echo "sufix no ext ext ${suffix_no_ext_ext}"

  IFS='_' read -ra probs <<< "${suffix_no_ext_ext}"
  echo "bkg ${probs[1]} ${probs[2]}"
  echo "fd ${probs[4]} ${probs[5]} ${probs[6]} ${probs[7]} ${probs[8]} ${probs[9]} ${probs[10]} ${probs[11]}"

  RESDIR="results-hyp-ml_${suffix_no_ext}${RES_SUFFIX}"
  RESPATH="/data8/majak/d2h-fitter/100924/${RESDIR}"
  mkdir "${RESPATH}"

  CUR_CFG="${CONFIG}_${suffix_no_ext}_edit.json"
  cp "${CONFIG_EXT}" "${CUR_CFG}"

  sed -i "s/%indir%/${INPUT_DIR//\//\\/}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%infile%/projections_${suffix}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%outdir%/${RESPATH//\//\\/}/g" "${CUR_CFG}" || exit 1

  sed -i "s/%bkg1216%/${probs[1]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%bkg1624%/${probs[2]}/g" "${CUR_CFG}" || exit 1

  sed -i "s/%sf12%/${probs[4]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf23%/${probs[4]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf34%/${probs[4]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf45%/${probs[4]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf56%/${probs[4]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf68%/${probs[4]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf812%/${probs[4]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf1216%/${probs[4]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf1624%/${probs[4]}/g" "${CUR_CFG}" || exit 1

  root -b -l -q -x "HFInvMassFitter.cxx" runMassFitter.C\(\"${CUR_CFG}\"\)
done

# ${RESPATH//\//\\/} is parameter expansion with additional parameters pattern and string: ${parameter/pattern/string}
# Pattern is /\/, starting / means that all occurences of the pattern should be replaced
# String is \\/, i.e., \/ in the final RESPATH -- slash escaped
