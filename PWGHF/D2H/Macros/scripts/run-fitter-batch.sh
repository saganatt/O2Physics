#!/bin/bash

# Run the D2H mass fitter in batch on a set of input mass histograms.
# For example, you can use it to obtain fits for different non-prompt BDT cuts.

# Directory and filename pattern of the files with mass histograms.
INPUT_DIR="/data8/majak/MLHEP/input-d2h-fitter-012025"
INFILE_PATTERN="masshistos_"
INPUT_PATTERN="${INPUT_DIR}/${INFILE_PATTERN}"

# The main directory to store all output subdirectories.
RESDIR="/data8/majak/d2h-fitter/0125"

# Base mass fitter config.
CONFIG="config_massfitter"
CONFIG_EXT="${CONFIG}.json"

# Suffix to append to all output filenames.
RES_SUFFIX="-poly2-fixed-sigma"

# Further common part of the input filenames.
# Difference between INPUT_PATTERN and PERM_PATTERN: PERM_PATTERN is included in the output filenames, while INPUT_PATTERN is not.
# For example, with the input filename "masshistos_fd_0.10.root", INPUT_PATTERN="${INPUT_DIR}/masshistos_", PERM_PATTERN="fd_",
# RES_SUFFIX="-poly-fixed-sigma", output filename is "results_fd_0.10-poly-fixed-sigma".
PERM_PATTERN="fd_"

for dir in ${INPUT_PATTERN}${PERM_PATTERN}0.[0-9][05]* ; do
  echo $dir

  suffix=${dir##${INPUT_PATTERN}}
  echo $suffix

  ROOT_EXT=".root"
  suffix_no_ext=${suffix%%${ROOT_EXT}}
  echo $suffix_no_ext

  suffix_no_ext_ext=${suffix_no_ext##${PERM_PATTERN}}
  echo "sufix no ext ext ${suffix_no_ext_ext}"

  # We split the processed filename by the '_' separator
  IFS='_' read -ra probs <<< "${suffix_no_ext_ext}"

  # If your filenames are of the form "masshistos_fd_0.10.root", and PERM_PATTERN="fd_", then probs[0] contains the 0.10 value.
  echo "fd ${probs[0]}"

  # If your filenames are of the form masshistos_bkg_0.20_0.30_fd_0.10_0.12...0.10.root, and PERM_PATTERN="bkg_",
  # then probs[0] and probs[1] have the bkg values, probs[2] contains the "fd" string, and probs[3], probs[4], ... contain the 0.10, 0.12, ..., 0.10 values.
  # echo "bkg ${probs[0]} ${probs[1]}"
  # echo "fd ${probs[3]} ${probs[4]} ${probs[5]} ${probs[6]} ${probs[7]} ${probs[8]} ${probs[9]} ${probs[10]} ${probs[11]} ${probs[12]} ${probs[13]} ${probs[14]}"

  RESPATH="${RESDIR}/results_${suffix_no_ext}${RES_SUFFIX}"
  mkdir "${RESPATH}"

 # Copy the base fitter config.
  CUR_CFG="${CONFIG}_${suffix_no_ext}_edit.json"
  cp "${CONFIG_EXT}" "${CUR_CFG}"

  # Replace the input and output names in the fitter config.
  # ${RESPATH//\//\\/} is parameter expansion with additional parameters pattern and string: ${parameter/pattern/string}
  # Pattern is /\/, starting / means that all occurences of the pattern should be replaced
  # String is \\/, i.e., \/ in the final RESPATH -- slash escaped
  sed -i "s/%indir%/${INPUT_DIR//\//\\/}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%infile%/${INFILE_PATTERN}${suffix}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%outdir%/${RESPATH//\//\\/}/g" "${CUR_CFG}" || exit 1

  # Replace the %sf...% placeholders with appropriate values in the fitter config.
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

