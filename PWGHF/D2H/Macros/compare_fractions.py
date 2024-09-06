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
from array import array

from ROOT import (  # pylint: disable=import-error,no-name-in-module
    TCanvas,
    TFile,
    TH1F,
    TLegend,
    TPaveText,
    gROOT,
    gStyle,
    kAzure,
    kBlack,
    kBlue,
    kCyan,
    kGray,
    kGreen,
    kMagenta,
    kOrange,
    kRed,
    kTeal,
    kYellow
)

COLORS=[kBlack, kAzure-7, kRed-3, kGreen+2, kOrange-3, kMagenta+1, kBlue, kTeal+3, kGreen, kAzure+8,
        kYellow+3, kOrange-5, kMagenta+2, kBlue-6, kCyan+1, kGreen-6]
MODELS_COLORS=[kGray+1, kOrange-3, kCyan-2, kRed-9, kAzure-9]
MODELS_STYLES=[3245, 3250, 3244, 3254, 3209]


def get_alice_text(alice_text_config):
    alice_text = TPaveText(0.15, 0.72, 0.4, 0.85, "brNDC")
    alice_text.SetTextFont(42)
    alice_text.SetTextSize(0.04)
    alice_text.SetBorderSize(0)
    alice_text.SetFillStyle(0)
    alice_text.SetTextAlign(11)

    alice_text.AddText("#scale[1.35]{ALICE Preliminary}")
    #alice_text.AddText("#scale[1.05]{pp #sqrt{s} = 13.6 TeV}")
    alice_text.AddText(f"#scale[1.15]{{{alice_text_config}}}")

    return alice_text


def get_legend(x_1, y_1, x_2, y_2, num_hists):
    leg = TLegend(x_1, y_1, x_2, y_2)
    if num_hists > 4:
        leg.SetNColumns(2)
    leg.SetTextSize(0.04)
    leg.SetBorderSize(0)
    return leg

def prepare_canvas(cname):
    canv = TCanvas(cname, "")
    canv.SetCanvasSize(800, 600)
    canv.SetTickx()
    canv.SetTicky()
    canv.SetLeftMargin(0.13)
    canv.SetBottomMargin(0.13)
    return canv


def save_canvas(canv, cfg, filename):
    for ext in ("png", "pdf"):
        canv.SaveAs(os.path.join(cfg["output"]["outdir"], f"{filename}.{ext}"))


def remove_high_pt(hist):
    ind = hist.GetXaxis().FindBin(12.0)
    bins = []
    for binn in range(ind):
        bins.append(hist.GetBinLowEdge(binn + 1))
    hist2 = TH1F(hist.GetName(), hist.GetTitle(), len(bins) - 1, array('d', bins))
    for binn in range(ind):
        hist2.SetBinContent(binn + 1, hist.GetBinContent(binn + 1))
        hist2.SetBinError(binn + 1, hist.GetBinError(binn + 1))
    hist2.SetMarkerSize(hist.GetMarkerSize())
    hist2.SetMarkerStyle(hist.GetMarkerStyle())
    hist2.SetLineWidth(hist.GetLineWidth())
    hist2.SetLineStyle(hist.GetLineStyle())
    del hist
    return hist2


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


def get_hist_for_label(label, color, cfg):
    if len(cfg["hists"][label]["file"]) == 1:
        with TFile.Open(os.path.join(cfg["inputdir"], cfg["hists"][label]["file"][0])) as fin:
            hist = fin.Get(cfg["histoname"])
            hist.SetDirectory(0)
    else:
        print(f"Merging histograms for {label}")
        hist = merge_fractions(cfg["inputdir"], cfg["histoname"], cfg["hists"][label]["file"])

    if not "Run 2" in label and not "D^{0}" in label and not "13 TeV" in label:
        print(f"Removing high pts for {label}")
        hist = remove_high_pt(hist)

    hist.SetMarkerColor(color)
    hist.SetLineColor(color)
    hist.GetXaxis().SetTitle("#it{p}_{T}(GeV/#it{c})")
    hist.GetYaxis().SetTitle(cfg["y_axis"])
    #hist.GetXaxis().SetRangeUser(0, 24.5)
    #hist.GetXaxis().SetNdivisions(12)

    return hist


def get_hist_systematics(hist_syst, label, color, cfg):
    for binn in range(hist_syst.GetNbinsX()):
        syst_err = combine_syst_errors(cfg["hists"][label]["systematics"][binn],
                                           hist_syst.GetBinContent(binn + 1))
        print(f"Syst error {label} bin {binn + 1} {syst_err}")
        hist_syst.SetBinError(binn + 1, syst_err)
    hist_syst.SetMarkerColor(color)
    hist_syst.SetLineColor(color)
    #hist_syst.GetXaxis().SetRangeUser(0, 24.5)
    #hist_syst.GetXaxis().SetNdivisions(12)
    return hist_syst


def get_hist_model(label, color, style, cfg):
    with TFile.Open(os.path.join(cfg["inputdir"], cfg["models"][label]["file"])) as fin:
        hist = fin.Get(cfg["models"][label]["histoname"])
        hist.SetDirectory(0)

    print(f"Removing high pts for model {label}")
    hist = remove_high_pt(hist)

    hist.SetMarkerColor(color)
    hist.SetFillColor(color)
    hist.SetLineColor(color)
    hist.SetFillStyle(style)
    hist.SetTitle("")
    hist.GetXaxis().SetTitle("#it{p}_{T}(GeV/#it{c})")
    hist.GetYaxis().SetTitle(cfg["y_axis"])
    #hist.GetXaxis().SetRangeUser(0, 24.5)
    #hist.GetXaxis().SetNdivisions(12)

    return hist


def plot_compare(cfg):
    canv = prepare_canvas(f'c_{cfg["histoname"]}')

    maxy = 0.
    miny = 1.0

    hists_models = []
    if cfg.get("models", None):
        leg_models = get_legend(0.45, 0.16, 0.87, 0.24, len(cfg["models"]))
        leg = get_legend(0.14, 0.60, 0.50, 0.70, len(cfg["hists"]))
        for ind, (label, color, style) in \
                enumerate(zip(cfg["models"], MODELS_COLORS, MODELS_STYLES)):
            hist = get_hist_model(label, color, style, cfg)
            maxy = max(hist.GetMaximum(), maxy)
            miny = min(hist.GetMinimum(), miny)

            canv.cd()
            draw_opt = "sameE3" if ind != 0 else "E3"
            hist.Draw(draw_opt)
            leg_models.AddEntry(hist, label, "f")

            hists_models.append(hist)
    else:
        leg = get_legend(0.14, 0.68, 0.58, 0.87, len(cfg["hists"]))
        leg_models = None

    hists = {}
    hists_syst = []
    for ind, (label, color) in enumerate(zip(cfg["hists"], COLORS)):
        hist = get_hist_for_label(label, color, cfg)

        maxy = max(hist.GetMaximum(), maxy)
        miny = min(hist.GetMinimum(), miny)

        canv.cd()
        draw_opt = "same" if ind != 0 or len(hists_models) > 0 else ""
        hist.Draw(draw_opt)
        leg.AddEntry(hist, label, "p")

        hists[label] = hist

        if cfg["hists"][label].get("systematics", None):
            hist_syst = hist.Clone()
            hist_syst = get_hist_systematics(hist_syst, label, color, cfg)
            maxy = max(hist_syst.GetMaximum(), maxy)
            miny = min(hist_syst.GetMinimum(), miny)
            hist_syst.Draw("E2 same")
            hists_syst.append(hist_syst)

    margin = 0.05
    #k = 1.0 - 2 * margin
    #rangey = maxy - miny
    #miny = miny - margin / k * rangey
    #maxy = maxy + margin / k * rangey
    miny = max(miny - margin, 0)
    print(f"Hist maxy: {maxy}")
    for hist_models in hists_models:
        hist_models.GetYaxis().SetRangeUser(miny, maxy + margin)
    for _, hist in hists.items():
        hist.GetYaxis().SetRangeUser(miny, maxy + margin)
    for hist_syst in hists_syst:
        hist_syst.GetYaxis().SetRangeUser(miny, maxy + margin)

    leg.Draw()
    if len(hists_models) > 0:
        leg_models.Draw()

    alice_text = None
    if cfg.get("alice_text", None):
        alice_text = get_alice_text(cfg["alice_text"])
        alice_text.Draw("same")

    return canv, hists, leg, hists_syst, hists_models, alice_text, leg_models


def plot_ratio(cfg, hists):
    canvr = prepare_canvas(f'c_ratio_{cfg["histoname"]}')
    legr = get_legend(0.37, 0.15, 0.82, 0.31, len(cfg["hists"]))

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
            legr.AddEntry(histr, label, "p")
            histsr.append(histr)
    legr.Draw()

    return canvr, histsr, legr


def calc_systematics(cfg, hists):
    syst_errors = []
    central_hist = hists[cfg["default"]]
    for binn in range(central_hist.GetNbinsX()):
        syst_err_bin = 0.00
        for label in hists:
            if label != cfg["default"] and hists[label].GetNbinsX() == central_hist.GetNbinsX():
                syst_err = abs(hists[label].GetBinContent(binn + 1) -
                               central_hist.GetBinContent(binn + 1))
                syst_err_bin += syst_err * syst_err
        syst_err_bin = 100 * (math.sqrt(syst_err_bin) / central_hist.GetBinContent(binn + 1))
        syst_errors.append(syst_err_bin)
    str_err = "Systematic errors:"
    for err in syst_errors:
        str_err = f"{str_err} {err:0.0f}"
    print(str_err)


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

        results = plot_compare(cfg) # pylint: disable=unused-variable
        canv = results[0]
        hists = results [1]
        output.cd()
        canv.Write()
        save_canvas(canv, cfg, cfg["output"]["file"])
        for _, hist in hists.items():
            hist.Write()

        canvr, histr, _ = plot_ratio(cfg, hists)
        canvr.Write()
        save_canvas(canvr, cfg, f'{cfg["output"]["file"]}_ratio')
        for hist in histr:
            hist.Write()

        calc_systematics(cfg, hists)


if __name__ == "__main__":
    main()