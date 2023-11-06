// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.
///
/// \brief FullTracks is a join of Tracks, TracksCov, and TracksExtra.
/// \author
/// \since

#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"
#include "Framework/ASoAHelpers.h"
#include <TH1F.h>
#include <cmath>
#include <cstdlib>

using namespace o2;
using namespace o2::soa;
using namespace o2::framework;
using namespace o2::framework::expressions;

struct DeltaEtaHistograms {
  OutputObj<TH1F> deltaEtaTwoForLoops{TH1F("deltaEtaTwoForLoops", "#Delta#eta", 40, -2, 2)};
  OutputObj<TH1F> deltaEtaFullNoIndexCheck{TH1F("deltaEtaFullNoIndexCheck", "#Delta#eta", 40, -2, 2)};
  OutputObj<TH1F> deltaEtaFull{TH1F("deltaEtaFull", "#Delta#eta", 40, -2, 2)};
  OutputObj<TH1F> deltaEtaUpper{TH1F("deltaEtaUpper", "#Delta#eta", 40, -2, 2)};
  OutputObj<TH1F> deltaEtaStrictlyUpper{TH1F("deltaEtaStrictlyUpper", "#Delta#eta", 40, -2, 2)};

  Configurable<int32_t> colMax{"colMax", 100, "collisions limit"};
  Configurable<int32_t> pairMax{"pairMax", 1000000, "pairs limit"};
  Configurable<int32_t> dfMax{"dfMax", 100, "dataframes limit"};
  Configurable<bool> randomSwap{"randomSwap", true, "swap randomly elements in a pair"};

  int countTwoLoops = 0;
  int countFull = 0;
  int countUpper = 0;
  int countStrictlyUpper = 0;

  void processTwoForLoops(aod::Collisions const& collisions, aod::FullTracks& tracks)
  {
    if (countTwoLoops >= dfMax) {
      return;
    }

    int counter = 0;
    for (auto& c : collisions) {
      if (counter >= colMax) {
        break;
      }

      Partition<aod::FullTracks> groupedTracks1 = (aod::track::collisionId == c.globalIndex()) && (aod::track::pt < 0.5f);
      Partition<aod::FullTracks> groupedTracks2 = (aod::track::collisionId == c.globalIndex()) && (aod::track::pt >= 0.5f && aod::track::pt < 1.0f);
      groupedTracks1.bindTable(tracks);
      groupedTracks2.bindTable(tracks);

      int counterT = 0;

      // LOG(info) << "Two loops collision: " << c.globalIndex() << " tracks: " << groupedTracks1.size() << ", " << groupedTracks2.size();

      for (auto& track1 : groupedTracks1) {
        if (counterT >= pairMax) {
          break;
        }
        //  In strictly upper 1st element can be max groupedTracks1.size() - 2 as strictly upper is designed for avoiding repetitions in the case of same-table iteration
        if (track1.filteredIndex() == groupedTracks1.size() - 1) {
          break;
        }
        for (auto& track2 : groupedTracks2) {
          if (track1.filteredIndex() < track2.filteredIndex()) {
            if (counterT >= pairMax) {
              break;
            }
            float deltaEta = track1.eta() - track2.eta();
            if (randomSwap && std::rand() > (RAND_MAX / 2)) {
              deltaEta = track2.eta() - track1.eta();
            }
            // LOG(info) << "Two loops filling for tracks: " << track1.globalIndex() << ", " << track2.globalIndex() << " ind: " << track1.filteredIndex() << ", " << track2.filteredIndex() << " eta: " << track1.eta() << ", " << track2.eta() << " delta: " << deltaEta;
            deltaEtaTwoForLoops->Fill(deltaEta);
            counterT++;
          }
        }
      }

      counter++;
    }
    countTwoLoops++;
  }
  PROCESS_SWITCH(DeltaEtaHistograms, processTwoForLoops, "Enable two for loops", true);

  void processFullIndexPolicyNoIndexCheck(aod::Collisions const& collisions, aod::FullTracks& tracks)
  {
    int counter = 0;
    for (auto& c : collisions) {
      if (counter >= colMax) {
        break;
      }

      // create the partition groupedTracks
      Partition<aod::FullTracks> groupedTracks1 = (aod::track::collisionId == c.globalIndex()) && (aod::track::pt < 0.5f);
      Partition<aod::FullTracks> groupedTracks2 = (aod::track::collisionId == c.globalIndex()) && (aod::track::pt >= 0.5f && aod::track::pt < 1.0f);
      groupedTracks1.bindTable(tracks);
      groupedTracks2.bindTable(tracks);

      for (auto& [track1, track2] : combinations(CombinationsFullIndexPolicy(groupedTracks1, groupedTracks2))) {
        float deltaEta = track1.eta() - track2.eta();
        deltaEtaFullNoIndexCheck->Fill(deltaEta);
      }

      counter++;
    }
  }
  PROCESS_SWITCH(DeltaEtaHistograms, processFullIndexPolicyNoIndexCheck, "Enable full index no index Check", false);

  void processFullIndexPolicy(aod::Collisions const& collisions, aod::FullTracks& tracks)
  {
    if (countFull >= dfMax) {
      return;
    }

    int counter = 0;
    for (auto& c : collisions) {
      if (counter >= colMax) {
        break;
      }

      // create the partition groupedTracks
      Partition<aod::FullTracks> groupedTracks1 = (aod::track::collisionId == c.globalIndex()) && (aod::track::pt < 0.5f);
      Partition<aod::FullTracks> groupedTracks2 = (aod::track::collisionId == c.globalIndex()) && (aod::track::pt >= 0.5f && aod::track::pt < 1.0f);
      groupedTracks1.bindTable(tracks);
      groupedTracks2.bindTable(tracks);

      int counterT = 0;

      // LOG(info) << "Full policy collision: " << c.globalIndex() << " tracks: " << groupedTracks1.size() << ", " << groupedTracks2.size();

      for (auto& [track1, track2] : combinations(CombinationsFullIndexPolicy(groupedTracks1, groupedTracks2))) {
        // In strictly upper 1st element can be max groupedTracks1.size() - 2 as strictly upper is designed for avoiding repetitions in the case of same-table iteration
        if (track1.filteredIndex() == groupedTracks1.size() - 1) {
          break;
        }
        if (track1.filteredIndex() < track2.filteredIndex()) {
          if (counterT >= pairMax) {
            break;
          }
          float deltaEta = track1.eta() - track2.eta();
          if (randomSwap && std::rand() > (RAND_MAX / 2)) {
            deltaEta = track2.eta() - track1.eta();
          }
          // LOG(info) << "Full policy filling for tracks: " << track1.globalIndex() << ", " << track2.globalIndex() << " ind: " << track1.filteredIndex() << ", " << track2.filteredIndex() << " eta: " << track1.eta() << ", " << track2.eta() << " delta: " << deltaEta;
          deltaEtaFull->Fill(deltaEta);
          counterT++;
        }
      }

      counter++;
    }
    countFull++;
  }
  PROCESS_SWITCH(DeltaEtaHistograms, processFullIndexPolicy, "Enable full index", false);

  void processUpperIndexPolicy(aod::Collisions const& collisions, aod::FullTracks& tracks)
  {
    if (countUpper >= dfMax) {
      return;
    }

    int counter = 0;
    for (auto& c : collisions) {
      if (counter >= colMax) {
        break;
      }

      // create the partition groupedTracks
      Partition<aod::FullTracks> groupedTracks1 = (aod::track::collisionId == c.globalIndex()) && (aod::track::pt < 0.5f);
      Partition<aod::FullTracks> groupedTracks2 = (aod::track::collisionId == c.globalIndex()) && (aod::track::pt >= 0.5f && aod::track::pt < 1.0f);
      groupedTracks1.bindTable(tracks);
      groupedTracks2.bindTable(tracks);

      int counterT = 0;

      // LOG(info) << "Upper policy collision: " << c.globalIndex() << " tracks: " << groupedTracks1.size() << ", " << groupedTracks2.size();

      for (auto& [track1, track2] : combinations(CombinationsUpperIndexPolicy(groupedTracks1, groupedTracks2))) {
        // In strictly upper 1st element can be max groupedTracks1.size() - 2 as strictly upper is designed for avoiding repetitions in the case of same-table iteration
        if (track1.filteredIndex() == groupedTracks1.size() - 1) {
          break;
        }
        if (track1.filteredIndex() < track2.filteredIndex()) {
          if (counterT >= pairMax) {
            break;
          }
          float deltaEta = track1.eta() - track2.eta();
          if (randomSwap && std::rand() > (RAND_MAX / 2)) {
            deltaEta = track2.eta() - track1.eta();
          }
          // LOG(info) << "Upper policy filling for tracks: " << track1.globalIndex() << ", " << track2.globalIndex() << " ind: " << track1.filteredIndex() << ", " << track2.filteredIndex() << " eta: " << track1.eta() << ", " << track2.eta() << " delta: " << deltaEta;
          deltaEtaUpper->Fill(deltaEta);
          counterT++;
        }
      }

      counter++;
    }
    countUpper++;
  }
  PROCESS_SWITCH(DeltaEtaHistograms, processUpperIndexPolicy, "Enable upper index", false);

  void processStrictlyUpper(aod::Collisions const& collisions, aod::FullTracks& tracks)
  {
    if (countStrictlyUpper >= dfMax) {
      return;
    }

    int counter = 0;
    for (auto& c : collisions) {
      if (counter >= colMax) {
        break;
      }

      // create the partition groupedTracks
      Partition<aod::FullTracks> groupedTracks1 = (aod::track::collisionId == c.globalIndex()) && (aod::track::pt < 0.5f);
      Partition<aod::FullTracks> groupedTracks2 = (aod::track::collisionId == c.globalIndex()) && (aod::track::pt >= 0.5f && aod::track::pt < 1.0f);
      groupedTracks1.bindTable(tracks);
      groupedTracks2.bindTable(tracks);

      int counterT = 0;

      // LOG(info) << "Strictly upper policy collision: " << c.globalIndex() << " tracks: " << groupedTracks1.size() << ", " << groupedTracks2.size();

      for (auto& [track1, track2] : combinations(CombinationsStrictlyUpperIndexPolicy(groupedTracks1, groupedTracks2))) {
        if (counterT >= pairMax) {
          break;
        }
        float deltaEta = track1.eta() - track2.eta();
        if (randomSwap && std::rand() > (RAND_MAX / 2)) {
          deltaEta = track2.eta() - track1.eta();
        }
        // LOG(info) << "Strictly upper policy filling for tracks: " << track1.globalIndex() << ", " << track2.globalIndex() << " ind: " << track1.filteredIndex() << ", " << track2.filteredIndex() << " eta: " << track1.eta() << ", " << track2.eta() << " delta: " << deltaEta;
        deltaEtaStrictlyUpper->Fill(deltaEta);
        counterT++;
      }

      counter++;
    }
    countStrictlyUpper++;
  }
  PROCESS_SWITCH(DeltaEtaHistograms, processStrictlyUpper, "Enable strictly upper", true);
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<DeltaEtaHistograms>(cfgc),
  };
}
