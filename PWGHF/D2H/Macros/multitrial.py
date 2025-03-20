# pylint: disable=missing-function-docstring, invalid-name
"""
file: multitrial.py
brief: Plot multitrial systematics based on multiple fit trials, one file per trial.
usage: python3 multitrial.py config_multitrial.json
author: Maja Karwowska <mkarwowska@cern.ch>, Warsaw University of Technology
"""
import argparse
import glob
import json
import re
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import MultipleLocator, AutoMinorLocator
from functools import cmp_to_key

from ROOT import (  # pylint: disable=import-error,no-name-in-module
    TFile,
    gROOT,
)


def plot_text_box(ax, text):
    ax.text(0.98, 0.97, text,
            horizontalalignment="right", verticalalignment="top",
            fontsize=40, va="top", transform=ax.transAxes,
            bbox={"edgecolor": "black", "fill": False})


def get_yields(cfg):
    filenames = sorted(glob.glob(cfg["file_pattern"]),
                       key=lambda filename: re.split("/", filename)[-2])
    yields = {}
    yields_err = {}
    trials = {}
    for pt_bin_min, pt_bin_max in zip(cfg["pt_bins_min"], cfg["pt_bins_max"]):
        yields[f"{pt_bin_min}_{pt_bin_max}"] = []
        yields_err[f"{pt_bin_min}_{pt_bin_max}"] = []
        trials[f"{pt_bin_min}_{pt_bin_max}"] = []
    for filename in filenames:
        print(f"Reading {filename}")
        with TFile.Open(filename) as fin:
            hist = fin.Get(cfg["histoname"])
            hist_sel = fin.Get(cfg["sel_histoname"])
            if hist.ClassName() != "TH1F":
                print(f"No hist in {filename}")
            if hist_sel.ClassName() != "TH1F":
                print(f"No hist sel in {filename}")
            dirname = re.split("/", filename)[4] # [-2] for D2H fitter
            trial_name = dirname.replace(cfg["dir_pattern"], "")
            for ind, (pt_bin_min, pt_bin_max) in enumerate(zip(cfg["pt_bins_min"],
                                                               cfg["pt_bins_max"])):
                if eval(cfg["selection"])(hist_sel.GetBinContent(ind + 1)) \
                        and hist.GetBinContent(ind + 1) > 1.0 :
                    yields[f"{pt_bin_min}_{pt_bin_max}"].append(hist.GetBinContent(ind + 1))
                    yields_err[f"{pt_bin_min}_{pt_bin_max}"].append(hist.GetBinError(ind + 1))
                    trials[f"{pt_bin_min}_{pt_bin_max}"].append(trial_name)
                else:
                    print(f"Rejected: {hist_sel.GetBinContent(ind + 1)} {trial_name} pt: {pt_bin_min}, {pt_bin_max}")
                    if hist.GetBinContent(ind + 1) < 1.0:
                        print(f"Yield 0")
    return yields, yields_err, trials


def plot_yields_trials(yields, yields_err, trials, cfg, pt_string, plot_pt_string):
    fig = plt.figure(figsize=(20, 15))
    ax = plt.subplot(1, 1, 1)
    ax.set_xlabel(cfg["x_axis"], fontsize=20)
    ax.set_ylabel(cfg["y_axis"], fontsize=20)
    ax.tick_params(which="both", width=2.5, direction="in")
    ax.tick_params(which="major", labelsize=20, length=15)
    ax.tick_params(which="minor", length=7)
    ax.xaxis.set_major_locator(MultipleLocator(5))
    ax.xaxis.set_minor_locator(AutoMinorLocator(5))
    ax.yaxis.set_minor_locator(AutoMinorLocator(5))
    ax.set_xlim(0, len(trials[pt_string]) - 1)
    #plt.grid(linestyle="-", linewidth=2)
    x_axis = range(len(trials[pt_string]))
    ax.errorbar(x_axis, yields[pt_string], yerr=yields_err[pt_string],
                fmt="o", c="b", elinewidth=2.5, linewidth=4.0)
    central_trial_ind = trials[pt_string].index(cfg["central_trial"])
    central_yield = yields[pt_string][central_trial_ind]
    ax.plot(x_axis, [central_yield] * len(x_axis), c="orange", linewidth=6.0)
    y_axis = np.linspace(ax.get_ylim()[0], ax.get_ylim()[1], 100)
    ax.plot([central_trial_ind] * len(y_axis), y_axis, c="m", linestyle="-", linewidth=6.0)
    plot_text_box(ax, plot_pt_string)
    fig.savefig(f'{cfg["outdir"]}/{cfg["outfile"]}_yields_trials_{pt_string}.png', bbox_inches='tight')
    plt.close()


def plot_yields_distr(yields, cfg, pt_string, plot_pt_string):
    plt.figure(figsize=(20, 15))
    ax = plt.subplot(1, 1, 1)
    ax.set_xlabel(cfg["y_axis"], fontsize=20)
    ax.tick_params(labelsize=20, length=7, width=2.5)
    ax.hist(yields[pt_string], color="b", linewidth=4.0)
    mean = np.mean(yields[pt_string])
    std_dev = np.std(yields[pt_string])
    plot_text_box(ax, f"{plot_pt_string}\n"\
                      f"mean:    {mean:.0f}\n"\
                      f"std dev: {std_dev:.2f}\n"\
                      f"#trials: {len(yields[pt_string])}")
    plt.savefig(f'{cfg["outdir"]}/{cfg["outfile"]}_distr_{pt_string}.png', bbox_inches='tight')
    plt.close()
    return mean, std_dev


def main():
    gROOT.SetBatch(True)

    parser = argparse.ArgumentParser(description="Arguments to pass")
    parser.add_argument("config", help="JSON config file")
    args = parser.parse_args()

    with open(args.config, encoding="utf8") as fil:
        cfg = json.load(fil)

        yields, yields_err, trials = get_yields(cfg)
        std_devs = []

        for pt_bin_min, pt_bin_max in zip(cfg["pt_bins_min"], cfg["pt_bins_max"]):
            plot_pt_string = f"${pt_bin_min} < p_\\mathrm{{T}}/(\\mathrm{{GeV}}/c) < {pt_bin_max}$"
            pt_string = f"{pt_bin_min}_{pt_bin_max}"
            plot_yields_trials(yields, yields_err, trials, cfg, pt_string, plot_pt_string)
            _, std_dev = plot_yields_distr(yields, cfg, pt_string, plot_pt_string)
            std_devs.append(std_dev)

            with open(f'{cfg["outdir"]}/{cfg["outfile"]}_trials_{pt_string}.txt',
                      "w", encoding="utf-8") as ftext:
                for trial in trials[pt_string]:
                    ftext.write(f"{trial}\n")

        with open(f'{cfg["outdir"]}/{cfg["outfile"]}_stddev_{pt_string}.txt',
                  "w", encoding="utf-8") as ftext:
            for std_dev in std_devs:
                ftext.write(f"{std_dev}\n")


if __name__ == "__main__":
    main()
