"""
file: generate_trials.py
brief: Generate compound names of the multitrial trials.
usage: python generate_trials.py
author: Maja Karwowska <mkarwowska@cern.ch>, Warsaw University of Technology
"""

BASE_TRIALS = (
    ["min_narrow", "min_narrow2", "min_wide", "min_wide2"],
    ["max_narrow", "max_narrow2", "max_wide", "max_wide2"],
    ["fixed_sigma"],
    ["fixed_mean"],
    ["double_gauss"],
    ["poly3"]
)


def generate_trials(trial_classes):
    combinations = [""]
    for trial_class in trial_classes:
        class_comb = []
        for cur_comb in combinations:
            for trial in trial_class:
                class_comb.append(cur_comb + "-" + trial)
                print(f"{cur_comb}-{trial}")
        combinations.extend(class_comb)
    return combinations


combinations = generate_trials(BASE_TRIALS)
