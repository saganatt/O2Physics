// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright
// holders. All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.
///
/// \brief McParticles is a join of Tracks, TracksCov, and TracksExtra.
/// \author
/// \since

#include "Framework/ASoAHelpers.h"
#include "Framework/AnalysisTask.h"
#include "Framework/runDataProcessing.h"
#include <TH1F.h>
#include <cmath>
#include <cstdlib>

using namespace o2;
using namespace o2::soa;
using namespace o2::framework;
using namespace o2::framework::expressions;

struct TestMcMixed {
  // SliceCache cache;
  std::vector<double> xBins{VARIABLE_WIDTH, -0.064, -0.062, -0.060,
                            0.066, 0.068, 0.070, 0.072};
  std::vector<double> yBins{VARIABLE_WIDTH, -0.320, -0.301, -0.300,
                            0.330, 0.340, 0.350, 0.360};
  using BinningType = ColumnBinningPolicy<aod::mccollision::PosX, aod::mccollision::PosY>;
  BinningType binningOnPositions{{xBins, yBins}, true};
  //  SameKindPair<aod::McCollisions, aod::Tracks, BinningType>
  //  pair{binningOnPositions, 5, -1, &cache}; // indicates that 5 events should
  //  be mixed and under/overflow (-1) to be ignored

  OutputObj<TH1F> deltaEtaTwoForLoopsDiffPartitions{
    TH1F("deltaEtaTwoForLoopsDiffPartitions", "#Delta#eta", 40, -2, 2)};
  OutputObj<TH1F> deltaEtaTwoForLoopsSamePartition{
    TH1F("deltaEtaTwoForLoopsSamePartition", "#Delta#eta", 40, -2, 2)};
  OutputObj<TH1F> deltaEtaStrictlyUpper{
    TH1F("deltaEtaStrictlyUpper", "#Delta#eta", 40, -2, 2)};

  Configurable<int32_t> colMax{"colMax", 100, "collisions limit"};
  Configurable<int32_t> pairMax{"pairMax", 1000000, "pairs limit"};
  Configurable<int32_t> dfMax{"dfMax", 100, "dataframes limit"};
  Configurable<bool> randomSwap{"randomSwap", true,
                                "swap randomly elements in a pair"};

  int countTwoLoopsDiffPartitions = 0;
  int countTwoLoopsSamePartition = 0;
  int countStrictlyUpper = 0;

  void processTwoForLoopsDiffPartitions(aod::McCollisions const& collisions,
                                        aod::McParticles& tracks)
  {
    if (countTwoLoopsDiffPartitions >= dfMax) {
      return;
    }

    int counter = 0;
    for (auto& [collision1, collision2] : soa::selfCombinations(
           binningOnPositions, 5, -1, collisions, collisions)) {
      if (counter >= colMax) {
        break;
      }

      Partition<aod::McParticles> groupedTracks1 =
        (aod::mcparticle::mcCollisionId == collision1.globalIndex()) &&
        (aod::mcparticle::pt < 0.5f);
      Partition<aod::McParticles> groupedTracks2 =
        (aod::mcparticle::mcCollisionId == collision2.globalIndex()) &&
        (aod::mcparticle::pt >= 0.5f && aod::mcparticle::pt < 1.0f);
      groupedTracks1.bindTable(tracks);
      groupedTracks2.bindTable(tracks);

      // auto groupedTracks1 = Tracks1->sliceByCached(
      //   aod::mcparticle::mcCollisionId, collision1.globalIndex(), cache);
      // auto groupedTracks2 = Tracks2->sliceByCached(
      //   aod::mcparticle::mcCollisionId, collision2.globalIndex(), cache);

      int counterT = 0;

      LOG(debug) << "Two loops collision: " << collision1.globalIndex()
                 << collision2.globalIndex()
                 << " tracks: " << groupedTracks1.size() << ", "
                 << groupedTracks2.size();

      for (auto& track1 : groupedTracks1) {
        if (counterT >= pairMax) {
          break;
        }
        //  In strictly upper 1st element can be max groupedTracks1.size() - 2
        //  as strictly upper is designed for avoiding repetitions in the case
        //  of same-table iteration
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
            LOG(debug) << "Two loops filling for tracks: "
                       << track1.globalIndex() << ", " << track2.globalIndex()
                       << " ind: " << track1.filteredIndex() << ", "
                       << track2.filteredIndex() << " eta: " << track1.eta()
                       << ", " << track2.eta() << " delta: " << deltaEta
                       << " pt: " << track1.pt() << ", " << track2.pt()
                       << " phi: " << track1.phi() << ", " << track2.phi();
            deltaEtaTwoForLoopsDiffPartitions->Fill(deltaEta);
            counterT++;
          }
        }
      }

      counter++;
    }
    countTwoLoopsDiffPartitions++;
  }
  PROCESS_SWITCH(TestMcMixed, processTwoForLoopsDiffPartitions,
                 "Enable two for loops on different partitions", true);

  void processStrictlyUpper(aod::McCollisions const& collisions,
                            aod::McParticles& tracks)
  {
    if (countStrictlyUpper >= dfMax) {
      return;
    }

    int counter = 0;
    for (auto& [collision1, collision2] : soa::selfCombinations(
           binningOnPositions, 5, -1, collisions, collisions)) {
      if (counter >= colMax)
        break;

      // create the partition groupedTracks
      Partition<aod::McParticles> groupedTracks1 =
        (aod::mcparticle::mcCollisionId == collision1.globalIndex()) &&
        (aod::mcparticle::pt < 0.5f);
      Partition<aod::McParticles> groupedTracks2 =
        (aod::mcparticle::mcCollisionId == collision2.globalIndex()) &&
        (aod::mcparticle::pt >= 0.5f && aod::mcparticle::pt < 1.0f);
      groupedTracks1.bindTable(tracks);
      groupedTracks2.bindTable(tracks);

      // auto groupedTracks1 = Tracks1->sliceByCached(
      //   aod::mcparticle::mcCollisionId, collision1.globalIndex(), cache);
      // auto groupedTracks2 = Tracks1->sliceByCached(
      //   aod::mcparticle::mcCollisionId, collision2.globalIndex(), cache);

      int counterT = 0;

      LOG(debug) << "Strictly upper policy collision: "
                 << collision1.globalIndex() << collision2.globalIndex()
                 << " tracks: " << groupedTracks1.size() << ", "
                 << groupedTracks2.size();

      for (auto& [track1, track2] :
           combinations(CombinationsStrictlyUpperIndexPolicy(groupedTracks1,
                                                             groupedTracks2))) {
        if (counterT >= pairMax) {
          break;
        }
        float deltaEta = track1.eta() - track2.eta();
        if (randomSwap && std::rand() > (RAND_MAX / 2)) {
          deltaEta = track2.eta() - track1.eta();
        }
        LOG(debug) << "Strictly upper policy filling for tracks: "
                   << track1.globalIndex() << ", " << track2.globalIndex()
                   << " ind: " << track1.filteredIndex() << ", "
                   << track2.filteredIndex() << " eta: " << track1.eta() << ", "
                   << track2.eta() << " delta: " << deltaEta;
        deltaEtaStrictlyUpper->Fill(deltaEta);
        counterT++;
      }

      counter++;
    }
    countStrictlyUpper++;
  }
  PROCESS_SWITCH(TestMcMixed, processStrictlyUpper,
                 "Enable strictly upper", true);
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<TestMcMixed>(cfgc),
  };
}
