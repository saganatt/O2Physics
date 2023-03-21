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

/// \file simpleApplyPidModel
/// \brief A simple example for using PID obtained from the PID ML ONNX Model. See README.md for more detailed instructions.
///
/// \author Maja Kabus <mkabus@cern.ch>

#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"
#include "CCDB/CcdbApi.h"
#include "Common/DataModel/TrackSelectionTables.h"
#include "Common/DataModel/PIDResponse.h"
#include "Tools/PIDML/pidOnnxModel.h"

#include <string>

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

namespace o2::aod
{

struct SimpleApplyOnnxModelBindings {
  ONNXModel simpleModel;

  Configurable<std::string> cfgPathCCDB{"ccdb-path", "Users/m/mkabus/PIDML", "base path to the CCDB directory with ONNX models"};
  Configurable<std::string> cfgCCDBURL{"ccdb-url", "http://alice-ccdb.cern.ch", "URL of the CCDB repository"};
  Configurable<bool> cfgUseCCDB{"useCCDB", true, "Whether to autofetch ML model from CCDB. If false, local file will be used."};
  Configurable<std::string> cfgPathLocal{"local-path", "/home/mkabus/PIDML", "base path to the local directory with ONNX models"};

  Configurable<uint64_t> cfgTimestamp{"timestamp", 1524176895000, "Hardcoded timestamp for tests"};
  Configurable<bool> enableOptimizations{"enableOptimizations", true, "Enables the ONNX extended model-optimization: sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED)"};

  o2::ccdb::CcdbApi ccdbApi;
  std::map<std::string, std::string> metadata;

  const AxisSpec axis{{100, 0, 1.0}, "output"};
  HistogramRegistry registry{
    "registry",
    {{"outputVector", "outputVector", kTH1F, {axis}},
     {"outputBinding", "outputBinding", kTH1F, {axis}}}};

  void init(InitContext const&)
  {
    if (cfgUseCCDB) {
      ccdbApi.init(cfgCCDBURL);
      bool retrieveSuccess = ccdbApi.retrieveBlob(cfgPathCCDB.value, ".", metadata, 1, false, cfgPathLocal.value);
      if (retrieveSuccess) {
        simpleModel.initModel(cfgPathLocal.value, enableOptimizations.value);
      }
    } else {
      simpleModel.initModel(cfgPathLocal.value, enableOptimizations.value);
    }
  }

  void processVector(aod::FullTracks const& tracks)
  {
    for (auto& track : tracks) {
      std::vector<float> modelInput = {track.x(), track.y(), track.pt(), track.eta(), track.phi()};
      float* modelOutput = simpleModel.evalModel(modelInput);
      LOGF(info, "collision id: %d track id: %d result: %.3f x: %.3f, y: %.3f, pt: %.3f, eta: %.3f, phi: %.3f",
           track.collisionId(), track.index(), modelOutput[0], track.x(), track.y(), track.pt(), track.eta(), track.phi());
      registry.fill(HIST("outputVector"), modelOutput[0]);
    }
  }
  PROCESS_SWITCH(SimpleApplyOnnxModelBindings, processVector, "Process with vector copy", true);

  void processBinding(aod::FullTracks const& tracks)
  {
    std::vector<Ort::Value> outputTensors;
    simpleModel.bindIO(tracks, outputTensors);
    float* outputValues = simpleModel.evalModelBinding<float>();

    int count = 0;
    for (auto& track : tracks) {
      float modelOutput = outputValues[count];
      LOGF(info, "collision id: %d track id: %d result: %.3f x: %.3f, y: %.3f, pt: %.3f, eta: %.3f, phi: %.3f",
           track.collisionId(), track.index(), modelOutput, track.x(), track.y(), track.pt(), track.eta(), track.phi());
      registry.fill(HIST("outputBinding"), modelOutput);
    }
  }
  PROCESS_SWITCH(SimpleApplyOnnxModelBindings, processBinding, "Process with input bindings", false);
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<SimpleApplyOnnxModelBindings>(cfgc)};
}
