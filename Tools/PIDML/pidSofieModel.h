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

/// \file pidSofieModel.h
/// \brief A class that manages PID ML ONNX model using ROOT TMVA SOPHIE.
///
/// \author Maja Kabus <mkabus@cern.ch>

#ifndef TOOLS_PIDML_PIDSOPHIEMODEL_H_
#define TOOLS_PIDML_PIDSOPHIEMODEL_H_

#include <string>
#include <algorithm>
#include <map>
#include <utility>
#include <memory>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

#include "CCDB/CcdbApi.h"

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

struct PidSofieModel {
 public:
  PidSofieModel(std::string& localPath, std::string& ccdbPath, bool useCCDB, o2::ccdb::CcdbApi& ccdbApi, uint64_t timestamp, int pid, PidMLDetector detector, double minCertainty) : mDetector(detector), mPid(pid), mMinCertainty(minCertainty)
  {
    std::string modelFile;
    loadInputFiles(localPath, ccdbPath, useCCDB, ccdbApi, timestamp, pid, modelFile);
    parseModel(modelFile);
    gInterpreter.Declare("#include <" + modelFile + ".hxx>");
  }

  PidSofieModel() = default;
  PidSofieModel(PidSofieModel&&) = default;
  PidSofieModel& operator=(PidSofieModel&&) = default;
  PidSofieModel(const PidSofieModel&) = delete;
  PidSofieModel& operator=(const PidSofieModel&) = delete;
  ~PidSofieModel() = default;

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

  void parseModel(std::string const& inputFile)
  {
    TMVA::Experimental::SOFIE::RModelParser_ONNX parser;
    TMVA::Experimental::SOFIE::RModel model = parser.Parse(inputFile, true);

    std::string outputFile = inputFile + ".hxx";
    model.Generate();
    model.OutputGenerated(outputFile);

    model.PrintRequiredInputTensors();
    // Printing initialized tensors (weights)
    std::cout << "\n\n";
    model.PrintInitializedTensors();
    std::cout << "\n\n";
    model.PrintIntermediateTensors();
  }

  PidMLDetector mDetector;
  int mPid;
  double mMinCertainty;

 private:
  void getModelPaths(std::string const& path, std::string& modelDir, std::string& modelFile, std::string& modelPath, int pid, std::string const& ext)
  {
    modelDir = path + "/TPC";
    if (mDetector >= kTPCTOF) {
      modelDir += "_TOF";
    }
    if (mDetector >= kTPCTOFTRD) {
      modelDir += "_TRD";
    }

    modelFile = "simple_model_";
    if (pid < 0) {
      modelFile += "0" + std::to_string(-pid);
    } else {
      modelFile += std::to_string(pid);
    }
    modelFile += ext;
    modelPath = modelDir + "/" + modelFile;
  }

  void downloadFromCCDB(o2::ccdb::CcdbApi& ccdbApi, std::string const& ccdbFile, uint64_t timestamp, std::string const& localDir, std::string const& localFile)
  {
    std::map<std::string, std::string> metadata;
    bool retrieveSuccess = ccdbApi.retrieveBlob(ccdbFile, localDir, metadata, timestamp, false, localFile);
    if (retrieveSuccess) {
      std::map<std::string, std::string> headers = ccdbApi.retrieveHeaders(ccdbFile, metadata, timestamp);
      LOG(info) << "Network file downloaded from: " << ccdbFile << " to: " << localDir << "/" << localFile;
    } else {
      LOG(fatal) << "Error encountered while fetching/loading the network from CCDB! Maybe the network doesn't exist yet for this run number/timestamp?";
    }
  }

  void loadInputFiles(std::string const& localPath, std::string const& ccdbPath, bool useCCDB, o2::ccdb::CcdbApi& ccdbApi, uint64_t timestamp, int pid, std::string& modelPath)
  {
    rapidjson::Document trainColumnsDoc;
    rapidjson::Document scalingParamsDoc;

    std::string localDir, localModelFile;
    std::string trainColumnsFile = "columns_for_training";
    std::string scalingParamsFile = "scaling_params";
    getModelPaths(localPath, localDir, localModelFile, modelPath, pid, ".onnx");
    std::string localTrainColumnsPath = localDir + "/" + trainColumnsFile + ".json";
    std::string localScalingParamsPath = localDir + "/" + scalingParamsFile + ".json";

    if (useCCDB) {
      std::string ccdbDir, ccdbModelFile, ccdbModelPath;
      getModelPaths(ccdbPath, ccdbDir, ccdbModelFile, ccdbModelPath, pid, "");
      std::string ccdbTrainColumnsPath = ccdbDir + "/" + trainColumnsFile;
      std::string ccdbScalingParamsPath = ccdbDir + "/" + scalingParamsFile;
      downloadFromCCDB(ccdbApi, ccdbModelPath, timestamp, localDir, localModelFile);
      downloadFromCCDB(ccdbApi, ccdbTrainColumnsPath, timestamp, localDir, "columns_for_training.json");
      downloadFromCCDB(ccdbApi, ccdbScalingParamsPath, timestamp, localDir, "scaling_params.json");
    }

    LOG(info) << "Using configuration files: " << localTrainColumnsPath << ", " << localScalingParamsPath;
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

#endif // TOOLS_PIDML_PIDSOPHIEMODEL_H_
