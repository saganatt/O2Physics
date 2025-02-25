#!/bin/bash

python merge_histos.py -o /data8/majak/fdd-results/012025/CutVarMerged_1-24.root \
  -n hCorrYieldsPrompt -n hCorrYieldsNonPrompt -n hCorrFracPrompt -n hCorrFracNonPrompt \
  -i /data8/majak/fdd-results/012025/fdd-results-1-2-wide/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-2-3-wide/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-3-4-cheb-wide/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-4-5-cheb-wide/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-5-6-cheb-wide/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-4-5-cheb-wide/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-4-5-cheb-wide/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-4-5-cheb-wide/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-10-12-cheb-wide/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-10-12-cheb-wide/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-16-24-cheb-wide/CutVarLc_pp13TeV_LHC23_pass4.root
