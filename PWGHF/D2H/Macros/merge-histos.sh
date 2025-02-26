#!/bin/bash

python merge_histos.py -o /data8/majak/fdd-results/012025/CutVarMergedNewFits_1-24.root \
  -n hCorrYieldsPrompt -n hCorrYieldsNonPrompt -n hCorrFracPrompt -n hCorrFracNonPrompt \
  -i /data8/majak/fdd-results/012025/fdd-results-1-2/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-2-3/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-3-4-cheb/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-4-5-cheb/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-5-6-cheb/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-4-5-cheb/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-4-5-cheb/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-4-5-cheb/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-10-12-cheb/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-12-16-mix/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-16-24-mix/CutVarLc_pp13TeV_LHC23_pass4.root
