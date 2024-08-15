# pylint: disable=missing-function-docstring
"""
file:  compare_fractions.py
brief: Compare non-prompt corrected fractions for the systematic uncertainties analysis.
usage: python3 compare_fractions.py config_compare_fractions.json
author: Maja Karwowska <mkarwowska@cern.ch>, Warsaw University of Technology
"""

import argparse
import json
import os

from ROOT import (  # pylint: disable=import-error,no-name-in-module
    TCanvas,
    TFile,
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
    leg = TLegend(0.65, 0.15, 0.85, 0.25)
    if num_hists > 5:
        leg.SetNColumns(2)
    leg.SetTextSize(0.03)
    leg.SetBorderSize(0)
    gPad.SetTicks(1, 1)
    return canv, leg


def save_canvas(canv, cfg, filename):
    for ext in ("png", "pdf"):
        canv.SaveAs(os.path.join(cfg["output"]["outdir"], f"{filename}.{ext}"))


def remove_high_pt(hist):
    for ind in range(2):
        hist.SetBinContent(hist.GetNbinsX() - ind, 0.0)
        hist.SetBinError(hist.GetNbinsX() - ind, 0.0)


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
    for ind, (label, color) in enumerate(zip(cfg["hists"], COLORS)):
        if len(cfg["hists"][label]) == 1:
            with TFile.Open(os.path.join(cfg["inputdir"], cfg["hists"][label][0])) as fin:
                hist = fin.Get(cfg["histoname"])
                hist.SetDirectory(0)
        else:
            print(f"Merging histograms for {label}")
            hist = merge_fractions(cfg["inputdir"], cfg["histoname"], cfg["hists"][label])
        remove_high_pt(hist)

        draw_opt = "same" if ind != 0 else ""
        hist.SetMarkerColor(color)
        hist.SetLineColor(color)
        hist.GetYaxis().SetTitle("Non-prompt #Lambda_{c} fraction")

        canv.cd()
        hist.Draw(draw_opt)
        leg.AddEntry(hist, label, "pl")

        hists[label] = hist
    leg.Draw()

    return canv, leg, hists


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

        canv, _, hists = plot_compare(cfg)
        output.cd()
        canv.Write()
        save_canvas(canv, cfg, cfg["output"]["file"])
        for histname in hists:
            hists[histname].Write()

        canvr, _, histr = plot_ratio(cfg, hists)
        canvr.Write()
        save_canvas(canvr, cfg, f'{cfg["output"]["file"]}_ratio')
        for hist in histr:
            hist.Write()


if __name__ == "__main__":
    main()
