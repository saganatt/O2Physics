#!/bin/bash

PERM_PATTERN="prompt"

INPUT_DIR="/data8/majak/MLHEP/input-d2h-fitter-012025"
INPUT_PATTERN="${INPUT_DIR}/projections-"
INFILE="projections-${PERM_PATTERN}.root"
dir="${INPUT_DIR}/${INFILE}"

CONFIG="config_massfitter_${PERM_PATTERN}"
CONFIG_EXT="${CONFIG}.json"

FIXED_SIGMA="[0.0081, 0.0081, 0.0088, 0.0102, 0.0117, 0.0133, 0.0140, 0.0155, 0.0175, 0.0197, 0.0228, 0.0260]"
FIXED_SIGMA_CUTVAR="[0.0097, 0.0082, 0.0092, 0.0104, 0.0116, 0.0134, 0.0145, 0.0162, 0.0182, 0.0204, 0.0230, 0.0263]"
FIXED_SIGMA_GAUSS="[0.0080, 0.0080, 0.0087, 0.0088, 0.0098, 0.0101, 0.0116, 0.0141, 0.0145, 0.0150, 0.0135, 0.0118]"
SECOND_SIGMA_GAUSS="[0.0134, 0.0134, 0.0088, 0.0164, 0.0199, 0.0184, 0.0207, 0.0319, 0.0313, 0.0320, 0.0320, 0.0320]"
FIXED_MEAN="[2.286, 2.284, 2.285, 2.285, 2.285, 2.285, 2.285, 2.285, 2.285, 2.284, 2.285, 2.285]"
FIXED_MEAN_PDG="[2.28646, 2.28646, 2.28646, 2.28646, 2.28646, 2.28646, 2.28646, 2.28646, 2.28646, 2.28646, 2.28646, 2.28646]"
MASS_MIN_WIDE="[2.206, 2.206, 2.19, 2.18, 2.17, 2.17, 2.17, 2.15, 2.09, 2.09, 2.09, 2.09]"
MASS_MAX_WIDE="[2.37, 2.37, 2.38, 2.39, 2.41, 2.41, 2.41, 2.43, 2.47, 2.47, 2.47, 2.47]"
MASS_MIN_NARROW="[2.226, 2.226, 2.21, 2.20, 2.19, 2.19, 2.19, 2.17, 2.11, 2.11, 2.11, 2.11]"
MASS_MAX_NARROW="[2.35, 2.35, 2.36, 2.37, 2.39, 2.39, 2.39, 2.41, 2.45, 2.45, 2.45, 2.45]"
GAUSS_SIGNAL="[1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]"
POLY3_BKG="[5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5]"

CENTRAL_TRIAL="-cheb"

TRIALS=(
  "-fixed_sigma"
  "-fixed_mean"
  "-narrow_range"
  "-wide_range"
  "-double_gauss"
  "-poly3"
  "-fixed_sigma-fixed_mean"
  "-fixed_sigma-narrow_range"
  "-fixed_sigma-wide_range"
  "-fixed_sigma-double_gauss"
  "-fixed_sigma-poly3"
  "-fixed_mean-narrow_range"
  "-fixed_mean-wide_range"
  "-fixed_mean-double_gauss"
  "-fixed_mean-poly3"
  "-narrow_range-double_gauss"
  "-narrow_range-poly3"
  "-wide_range-double_gauss"
  "-wide_range-poly3"
  "-double_gauss-poly3"
  "-fixed_sigma-fixed_mean-narrow_range"
  "-fixed_sigma-fixed_mean-wide_range"
  "-fixed_sigma-fixed_mean-double_gauss"
  "-fixed_sigma-fixed_mean-poly3"
  "-fixed_sigma-narrow_range-double_gauss"
  "-fixed_sigma-narrow_range-poly3"
  "-fixed_sigma-wide_range-double_gauss"
  "-fixed_sigma-wide_range-poly3"
  "-fixed_sigma-double_gauss-poly3"
  "-fixed_mean-narrow_range-double_gauss"
  "-fixed_mean-narrow_range-poly3"
  "-fixed_mean-wide_range-double_gauss"
  "-fixed_mean-wide_range-poly3"
  "-fixed_mean-double_gauss-poly3"
  "-narrow_range-double_gauss-poly3"
  "-wide_range-double_gauss-poly3"
  "-fixed_sigma-fixed_mean-narrow_range-double_gauss"
  "-fixed_sigma-fixed_mean-narrow_range-poly3"
  "-fixed_sigma-fixed_mean-wide_range-double_gauss"
  "-fixed_sigma-fixed_mean-wide_range-poly3"
  "-fixed_sigma-fixed_mean-double_gauss-poly3"
  "-fixed_sigma-narrow_range-double_gauss-poly3"
  "-fixed_sigma-wide_range-double_gauss-poly3"
  "-fixed_mean-narrow_range-double_gauss-poly3"
  "-fixed_mean-wide_range-double_gauss-poly3"
  "-fixed_sigma-fixed_mean-narrow_range-double_gauss-poly3"
  "-fixed_sigma-fixed_mean-wide_range-double_gauss-poly3"
)

for ind_tr in "${!TRIALS[@]}" ; do
  trial=${TRIALS[ind_tr]}
  echo $trial

  RESDIR="results_${PERM_PATTERN}${CENTRAL_TRIAL}${trial}"
  RESPATH="/data8/majak/d2h-fitter/0125/${RESDIR}"
  mkdir "${RESPATH}"

  CUR_CFG="${CONFIG}${trial}.json"
  cp "${CONFIG_EXT}" "${CUR_CFG}"

  sed -i "s/%indir%/${INPUT_DIR//\//\\/}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%infile%/${INFILE}/g" "${CUR_CFG}" || exit 1
  sed -i "s/%outdir%/${RESPATH//\//\\/}/g" "${CUR_CFG}" || exit 1

  IFS='-' read -ra SINGLE_TRIALS <<< "${trial}"
  echo "Trial combination: "
  for i in "${!SINGLE_TRIALS[@]}" ; do
    if [[ ${SINGLE_TRIALS[i]} == "" ]]; then
      continue
    fi 
    
    echo "${SINGLE_TRIALS[i]} "

    if [[ ${SINGLE_TRIALS[i]} == "fixed_sigma" ]]; then
      sed -i "s/\"FixSigma\":.*$/\"FixSigma\": true,/" "${CUR_CFG}" || exit 1
      sed -i "s/\"FixSigmaManual.*$/\"FixSigmaManual\": ${FIXED_SIGMA},/" "${CUR_CFG}" || exit 1
    elif [[ ${SINGLE_TRIALS[i]} == "fixed_mean" ]]; then
      sed -i "s/\"FixMean\":.*$/\"FixMean\": true,/" "${CUR_CFG}" || exit 1
      sed -i "s/\"FixMeanManual.*$/\"FixMeanManual\": ${FIXED_MEAN},/" "${CUR_CFG}" || exit 1
    elif [[ ${SINGLE_TRIALS[i]} == "wide_range" ]]; then
      sed -i "s/\"MassMin.*$/\"MassMin\": ${MASS_MIN_WIDE},/" "${CUR_CFG}" || exit 1
      sed -i "s/\"MassMax.*$/\"MassMax\": ${MASS_MAX_WIDE},/" "${CUR_CFG}" || exit 1
    elif [[ ${SINGLE_TRIALS[i]} == "narrow_range" ]]; then
      sed -i "s/\"MassMin.*$/\"MassMin\": ${MASS_MIN_NARROW},/" "${CUR_CFG}" || exit 1
      sed -i "s/\"MassMax.*$/\"MassMax\": ${MASS_MAX_NARROW},/" "${CUR_CFG}" || exit 1
    elif [[ ${SINGLE_TRIALS[i]} == "double_gauss" ]]; then
      sed -i "s/\"SgnFunc.*$/\"SgnFunc\": ${GAUSS_SIGNAL},/" "${CUR_CFG}" || exit 1
      sed -i "s/\"FixSigma\":.*$/\"FixSigma\": true,/" "${CUR_CFG}" || exit 1
      sed -i "s/\"FixSigmaManual.*$/\"FixSigmaManual\": ${FIXED_SIGMA_GAUSS},/" "${CUR_CFG}" || exit 1
      sed -i "s/\"FixSecondSigma\":.*$/\"FixSecondSigma\": true,/" "${CUR_CFG}" || exit 1
      sed -i "s/\"FixSecondSigmaManual.*$/\"FixSecondSigmaManual\": ${SECOND_SIGMA_GAUSS},/" "${CUR_CFG}" || exit 1
    elif [[ ${SINGLE_TRIALS[i]} == "poly3" ]]; then
      sed -i "s/\"BkgFunc.*$/\"BkgFunc\": ${POLY3_BKG},/" "${CUR_CFG}" || exit 1
    fi
  
  done

  echo ""
  echo ""

  root -b -l -q -x "HFInvMassFitter.cxx" runMassFitter.C\(\"${CUR_CFG}\"\)
done
