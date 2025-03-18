#!/bin/bash

python merge_histos.py -o /data8/majak/systematics/032025/bdt/CutVarLc_pp13TeV_LHC23_pass4_narrow_left.root \
  -n hCorrYieldsPrompt -n hCorrYieldsNonPrompt -n hCorrFracPrompt -n hCorrFracNonPrompt \
  -n hCovPromptPrompt -n hCovPromptNonPrompt -n hCovNonPromptNonPrompt \
  -i /data8/majak/fdd-results/012025/fdd-results-1-2-narrow-left/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-2-3-cheb-narrow-left/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-3-4-cheb-narrow-left/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-4-5-cheb-narrow-left/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-5-6-cheb-narrow-left/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-4-5-cheb-narrow-left/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-4-5-cheb-narrow-left/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-4-5-cheb-narrow-left/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-10-12-cheb-narrow-left/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-10-12-cheb-narrow-left/CutVarLc_pp13TeV_LHC23_pass4.root \
  -i /data8/majak/fdd-results/012025/fdd-results-16-24-cheb-narrow-left/CutVarLc_pp13TeV_LHC23_pass4.root
