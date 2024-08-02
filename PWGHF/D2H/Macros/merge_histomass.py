"""
Merge MLHEP histomass.root files for the PWGHF mass fitter. One histomass file per pt bin.
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
    parser.add_argument("histpattern", type=str, help="Pattern of path to histograms to project")
    parser.add_argument("outfile", type=str, help="Output file")
    parser.add_argument("infiles", nargs="+", help="Input files")
    args = parser.parse_args()

    fout = TFile(args.outfile, "RECREATE")

    for ind, filename in enumerate(args.infiles):
        fin = TFile(filename)
        list_hists = [key.GetName() for key in fin.GetListOfKeys() \
                      if args.histpattern in key.GetName()]
        fin.cd()
        hist = fin.Get(list_hists[ind])
        fout.cd()
        hist.Write()

    fout.Close()

if __name__ == "__main__":
    main()
