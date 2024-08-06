"""
Merge MLHEP efficiency root files for the PWGHF cut variation macro. One file per pt bin.
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

    fins = [TFile(filename) for filename in args.infile]

    def get_hist(fin, histname):
        fin.cd()
        return fin.Get(histname)

    for name in args.histname:
        hist_list = [get_hist(fin, name) for fin in fins]
        hist = hist_list[0].Clone()
        for ind, hist_tmp in enumerate(hist_list):
            hist.SetBinContent(ind+1, hist_tmp.GetBinContent(ind+1))
            hist.SetBinError(ind+1, hist_tmp.GetBinError(ind+1))
        fout.cd()
        hist.Write()

    fout.Close()

if __name__ == "__main__":
    main()
