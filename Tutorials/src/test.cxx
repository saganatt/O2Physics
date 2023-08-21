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

  bool processedLoops = false;
  bool processedStrictlyUpper = false;

  void processTwoForLoops(aod::Collisions const& collisions, aod::FullTracks& tracks)
  {
    if (processedLoops) {
      return;
    }

    int counter = 0;
    for (auto& c : collisions) {
      if (counter >= 1) {
        break;
      }

      Partition<aod::FullTracks> groupedTracks1 = (aod::track::collisionId == c.globalIndex()) && (aod::track::pt < 0.5f);
      Partition<aod::FullTracks> groupedTracks2 = (aod::track::collisionId == c.globalIndex()) && (aod::track::pt >= 0.5f && aod::track::pt < 1.0f);
      groupedTracks1.bindTable(tracks);
      groupedTracks2.bindTable(tracks);

      LOG(info) << "Two loops collision: " << c.globalIndex() << " tracks: " << groupedTracks1.size() << ", " << groupedTracks2.size();

      for (auto& track1 : groupedTracks1) {
        for (auto& track2 : groupedTracks2) {
          if (track1.index() < track2.index()) {
            float deltaEta = track1.eta() - track2.eta();
            LOG(info) << "Two loops filling for tracks: " << track1.globalIndex() << ", " << track2.globalIndex() << " ind: " << track1.index() << ", " << track2.index() << " eta: " << track1.eta() << ", " << track2.eta() << " delta: " << deltaEta;
            deltaEtaTwoForLoops->Fill(deltaEta);
          }
        }
      }

      counter++;
    }
    processedLoops = true;
  }
  PROCESS_SWITCH(DeltaEtaHistograms, processTwoForLoops, "Enable two for loops", true);

  void processFullIndexPolicyNoIndexCheck(aod::Collisions const& collisions, aod::FullTracks& tracks)
  {
    int counter = 0;
    for (auto& c : collisions) {
      if (counter >= 1) {
        break;
      }

      // create the partition groupedTracks
      Partition<aod::FullTracks> groupedTracks1 = (aod::track::collisionId == c.globalIndex()) && (aod::track::pt < 0.5f);
      Partition<aod::FullTracks> groupedTracks2 = (aod::track::collisionId == c.globalIndex()) && (aod::track::pt >= 0.5f && aod::track::pt < 1.0f);
      groupedTracks1.bindTable(tracks);
      groupedTracks2.bindTable(tracks);

      for (auto& [track1, track2] : combinations(CombinationsFullIndexPolicy(groupedTracks2, groupedTracks2))) {
        float deltaEta = track1.eta() - track2.eta();
        deltaEtaFullNoIndexCheck->Fill(deltaEta);
      }

      counter++;
    }
  }
  PROCESS_SWITCH(DeltaEtaHistograms, processFullIndexPolicyNoIndexCheck, "Enable full index no index Check", false);

  void processFullIndexPolicy(aod::Collisions const& collisions, aod::FullTracks& tracks)
  {
    int counter = 0;
    for (auto& c : collisions) {
      if (counter >= 1) {
        break;
      }

      // create the partition groupedTracks
      Partition<aod::FullTracks> groupedTracks1 = (aod::track::collisionId == c.globalIndex()) && (aod::track::pt < 0.5f);
      Partition<aod::FullTracks> groupedTracks2 = (aod::track::collisionId == c.globalIndex()) && (aod::track::pt >= 0.5f && aod::track::pt < 1.0f);
      groupedTracks1.bindTable(tracks);
      groupedTracks2.bindTable(tracks);

      LOG(info) << "Full policy collision: " << c.globalIndex() << " tracks: " << groupedTracks1.size() << ", " << groupedTracks2.size();

      for (auto& [track1, track2] : combinations(CombinationsFullIndexPolicy(groupedTracks2, groupedTracks2))) {
        if (track1.index() < track2.index()) {
          float deltaEta = track1.eta() - track2.eta();
          LOG(info) << "Full policy filling for tracks: " << track1.globalIndex() << ", " << track2.globalIndex() << " ind: " << track1.index() << ", " << track2.index() << " eta: " << track1.eta() << ", " << track2.eta() << " delta: " << deltaEta;
          deltaEtaFull->Fill(deltaEta);
        }
      }

      counter++;
    }
  }
  PROCESS_SWITCH(DeltaEtaHistograms, processFullIndexPolicy, "Enable full index", false);

  void processUpperIndexPolicy(aod::Collisions const& collisions, aod::FullTracks& tracks)
  {
    int counter = 0;
    for (auto& c : collisions) {
      if (counter >= 1) {
        break;
      }

      // create the partition groupedTracks
      Partition<aod::FullTracks> groupedTracks1 = (aod::track::collisionId == c.globalIndex()) && (aod::track::pt < 0.5f);
      Partition<aod::FullTracks> groupedTracks2 = (aod::track::collisionId == c.globalIndex()) && (aod::track::pt >= 0.5f && aod::track::pt < 1.0f);
      groupedTracks1.bindTable(tracks);
      groupedTracks2.bindTable(tracks);

      LOG(info) << "Upper policy collision: " << c.globalIndex() << " tracks: " << groupedTracks1.size() << ", " << groupedTracks2.size();

      for (auto& [track1, track2] : combinations(CombinationsUpperIndexPolicy(groupedTracks2, groupedTracks2))) {
        if (track1.index() < track2.index()) {
          float deltaEta = track1.eta() - track2.eta();
          LOG(info) << "Upper policy filling for tracks: " << track1.globalIndex() << ", " << track2.globalIndex() << " ind: " << track1.index() << ", " << track2.index() << " eta: " << track1.eta() << ", " << track2.eta() << " delta: " << deltaEta;
          deltaEtaUpper->Fill(deltaEta);
        }
      }

      counter++;
    }
  }
  PROCESS_SWITCH(DeltaEtaHistograms, processUpperIndexPolicy, "Enable upper index", false);

  void processStrictlyUpper(aod::Collisions const& collisions, aod::FullTracks& tracks)
  {
    if (processedStrictlyUpper) {
      return;
    }
    int counter = 0;
    for (auto& c : collisions) {
      if (counter >= 1) {
        break;
      }

      // create the partition groupedTracks
      Partition<aod::FullTracks> groupedTracks1 = (aod::track::collisionId == c.globalIndex()) && (aod::track::pt < 0.5f);
      Partition<aod::FullTracks> groupedTracks2 = (aod::track::collisionId == c.globalIndex()) && (aod::track::pt >= 0.5f && aod::track::pt < 1.0f);
      groupedTracks1.bindTable(tracks);
      groupedTracks2.bindTable(tracks);

      LOG(info) << "Strictly upper policy collision: " << c.globalIndex() << " tracks: " << groupedTracks1.size() << ", " << groupedTracks2.size();

      for (auto& [track1, track2] : combinations(CombinationsStrictlyUpperIndexPolicy(groupedTracks2, groupedTracks2))) {
        float deltaEta = track1.eta() - track2.eta();
        //LOG(info) << "Strictly upper policy filling for tracks: " << track1.globalIndex() << ", " << track2.globalIndex() << " ind: " << track1.index() << ", " << track2.index() << " eta: " << track1.eta() << ", " << track2.eta() << " delta: " << deltaEta;
        deltaEtaStrictlyUpper->Fill(deltaEta);
      }

      counter++;
    }
    processedStrictlyUpper = true;
  }
  PROCESS_SWITCH(DeltaEtaHistograms, processStrictlyUpper, "Enable strictly upper", true);
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<DeltaEtaHistograms>(cfgc),
  };
}
