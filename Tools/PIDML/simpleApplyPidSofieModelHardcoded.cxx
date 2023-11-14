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

/// \file simpleApplyPidSofieModelHardcoded
/// \brief A simple example for using PID obtained from the PID ML ONNX Model via SOPHIE TMVA.
///        Assumption: model files to use are known at compile time.
///        Use-cases: models do not change between datasets -- most of models used in analysis
///                   except for those PID-related. PID here is just an example.
///
/// \author Maja Kabus <mkabus@cern.ch>

#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"
#include "CCDB/CcdbApi.h"
#include "Common/DataModel/TrackSelectionTables.h"
#include "Common/DataModel/PIDResponse.h"
#include "Tools/PIDML/pidSofieModelHardcoded.h"

#include <string>

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

namespace o2::aod
{
namespace mlpidresult
{
DECLARE_SOA_INDEX_COLUMN(Track, track);       //! Track index
DECLARE_SOA_COLUMN(Pid, pid, int);            //! Pid to be tested by the model
DECLARE_SOA_COLUMN(Accepted, accepted, bool); //! Whether the model accepted particle to be of given kind
} // namespace mlpidresult
DECLARE_SOA_TABLE(MlPidResults, "AOD", "MLPIDRESULTS", o2::soa::Index<>, mlpidresult::TrackId, mlpidresult::Pid, mlpidresult::Accepted);
} // namespace o2::aod

struct SimpleApplySofieModel {
  PidSofieModel pidModel; // One instance per model, e.g., one per each pid to predict
  Configurable<double> cfgCertainty{"certainty", 0.5, "Min certainty of the model to accept given particle to be of given kind"};

  Produces<o2::aod::MlPidResults> pidMLResults;

  Filter trackFilter = requireGlobalTrackInFilter();
  // Minimum table requirements for sample model:
  // TPC signal (FullTracks), TOF signal (TOFSignal), TOF beta (pidTOFbeta), dcaXY and dcaZ (TracksDCA)
  // Filter on isGlobalTrack (TracksSelection)
  using BigTracks = soa::Filtered<soa::Join<aod::FullTracks, aod::TracksDCA, aod::pidTOFbeta, aod::TrackSelection, aod::TOFSignal>>;

  void init(InitContext const&)
  {
    pidModel = PidSofieModelHardcoded(cfgCertainty.value);
  }

  void process(BigTracks const& tracks)
  {
    for (auto& track : tracks) {
      bool accepted = pidModel.applyModelBoolean(track);
      LOGF(info, "collision id: %d track id: %d accepted: %d p: %.3f; x: %.3f, y: %.3f, z: %.3f",
           track.collisionId(), track.index(), accepted, track.p(), track.x(), track.y(), track.z());
      pidMLResults(track.index(), cfgPid.value, accepted);
    }
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<SimpleApplySofieModel>(cfgc)};
}
