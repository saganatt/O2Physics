#!/bin/bash

# Merge yields at 0.0 for prompt cross section
python merge_histos.py -o /data8/majak/crosssec/merged_yields_approvals.root \
  -i "/data8/majak/MLHEP/input-fd-23082024/yields-bkg_0.20_0.60_fd_0.00-rebin-1-fixed-sigma.root" \
  -i "/data8/majak/MLHEP/input-fd-23082024/yields-bkg_0.20_0.60_fd_0.00-rebin-1-fixed-sigma.root" \
  -i "/data8/majak/MLHEP/input-fd-23082024/yields-bkg_0.20_0.60_fd_0.00-rebin-1-fixed-sigma.root" \
  -i "/data8/majak/MLHEP/input-fd-23082024/yields-bkg_0.20_0.60_fd_0.00-rebin-1-fixed-sigma.root" \
  -i "/data8/majak/MLHEP/input-fd-23082024/yields-bkg_0.20_0.60_fd_0.00-rebin-1-fixed-sigma.root" \
  -i "/data8/majak/MLHEP/input-fd-23082024/yields-bkg_0.20_0.60_fd_0.00-rebin-1-fixed-sigma.root" \
  -i "/data8/majak/MLHEP/input-fd-23082024/yields-bkg_0.20_0.60_fd_0.00-rebin-1-fixed-sigma.root" \
  -i "/data8/majak/MLHEP/input-fd-10092024/yields-1224_split_widerange_bkg_0.60_0.60_fd_0.00-fixed-sigma.root" \
  -i "/data8/majak/MLHEP/input-fd-10092024/yields-1224_split_widerange_bkg_0.60_0.60_fd_0.00-fixed-sigma.root"
