#!/bin/bash

python merge_histos.py -o /data8/majak/crosssec/202502/fractions/CutVarLc_pp13TeV_LHC23_pass4_wide.root \
  -n hCorrYieldsPrompt -n hCorrYieldsNonPrompt -n hCorrFracPrompt -n hCorrFracNonPrompt \
  -n hCovPromptPrompt -n hCovPromptNonPrompt -n hCovNonPromptNonPrompt \
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
