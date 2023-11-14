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

/// \file pidSofieModelHardcoded.h
/// \brief A class that manages PID ML ONNX model using ROOT TMVA SOFIE.
///        Assumption: model files to use are known at compile time.
///        Use-cases: models do not change between datasets -- most of models used in analysis
///                   except for those PID-related. PID here is just an example.
///
/// \author Maja Kabus <mkabus@cern.ch>

#ifndef TOOLS_PIDML_PIDSOFIEMODEL_H_
#define TOOLS_PIDML_PIDSOFIEMODEL_H_

#include <string>
#include <algorithm>
#include <map>
#include <utility>
#include <memory>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

#include "CCDB/CcdbApi.h"

#include "Tools/PIDML/simple_model_pion.hxx"

#include <TInterpreter.h>
#include <TMVA/RModelParser_ONNX.hxx>
#include <TMVA/RModel.hxx>
#include <TMVA/SOFIEHelpers.hxx>

enum PidMLDetector {
  kTPCOnly = 0,
  kTPCTOF,
  kTPCTOFTRD,
  kNDetectors ///< number of available detectors configurations
};

// TODO: Copied from cefpTask, shall we put it in some common utils code?
namespace
{
bool readJsonFile(const std::string& config, rapidjson::Document& d)
{
  FILE* fp = fopen(config.data(), "rb");
  if (!fp) {
    LOG(error) << "Missing configuration json file: " << config;
    return false;
  }

  char readBuffer[65536];
  rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

  d.ParseStream(is);
  fclose(fp);
  return true;
}
} // namespace

struct PidSofieModelHardcoded {
 public:
  PidSofieModelHardcoded(double minCertainty) : mDetector(PidMLDetector::kTPCOnly), mPid(211), mMinCertainty(minCertainty)
  {
    loadInputFiles();
  }

  PidSofieModelHardcoded() = default;
  PidSofieModelHardcoded(PidSofieModelHardcoded&&) = default;
  PidSofieModelHardcoded& operator=(PidSofieModelHardcoded&&) = default;
  PidSofieModelHardcoded(const PidSofieModelHardcoded&) = delete;
  PidSofieModelHardcoded& operator=(const PidSofieModelHardcoded&) = delete;
  ~PidSofieModelHardcoded() = default;

  template <typename T>
  float applyModel(const T& track)
  {
    return getModelOutput(track);
  }

  template <typename T>
  bool applyModelBoolean(const T& track)
  {
    return getModelOutput(track) >= mMinCertainty;
  }

  PidMLDetector mDetector;
  int mPid;
  double mMinCertainty;

 private:
  void loadInputFiles()
  {
    rapidjson::Document trainColumnsDoc;
    rapidjson::Document scalingParamsDoc;

    std::string localTrainColumnsPath = "/home/mkabus/alice/O2Physics/Tools/PIDML/columns_for_training.json";
    std::string localScalingParamsPath = "/home/mkabus/alice/O2Physics/Tools/PIDML/scaling_params.json";

    if (readJsonFile(localTrainColumnsPath, trainColumnsDoc)) {
      for (auto& param : trainColumnsDoc["columns_for_training"].GetArray()) {
        mTrainColumns.emplace_back(param.GetString());
      }
    }
    if (readJsonFile(localScalingParamsPath, scalingParamsDoc)) {
      for (auto& param : scalingParamsDoc["data"].GetArray()) {
        mScalingParams[param[0].GetString()] = std::make_pair(param[1].GetFloat(), param[2].GetFloat());
      }
    }
  }

  // template <typename T>
  // float getModelOutput(const T& tracks)
  //{
  //   ROOT::EnableImplicitMT(nthreads);

  //  ROOT::RDataFrame df1("tracks", tracks);
  //  int nslots = df1.GetNSlots();
  //  std::cout << "Running using " << nslots << " threads" << std::endl;
  //  auto h1 = df1.DefineSlot("DNN_Value", TMVA::Experimental::SofieFunctor<7, TMVA_SOFIE_Pid_trained_model::Session>(nslots),
  //                           {"m_jj", "m_jjj", "m_lv", "m_jlv", "m_bb", "m_wbb", "m_wwbb"})
  //              .Histo1D({"h_sig", "", 100, 0, 1}, "DNN_Value");

  //  auto input_shape = mInputShapes[0];
  //  std::vector<float> inputTensorValues = createInputsSingle(track);
  //  std::vector<Ort::Value> inputTensors;
  //  inputTensors.emplace_back(Ort::Experimental::Value::CreateTensor<float>(inputTensorValues.data(), inputTensorValues.size(), input_shape));

  //  // Double-check the dimensions of the input tensor
  //  assert(inputTensors[0].IsTensor() &&
  //         inputTensors[0].GetTensorTypeAndShapeInfo().GetShape() == input_shape);
  //  LOG(debug) << "input tensor shape: " << printShape(inputTensors[0].GetTensorTypeAndShapeInfo().GetShape());

  //  try {
  //    auto outputTensors = mSession->Run(mInputNames, inputTensors, mOutputNames);

  //    // Double-check the dimensions of the output tensors
  //    // The number of output tensors is equal to the number of output nodes specified in the Run() call
  //    assert(outputTensors.size() == mOutputNames.size() && outputTensors[0].IsTensor());
  //    LOG(debug) << "output tensor shape: " << printShape(outputTensors[0].GetTensorTypeAndShapeInfo().GetShape());

  //    const float* output_value = outputTensors[0].GetTensorData<float>();
  //    float certainty = sigmoid(*output_value); // FIXME: Temporary, sigmoid will be added as network layer
  //    return certainty;
  //  } catch (const Ort::Exception& exception) {
  //    LOG(error) << "Error running model inference: " << exception.what();
  //  }
  //  return false; // unreachable code
  //}

  std::vector<std::string> mTrainColumns;
  std::map<std::string, std::pair<float, float>> mScalingParams;
};

#endif // TOOLS_PIDML_PIDSOFIEMODEL_H_
