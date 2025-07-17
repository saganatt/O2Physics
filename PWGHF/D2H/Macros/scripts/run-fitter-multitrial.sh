#!/bin/bash

# Run the D2H mass fitter in batch for all multitrial trials.
# You need a D2H mass fitter config with an %outdir% placeholder.

# Base mass config file.
PERM_PATTERN="prompt"
CONFIG="config_massfitter_${PERM_PATTERN}"
CONFIG_EXT="${CONFIG}.json"

# Base output directory to store all subdirectories with multitrial output
RESDIR="/data8/majak/d2h-fitter/0325"

# Filename of file to store trial names.
TRIAL_FILE="trials.txt"

# Suffix of the output fit file with the central trial.
# Usually, the central trial has no modification applied.
CENTRAL_TRIAL="-cheb"

# Definitions of helper variables for base trials for all pT bins.
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
MASS_MIN_WIDE2="[2.196, 2.196, 2.18, 2.17, 2.16, 2.16, 2.16, 2.14, 2.08, 2.08, 2.08, 2.08]"
MASS_MAX_WIDE2="[2.38, 2.38, 2.39, 2.40, 2.42, 2.42, 2.42, 2.44, 2.48, 2.48, 2.48, 2.48]"
MASS_MIN_NARROW2="[2.236, 2.236, 2.22, 2.21, 2.20, 2.20, 2.20, 2.18, 2.12, 2.12, 2.12, 2.12]"
MASS_MAX_NARROW2="[2.34, 2.34, 2.35, 2.36, 2.38, 2.38, 2.38, 2.40, 2.44, 2.44, 2.44, 2.44]"

# Generate all compound trial names from the base trials.
# Double-check that the trial names in generate_trials.py correspond to the trials defined above.
python generate_trials.py > ${TRIAL_FILE}

# Loop over each compound trial name.
while IFS="" read -r trial || [ -n "$trial" ] ; do
  echo "${trial}"

  RESPATH="${RESDIR}/results_${PERM_PATTERN}${CENTRAL_TRIAL}${trial}"
  mkdir "${RESPATH}"

  CUR_CFG="${CONFIG}${trial}.json"
  cp "${CONFIG_EXT}" "${CUR_CFG}"

  # Put appropriate output directory in the fitter config.
  sed -i "s/%outdir%/${RESPATH//\//\\/}/g" "${CUR_CFG}" || exit 1

  # Split the compound trial name by "-" separator into the constituent base trials.
  IFS='-' read -ra SINGLE_TRIALS <<< "${trial}"
  echo "Trial combination: "
  # Loop over each base trial.
  for i in "${!SINGLE_TRIALS[@]}" ; do
    # Don't do nothing for the empty trial name -- this will be the baseline, unmodified fit.
    if [[ ${SINGLE_TRIALS[i]} == "" ]]; then
      continue
    fi

    echo "${SINGLE_TRIALS[i]} "

    # Modify the fitter config appropriately for a given base trial.
    if [[ ${SINGLE_TRIALS[i]} == "fixed_sigma" ]]; then
      sed -i "s/\"FixSigma\":.*$/\"FixSigma\": true,/" "${CUR_CFG}" || exit 1
      sed -i "s/\"FixSigmaManual.*$/\"FixSigmaManual\": ${FIXED_SIGMA},/" "${CUR_CFG}" || exit 1
    elif [[ ${SINGLE_TRIALS[i]} == "fixed_mean" ]]; then
      sed -i "s/\"FixMean\":.*$/\"FixMean\": true,/" "${CUR_CFG}" || exit 1
      sed -i "s/\"FixMeanManual.*$/\"FixMeanManual\": ${FIXED_MEAN},/" "${CUR_CFG}" || exit 1
    elif [[ ${SINGLE_TRIALS[i]} == "min_wide" ]]; then
      sed -i "s/\"MassMin.*$/\"MassMin\": ${MASS_MIN_WIDE},/" "${CUR_CFG}" || exit 1
    elif [[ ${SINGLE_TRIALS[i]} == "max_wide" ]]; then
      sed -i "s/\"MassMax.*$/\"MassMax\": ${MASS_MAX_WIDE},/" "${CUR_CFG}" || exit 1
    elif [[ ${SINGLE_TRIALS[i]} == "min_narrow" ]]; then
      sed -i "s/\"MassMin.*$/\"MassMin\": ${MASS_MIN_NARROW},/" "${CUR_CFG}" || exit 1
    elif [[ ${SINGLE_TRIALS[i]} == "max_narrow" ]]; then
      sed -i "s/\"MassMax.*$/\"MassMax\": ${MASS_MAX_NARROW},/" "${CUR_CFG}" || exit 1
    elif [[ ${SINGLE_TRIALS[i]} == "min_wide2" ]]; then
      sed -i "s/\"MassMin.*$/\"MassMin\": ${MASS_MIN_WIDE2},/" "${CUR_CFG}" || exit 1
    elif [[ ${SINGLE_TRIALS[i]} == "max_wide2" ]]; then
      sed -i "s/\"MassMax.*$/\"MassMax\": ${MASS_MAX_WIDE2},/" "${CUR_CFG}" || exit 1
    elif [[ ${SINGLE_TRIALS[i]} == "min_narrow2" ]]; then
      sed -i "s/\"MassMin.*$/\"MassMin\": ${MASS_MIN_NARROW2},/" "${CUR_CFG}" || exit 1
    elif [[ ${SINGLE_TRIALS[i]} == "max_narrow2" ]]; then
      sed -i "s/\"MassMax.*$/\"MassMax\": ${MASS_MAX_NARROW2},/" "${CUR_CFG}" || exit 1
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
done < ${TRIAL_FILE}
