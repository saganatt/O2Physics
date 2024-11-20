#!/bin/bash

python merge_histos.py -o /data8/majak/systematics/230824/finalCutVarMerged_0-24.root \
  -n hCorrYieldsPrompt -n hCorrYieldsNonPrompt -n hCorrFracPrompt -n hCorrFracNonPrompt \
  -i /data8/majak/systematics/230824/run3/CutVarLc_pp13TeV_LHC24d3_default.root \
  /data8/majak/systematics/230824/run3/CutVarLc_pp13TeV_LHC24d3_default.root \
  /data8/majak/systematics/230824/run3/CutVarLc_pp13TeV_LHC24d3_default.root \
  /data8/majak/systematics/230824/run3/CutVarLc_pp13TeV_LHC24d3_default.root \
  /data8/majak/systematics/230824/run3/CutVarLc_pp13TeV_LHC24d3_default.root \
  /data8/majak/systematics/230824/run3/CutVarLc_pp13TeV_LHC24d3_default.root \
  /data8/majak/systematics/230824/run3/CutVarLc_pp13TeV_LHC24d3_default.root \
  /data8/majak/systematics/230824/run3/CutVarLc_pp13TeV_LHC24d3_default_1-24.root \
  /data8/majak/systematics/230824/run3/CutVarLc_pp13TeV_LHC24d3_default_1-24.root 
