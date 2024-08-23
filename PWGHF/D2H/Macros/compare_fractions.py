# pylint: disable=missing-function-docstring
"""
file:  compare_fractions.py
brief: Compare non-prompt corrected fractions for the systematic uncertainties analysis.
usage: python3 compare_fractions.py config_compare_fractions.json
author: Maja Karwowska <mkarwowska@cern.ch>, Warsaw University of Technology
"""

import argparse
import json
import math
import os

from ROOT import (  # pylint: disable=import-error,no-name-in-module
    TCanvas,
    TFile,
    TH1F,
    TLegend,
    gPad,
    gROOT,
    gStyle,
    kAzure,
    kBlack,
    kBlue,
    kCyan,
    kGreen,
    kMagenta,
    kOrange,
    kRed,
    kTeal,
    kYellow
)

COLORS=[kBlack, kAzure-7, kRed+2, kGreen+2, kOrange-3, kMagenta+1, kBlue, kTeal+3, kGreen, kAzure+8,
        kYellow+3, kOrange-5, kMagenta+2, kBlue-6, kCyan+1, kGreen-6]


def prepare_canvas(cname, num_hists):
    canv = TCanvas(cname, "")
    canv.SetCanvasSize(800, 600)
    leg = TLegend(0.55, 0.12, 0.83, 0.28)
    if num_hists > 5:
        leg.SetNColumns(2)
    leg.SetTextSize(0.03)
    leg.SetBorderSize(0)
    gPad.SetTicks(1, 1)
    return canv, leg


def save_canvas(canv, cfg, filename):
    for ext in ("png", "pdf"):
        canv.SaveAs(os.path.join(cfg["output"]["outdir"], f"{filename}.{ext}"))


# FIXME: move to a separate script. This should create a new histogram with corrected binning.
def remove_high_pt(hist):
    ind = hist.GetXaxis().FindBin(8.0)
    for binn in range(ind, hist.GetNbinsX() + 1):
        hist.SetBinContent(binn, 0.0)
        hist.SetBinError(binn, 0.0)


def combine_syst_errors(syst_errors, value):
    err = 0.0
    for syst in syst_errors:
        err += syst * syst
    return math.sqrt(err) * value


def merge_fractions(inputdir, histname, filenames):
    with TFile.Open(os.path.join(inputdir, filenames[0])) as fin:
        reshist = fin.Get(histname).Clone()
        reshist.SetDirectory(0)

    maxy = miny = reshist.GetBinContent(1)

    for ind, file in enumerate(filenames[1:]):
        ind += 1
        fin = TFile.Open(os.path.join(inputdir, file))
        hist = fin.Get(histname)
        reshist.SetBinContent(ind + 1, hist.GetBinContent(ind + 1))
        reshist.SetBinError(ind + 1, hist.GetBinError(ind + 1))
        maxy = max(hist.GetBinContent(ind + 1), maxy)
        miny = min(hist.GetBinContent(ind + 1), miny)
        fin.Close()
    reshist.SetMaximum(maxy)
    reshist.SetMinimum(miny)

    return reshist


def plot_compare(cfg):
    canv, leg = prepare_canvas(f'c_{cfg["histoname"]}', len(cfg["hists"]))

    hists = {}
    hists_syst = []
    maxy = 0.
    miny = 1.0
    margin = 0.05
    for ind, (label, color) in enumerate(zip(cfg["hists"], COLORS)):
        if len(cfg["hists"][label]["file"]) == 1:
            with TFile.Open(os.path.join(cfg["inputdir"], cfg["hists"][label]["file"][0])) as fin:
                hist = fin.Get(cfg["histoname"])
                hist.SetDirectory(0)
        else:
            print(f"Merging histograms for {label}")
            hist = merge_fractions(cfg["inputdir"], cfg["histoname"], cfg["hists"][label]["file"])

        if not "Run 2" in label and not "D^{0}" in label:
            print(f"Removing high pts for {label}")
            remove_high_pt(hist)

        hist.SetMarkerColor(color)
        hist.SetLineColor(color)
        hist.GetYaxis().SetTitle("Non-prompt fraction")
        #hist.GetYaxis().SetTitle("Non-prompt #Lambda_{c} fraction")

        maxy = max(hist.GetMaximum(), maxy)
        miny = min(hist.GetMinimum(), miny)

        canv.cd()
        draw_opt = "same" if ind != 0 else ""
        hist.Draw(draw_opt)
        leg.AddEntry(hist, label, "pl")

        hists[label] = hist

        if cfg["hists"][label].get("systematics", None):
            hist_syst = hist.Clone()
            for binn in range(hist_syst.GetNbinsX()):
                syst_err = combine_syst_errors(cfg["hists"][label]["systematics"][binn],
                                               hist_syst.GetBinContent(binn + 1))
                print(f"Syst error {label} bin {binn + 1} {syst_err}")
                hist_syst.SetBinError(binn + 1, syst_err)
                hist_syst.SetMarkerColor(color)
                hist_syst.SetLineColor(color)
            maxy = max(hist_syst.GetMaximum(), maxy)
            miny = min(hist_syst.GetMinimum(), miny)
            hist_syst.Draw("E2 same")
            hists_syst.append(hist_syst)

    leg.Draw()

    k = 1.0 - 2 * margin
    rangey = maxy - miny
    #miny = miny - margin / k * rangey
    #maxy = maxy + margin / k * rangey
    for _, hist in hists.items():
        hist.GetYaxis().SetRangeUser(miny - margin, maxy + margin);
    for hist_syst in hists_syst:
        hist_syst.GetYaxis().SetRangeUser(miny - margin, maxy + margin);

    return canv, leg, hists, hists_syst


def plot_ratio(cfg, hists):
    canvr, legr = prepare_canvas(f'c_ratio_{cfg["histoname"]}', len(cfg["hists"]))

    histsr = []
    maxy = 2.0
    for ind, label in enumerate(hists):
        if label != cfg["default"]:
            histr = hists[label].Clone()
            histr.SetName(f"h_ratio_{label}")
            histr.Divide(hists[cfg["default"]])
            draw_opt = "same" if ind != 0 else ""
            histr.GetYaxis().SetTitle("Ratio")
            maxy = max(maxy, histr.GetMaximum())
            histr.SetMaximum(maxy)
            histr.SetMinimum(0.0)
            histr.Draw(draw_opt)
            legr.AddEntry(histr, label, "pl")
            histsr.append(histr)
    legr.Draw()

    return canvr, legr, histsr


def plot_diffs(cfg, hists):
    histsd = []
    canvs = []

    nbins = hists[cfg["default"]].GetNbinsX()
    if len(cfg["bin_min"]) != nbins or len(cfg["bin_max"]) != nbins:
        print("Incorrect number of bins in the configuration")
        return histsd, canvs

    for binn, (binmin, binmax) in enumerate(zip(cfg["bin_min"], cfg["bin_max"])):
        canvr, _ = prepare_canvas(f'c_rmse_{binmin}_{binmax}', nbins)
        histd = hists[cfg["default"]].Clone()
        histd = TH1F(f"h_rmse_{binmin}_{binmax}", f"Error for #it{{p}}_{{T}} [{binmin}, {binmax})",
                     100, -0.02, 0.02)
        for label in hists:
            if label != cfg["default"]:
                diff = hists[label].GetBinContent(binn + 1) -\
                        hists[cfg["default"]].GetBinContent(binn + 1)
                histd.Fill(diff)
        histd.Draw()

        histsd.append(histd)
        canvs.append(canvr)

    return canvs, histsd


def main():
    """
    Main function.
    """
    gROOT.SetBatch(True)

    gStyle.SetOptStat(0)
    gStyle.SetFrameLineWidth(2)
    gStyle.SetTitleSize(0.05, "x")
    gStyle.SetTitleSize(0.05, "y")
    gStyle.SetMarkerSize(1)
    gStyle.SetLabelOffset(0.015, "x")
    gStyle.SetLabelOffset(0.02, "y")
    gStyle.SetTitleOffset(1.2, "x")
    gStyle.SetTitleOffset(1.2, "y")

    parser = argparse.ArgumentParser(description="Arguments to pass")
    parser.add_argument("config", help="JSON config file")
    args = parser.parse_args()

    with open(args.config, encoding="utf8") as fil:
        cfg = json.load(fil)

    with TFile(os.path.join(cfg["output"]["outdir"],
               f'{cfg["output"]["file"]}.root'), "recreate") as output:

        canv, leg, hists, _ = plot_compare(cfg) # pylint: disable=unused-variable
        output.cd()
        canv.Write()
        save_canvas(canv, cfg, cfg["output"]["file"])
        for _, hist in hists.items():
            hist.Write()

        canvr, _, histr = plot_ratio(cfg, hists)
        canvr.Write()
        save_canvas(canvr, cfg, f'{cfg["output"]["file"]}_ratio')
        for hist in histr:
            hist.Write()

        canvds, histds = plot_diffs(cfg, hists)
        for canv, binmin, binmax in zip(canvds, cfg["bin_min"], cfg["bin_max"]):
            canv.Write()
            save_canvas(canv, cfg, f'{cfg["output"]["file"]}_diff_{binmin}_{binmax}')
        for hist in histds:
            hist.Write()


if __name__ == "__main__":
    main()
