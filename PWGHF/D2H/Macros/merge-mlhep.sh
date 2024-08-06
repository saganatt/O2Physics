#!/bin/bash

MLHEP_DIR="/data8/majak/MLHEP"
OUTPUT_DIR="/data8/majak/MLHEP/input-d2h-fitter-hyp-ml-fdd-precise-merged"

FD_12=(0.26 0.28 0.30 0.32 0.34 0.36 0.38 0.40 0.42 0.44 0.46 0.48 0.50 0.52 0.55 0.58 0.60)
FD_23=(0.20 0.22 0.24 0.26 0.28 0.30 0.32 0.34 0.36 0.38 0.40 0.42 0.44 0.46 0.48 0.50 0.52)
FD_34=(0.26 0.28 0.30 0.32 0.34 0.36 0.38 0.40 0.42 0.44 0.46 0.48 0.50 0.52 0.55 0.58 0.60)
FD_45=(0.17 0.19 0.21 0.23 0.25 0.27 0.29 0.31 0.33 0.35 0.38 0.40 0.43 0.47 0.50 0.54 0.58)
FD_56=(0.12 0.14 0.16 0.18 0.20 0.22 0.24 0.26 0.27 0.30 0.32 0.34 0.36 0.38 0.41 0.45 0.49)
FD_68=(0.16 0.18 0.20 0.22 0.24 0.26 0.28 0.30 0.32 0.34 0.36 0.38 0.40 0.42 0.46 0.48 0.52)
FD_812=(0.07 0.11 0.13 0.15 0.17 0.18 0.21 0.22 0.25 0.27 0.28 0.30 0.34 0.37 0.41 0.45 0.48)
FD_1224=(0.07 0.11 0.13 0.15 0.17 0.18 0.21 0.22 0.25 0.27 0.28 0.30 0.34 0.37 0.41 0.45 0.48)

BKG_812=0.20
BKG_1224=0.30

for i in "${!FD_12[@]}" ; do
  fd12=${FD_12[i]}
  fd23=${FD_23[i]}
  fd34=${FD_34[i]}
  fd45=${FD_45[i]}
  fd56=${FD_56[i]}
  fd68=${FD_68[i]}
  fd812=${FD_812[i]}
  fd1224=${FD_1224[i]}
  
  RESULTS_23="${MLHEP_DIR}/results-2207-hyp-ml_fd_precise_bkg_${BKG_812}_${BKG_1224}_fd_${fd12}_${fd23}_${fd45}_${fd68}_${fd812}_${fd1224}"
  RESULTS_34="${MLHEP_DIR}/results-2207-hyp-ml_fd_precise_bkg_${BKG_812}_${BKG_1224}_fd_${fd12}_${fd34}_${fd56}_${fd68}_${fd812}_${fd1224}"

  OUTPUT_DIR="/data8/majak/MLHEP/input-d2h-fitter-hyp-ml-fdd-precise-merged"
  RESPATH="${OUTPUT_DIR}/projections_fdd_precise_bkg_${BKG_812}_${BKG_1224}_fd_${fd12}_${fd23}_${fd34}_${fd45}_${fd56}_${fd68}_${fd812}_${fd1224}.root"

  python merge_histomass.py \
    -n hmassfPt \
    -o ${RESPATH} \
    -i "${RESULTS_23}/LHC22pp/Results/resultsdatatot/masshisto.root" \
       "${RESULTS_23}/LHC22pp/Results/resultsdatatot/masshisto.root" \
       "${RESULTS_34}/LHC22pp/Results/resultsdatatot/masshisto.root" \
       "${RESULTS_23}/LHC22pp/Results/resultsdatatot/masshisto.root" \
       "${RESULTS_34}/LHC22pp/Results/resultsdatatot/masshisto.root" \
       "${RESULTS_23}/LHC22pp/Results/resultsdatatot/masshisto.root" \
       "${RESULTS_23}/LHC22pp/Results/resultsdatatot/masshisto.root" \
       "${RESULTS_23}/LHC22pp/Results/resultsdatatot/masshisto.root"

  OUTPUT_DIR="/data8/majak/MLHEP/input-fd_precise_2224"
  RESPATH="${OUTPUT_DIR}/efficienciesLcpKpiRun3analysis_bkg_${BKG_812}_${BKG_1224}_fd_${fd12}_${fd23}_${fd34}_${fd45}_${fd56}_${fd68}_${fd812}_${fd1224}.root"

  python merge_eff.py \
    -n eff \
    -n eff_fd \
    -o ${RESPATH} \
    -i "${RESULTS_23}/LHC22pp_mc/Results/prod_LHC24d3b/resultsmctot/efficienciesLcpKpiRun3analysis.root" \
       "${RESULTS_23}/LHC22pp_mc/Results/prod_LHC24d3b/resultsmctot/efficienciesLcpKpiRun3analysis.root" \
       "${RESULTS_34}/LHC22pp_mc/Results/prod_LHC24d3b/resultsmctot/efficienciesLcpKpiRun3analysis.root" \
       "${RESULTS_23}/LHC22pp_mc/Results/prod_LHC24d3b/resultsmctot/efficienciesLcpKpiRun3analysis.root" \
       "${RESULTS_34}/LHC22pp_mc/Results/prod_LHC24d3b/resultsmctot/efficienciesLcpKpiRun3analysis.root" \
       "${RESULTS_23}/LHC22pp_mc/Results/prod_LHC24d3b/resultsmctot/efficienciesLcpKpiRun3analysis.root" \
       "${RESULTS_23}/LHC22pp_mc/Results/prod_LHC24d3b/resultsmctot/efficienciesLcpKpiRun3analysis.root" \
       "${RESULTS_23}/LHC22pp_mc/Results/prod_LHC24d3b/resultsmctot/efficienciesLcpKpiRun3analysis.root"
done
