# pylint: disable=missing-function-docstring
"""
file:  modify_input.py
brief: Perform adjustments on input histogram.
usage: python3 modify_input.py file.root my_histo file_out.root
author: Maja Karwowska <mkarwowska@cern.ch>, Warsaw University of Technology
"""

import argparse

from ROOT import (  # pylint: disable=import-error,no-name-in-module
    gROOT,
    TFile,
)

# 2024 values for LHC22o
MLHEP_EV_SEL = 20430386.
NORM = 47092223769.611162532
BR = 0.0623

# 2025 values for LHC23_pass4_thin
MLHEP_EV_SEL = 258442910841.
NORM = 3.0077675e+11

def main():
    """
    Main function.
    """
    gROOT.SetBatch(True)

    parser = argparse.ArgumentParser(description="Arguments to pass")
    parser.add_argument("filename", help="input file with histogram")
    parser.add_argument("histname", help="histogram name")
    parser.add_argument("outhistname", help="outhistogram name")
    parser.add_argument("outname", help="output file for the new histogram")
    args = parser.parse_args()

    with TFile(args.filename) as fin, TFile(args.outname, "recreate") as fout:
        hist = fin.Get(args.histname)
        hist2 = hist.Clone(args.outhistname)
        hist2.SetDirectory(0)
        hist2.Scale(MLHEP_EV_SEL / NORM)
        #hist.Scale(1./59400000000) # luminosity scaling, lumi in pb
        #hist.Scale(BR) # BR scaling back
        fout.cd()
        hist2.Write()


if __name__ == "__main__":
    main()
