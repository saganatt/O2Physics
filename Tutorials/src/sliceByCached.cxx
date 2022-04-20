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
/// \brief Example tasks for sliceByCached.
/// \author Maja Kabus mkabus@cern.ch
/// \since

#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"
#include "Framework/ASoAHelpers.h"
#include "Common/DataModel/EventSelection.h"
#include "Common/DataModel/Centrality.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;
using namespace o2::soa;

struct SliceByCachedJoined {
  void process(aod::Collisions const& collisions, aod::Tracks& tracks)
  {
    LOGF(info, "Input data Collisions %d, Tracks %d ", collisions.size(), tracks.size());

    int count = 0;
    for (auto& col : collisions) {
      auto colTracks = tracks.sliceByCached(aod::track::collisionId, col.globalIndex());
      LOGF(info, "Collision: %d, tracks: %d", col.globalIndex(), colTracks.size());
      if (count > 50)
        break;
      count++;
    }
  }
};

struct SliceByCachedFiltered {
  Filter trackFilter = aod::track::pt < 5.0f;
  using myTracks = soa::Filtered<aod::Tracks>;

  void process(aod::Collisions const& collisions, myTracks& tracks)
  {
    LOGF(info, "Input data Collisions %d, Tracks %d ", collisions.size(), tracks.size());

    int count = 0;
    for (auto& col : collisions) {
      auto colTracks = tracks.sliceByCached(aod::track::collisionId, col.globalIndex());
      LOGF(info, "Collision: %d, tracks: %d", col.globalIndex(), colTracks.size());
      if (count > 20)
        break;
      count++;
    }
  }
};

struct SliceByCachedPartitioned { // nested filtered
  Configurable<float> etalow{"etaLow", -1.0f, "lowest eta"};
  Configurable<float> etaup{"etaUp", 1.0f, "highest eta"};
  Filter etafilter = (aod::track::eta < etaup) && (aod::track::eta > etalow);

  using myTracks = soa::Filtered<aod::Tracks>;

  Configurable<float> philow{"phiLow", 1.0f, "lowest phi"};
  Configurable<float> phiup{"phiUp", 2.0f, "highest phi"};

  Partition<myTracks> leftPhi = aod::track::phi < philow;
  Partition<myTracks> midPhi = aod::track::phi >= philow && aod::track::phi < phiup;
  Partition<myTracks> rightPhi = aod::track::phi >= phiup;

  void process(aod::Collisions const& collisions, myTracks& tracks)
  {
    LOGF(info, "Input data Collisions %d, Tracks %d ", collisions.size(), tracks.size());

    int count = 0;
    for (auto& collision : collisions) {
      auto colTracksLeft = leftPhi->sliceByCached(aod::track::collisionId, collision.globalIndex());
      auto colTracksRight = rightPhi->sliceByCached(aod::track::collisionId, collision.globalIndex());
      auto colTracksMid = midPhi->sliceByCached(aod::track::collisionId, collision.globalIndex());
      LOGF(info, "Collision: %d [left phis = %d] [mid phis = %d] [right phis = %d]",
           collision.globalIndex(), colTracksLeft.size(), colTracksMid.size(), colTracksRight.size());
      for (auto& track : colTracksLeft) {
        LOGF(info, "id = %d, from collision: %d, collision: %d; eta:  %.3f < %.3f < %.3f; phi: %.3f < %.3f",
             track.collisionId(), track.collision().globalIndex(), collision.globalIndex(), (float)etalow, track.eta(), (float)etaup, track.phi(), (float)philow);
      }
      for (auto& track : colTracksMid) {
        LOGF(info, "id = %d, from collision: %d, collision: %d; eta: %.3f < %.3f < %.3f; phi: %.3f <= %.3f < %.3f",
             track.collisionId(), track.collision().globalIndex(), collision.globalIndex(), (float)etalow, track.eta(), (float)etaup, (float)philow, track.phi(), (float)phiup);
      }
      for (auto& track : colTracksRight) {
        LOGF(info, "id = %d, from collision: %d, collision: %d; eta: %.3f < %.3f < %.3f; phi: %.3f < %.3f",
             track.collisionId(), track.collision().globalIndex(), collision.globalIndex(), (float)etalow, track.eta(), (float)etaup, (float)phiup, track.phi());
      }
      if (count > 50)
        break;
      count++;
    }
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<SliceByCachedJoined>(cfgc),
    adaptAnalysisTask<SliceByCachedFiltered>(cfgc),
    adaptAnalysisTask<SliceByCachedPartitioned>(cfgc),
  };
}
