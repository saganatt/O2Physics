#!/bin/bash

INPUT_DIR="/data8/majak/MLHEP/input-d2h-fitter-012025"
INPUT_PATTERN="${INPUT_DIR}/projections-"

CONFIG="config_massfitter_prompt"
CONFIG_EXT="${CONFIG}.json"
PERM_PATTERN="prompt"

#nprobs=(0.550 0.550 0.500 0.350 0.350 0.350 0.350 0.350 0.350 0.350 0.350 0.350)
#pprobs=(0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00)
nprobs=(0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000)
pprobs=(0.40 0.40 0.25 0.25 0.25 0.25 0.25 0.25 0.25 0.25 0.25 0.25)

RES_SUFFIX="-cheb"
#RES_SUFFIX="-figure"

for dir in ${INPUT_PATTERN}${PERM_PATTERN}* ; do
  echo $dir

  suffix=${dir##${INPUT_PATTERN}}
  echo $suffix

  ROOT_EXT=".root"
  suffix_no_ext=${suffix%%${ROOT_EXT}}
  echo $suffix_no_ext

  suffix_no_ext_ext=${suffix_no_ext##${PERM_PATTERN}}
  echo "sufix no ext ext ${suffix_no_ext_ext}"

  #IFS='_' read -ra probs <<< "${suffix_no_ext_ext}"
  #echo "fd ${probs[0]}"

  #echo "bkg ${probs[1]} ${probs[2]}"
  echo "fd ${nprobs[0]} ${nprobs[1]} ${nprobs[2]} ${nprobs[3]} ${nprobs[4]} ${nprobs[5]} ${nprobs[6]} ${nprobs[7]} ${nprobs[8]} ${nprobs[9]} ${nprobs[10]} ${nprobs[11]}"

  RESDIR="results_${suffix_no_ext}${RES_SUFFIX}"
  RESPATH="/data8/majak/d2h-fitter/0125/${RESDIR}"
  mkdir "${RESPATH}"

  CUR_CFG="${CONFIG}_${suffix_no_ext}_edit.json"
  cp "${CONFIG_EXT}" "${CUR_CFG}"

  sed -i "s/%indir%/${INPUT_DIR//\//\\/}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%infile%/projections-${suffix}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%outdir%/${RESPATH//\//\\/}/g" "${CUR_CFG}" || exit 1

  sed -i "s/%sf01%/${nprobs[0]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf12%/${nprobs[1]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf23%/${nprobs[2]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf34%/${nprobs[3]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf45%/${nprobs[4]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf56%/${nprobs[5]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf67%/${nprobs[6]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf78%/${nprobs[7]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf810%/${nprobs[8]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf1012%/${nprobs[9]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf1216%/${nprobs[10]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sf1624%/${nprobs[11]}/g" "${CUR_CFG}" || exit 1

  sed -i "s/%sp01%/${pprobs[0]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sp12%/${pprobs[1]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sp23%/${pprobs[2]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sp34%/${pprobs[3]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sp45%/${pprobs[4]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sp56%/${pprobs[5]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sp67%/${pprobs[6]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sp78%/${pprobs[7]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sp810%/${pprobs[8]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sp1012%/${pprobs[9]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sp1216%/${pprobs[10]}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%sp1624%/${pprobs[11]}/g" "${CUR_CFG}" || exit 1

  root -b -l -q -x "HFInvMassFitter.cxx" runMassFitter.C\(\"${CUR_CFG}\"\)
done

# ${RESPATH//\//\\/} is parameter expansion with additional parameters pattern and string: ${parameter/pattern/string}
# Pattern is /\/, starting / means that all occurences of the pattern should be replaced
# String is \\/, i.e., \/ in the final RESPATH -- slash escaped
