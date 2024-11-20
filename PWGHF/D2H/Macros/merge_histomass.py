"""
Merge MLHEP histomass root files for the PWGHF mass fitter. One file per pt bin.
One histogram per pt bin.
"""

import argparse

from ROOT import TFile, gROOT # pylint: disable=import-error

def main():
    """
    Main
    """

    gROOT.SetBatch(True)

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("-n", "--histname", action="append", type=str,
                        help="name of histograms to merge")
    parser.add_argument("-o", "--outfile", action="append", type=str, help="Output file")
    parser.add_argument("-i", "--infile", type=str, nargs="+", help="Input file")
    args = parser.parse_args()

    if len(args.outfile) != 1:
        raise ValueError("Provide exactly 1 output file")

    fout = TFile(args.outfile[0], "RECREATE")

    for name in args.histname:
        hist_list = []
        for ind, filename in enumerate(args.infile):
            fin = TFile(filename)
            list_hists = [key.GetName() for key in fin.GetListOfKeys() \
                          if name in key.GetName()]
            hist = fin.Get(list_hists[ind])
            fout.cd()
            hist.Write()

    fout.Close()

if __name__ == "__main__":
    main()
