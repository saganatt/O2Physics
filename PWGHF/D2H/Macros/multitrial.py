import argparse
import glob
import json
import re
import matplotlib.pyplot as plt

from ROOT import (  # pylint: disable=import-error,no-name-in-module
    TFile,
    gROOT,
)


def get_yields(cfg):
    filenames = sorted(glob.glob(cfg["file_pattern"]))
    yields = {}
    yields_err = {}
    trials = []
    for pt_bin_min, pt_bin_max in zip(cfg["pt_bins_min"], cfg["pt_bins_max"]):
        yields[f"{pt_bin_min}_{pt_bin_max}"] = []
        yields_err[f"{pt_bin_min}_{pt_bin_max}"] = []
    for filename in filenames:
        with TFile.Open(filename) as fin:
            hist = fin.Get(cfg["histoname"])
            dirname = re.search(cfg["dir_pattern"], filename).group(0)
            trial_name = re.split("_", dirname)[-1]
            trials.append(trial_name)
            for ind, (pt_bin_min, pt_bin_max) in enumerate(zip(cfg["pt_bins_min"], cfg["pt_bins_max"])):
                yields[f"{pt_bin_min}_{pt_bin_max}"].append(hist.GetBinContent(ind + 1))
                yields_err[f"{pt_bin_min}_{pt_bin_max}"].append(hist.GetBinError(ind + 1))
    print(f"final yields:\n{yields}\ntrials:\n{trials}\nyields error:\n{yields_err}")
    return yields, yields_err, trials


def plot_yields_trials(yields, yields_err, trials, cfg, pt_string):
    plt.figure(figsize=(20, 15))
    ax = plt.subplot(1, 1, 1)
    ax.set_xlabel(cfg["x_axis"])
    ax.set_ylabel(cfg["y_axis"])
    ax.tick_params(labelsize=20)
    #plt.grid(linestyle="-", linewidth=2)
    plt.errorbar(range(len(trials)), yields[pt_string], yerr=yields_err[pt_string],
                 c="b", elinewidth=2.5, linewidth=4.0)
    central_trial_ind = trials.index(cfg["central_trial"])
    central_yield = yields[pt_string][central_trial_ind]
    plt.plot(ax.get_xlim()[0], central_yield, ax.get_xlim()[1], central_yield,
             c="orange", linewidth=6.0)
    plt.plot(central_trial_ind, ax.get_ylim()[0], central_trial_ind, ax.get_ylim()[1],
             c="m", linestyle="-", linewidth=4.0)
    #ax.set_xticks(ax.get_xticks()[::50])
    plt.savefig(f'{cfg["outdir"]}/{cfg["outfile"]}_yields_trials_{pt_string}.png')


def plot_yields_distr(yields, cfg, pt_string):
    plt.figure(figsize=(20, 15))
    ax = plt.subplot(1, 1, 1)
    ax.set_xlabel(cfg["y_axis"])
    plt.hist(yields, c="b", linewidth=4.0)
    plt.savefig(f'{cfg["outdir"]}/{cfg["outfile"]}_distr_{pt_string}.png')


def main():
    gROOT.SetBatch(True)

    parser = argparse.ArgumentParser(description="Arguments to pass")
    parser.add_argument("config", help="JSON config file")
    args = parser.parse_args()

    with open(args.config, encoding="utf8") as fil:
        cfg = json.load(fil)

        yields, yields_err, trials = get_yields(cfg)

        for pt_bin_min, pt_bin_max in zip(cfg["pt_bins_min"], cfg["pt_bins_max"]):
            pt_string = f"{pt_bin_min}_{pt_bin_max}"
            plot_yields_trials(yields, yields_err, trials, cfg, pt_string)
            plot_yields_distr(yields, cfg, pt_string)

            with open(f'{cfg["outdir"]}/{cfg["outfile"]}_trials_{pt_bin_min}_{pt_bin_max}.txt', "w") as ftext:
                for trial in trials:
                    f.write(f"{trial}\n")


if __name__ == "__main__":
    main()
