"""
Simple projection macro that prepares MLHEP histomass.root for the PWGHF mass fitter.
"""

import argparse
import json

from ROOT import TFile, gROOT # pylint: disable=import-error

def main():
    """
    Main
    """

    gROOT.SetBatch(True)

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("infile", type=str, help="Input file")
    parser.add_argument("histpattern", type=str, help="Pattern of path to histograms to project")
    parser.add_argument("outfile", type=str, help="Output file")
    args = parser.parse_args()

    fin = TFile(args.infile)

    fout = TFile(args.outfile, "RECREATE")

    for key in fin.GetListOfKeys():
        fin.cd()
        hist = fin.Get(key.GetName())
        if args.histpattern in hist.GetName():
            fout.cd()
            hist.Write()

    fout.Close()

if __name__ == "__main__":
    main()
