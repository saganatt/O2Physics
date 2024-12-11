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

/// \file correlatorDsHadrons.cxx
/// \brief Ds-Hadrons correlator task - data-like, MC-reco and MC-Gen analyses
/// \author Grazia Luparello <grazia.luparello@cern.ch>
/// \author Samuele Cattaruzzi <samuele.cattaruzzi@cern.ch>

#include <vector>

#include "CommonConstants/PhysicsConstants.h"
#include "Framework/AnalysisTask.h"
#include "Framework/ASoAHelpers.h"
#include "Framework/HistogramRegistry.h"
#include "Framework/runDataProcessing.h"

#include "Common/Core/TrackSelection.h"
#include "Common/DataModel/Centrality.h"
#include "Common/DataModel/EventSelection.h"
#include "Common/DataModel/Multiplicity.h"
#include "Common/DataModel/TrackSelectionTables.h"

#include "PWGHF/Core/HfHelper.h"
#include "PWGHF/DataModel/CandidateReconstructionTables.h"
#include "PWGHF/DataModel/CandidateSelectionTables.h"
#include "PWGHF/HFC/DataModel/CorrelationTables.h"
#include "PWGHF/HFC/DataModel/DerivedDataCorrelationTables.h"

using namespace o2;
using namespace o2::analysis;
using namespace o2::constants::physics;
using namespace o2::constants::math;
using namespace o2::framework;
using namespace o2::framework::expressions;

/// Returns deltaPhi value in range [-pi/2., 3.*pi/2], typically used for correlation studies
double getDeltaPhi(double phiHadron, double phiD)
{
  return RecoDecay::constrainAngle(phiHadron - phiD, -PIHalf);
}

// binning type
using BinningTypeDerived = ColumnBinningPolicy<aod::hf_collisions_reduced::PosZ, aod::hf_collisions_reduced::Multiplicity>;

/// Ds-Hadron correlation pair builder - for real data and data-like analysis (i.e. reco-level w/o matching request via MC truth)
struct HfCorrelatorDsHadronsReduced {
  Produces<aod::DsHadronPair> entryDsHadronPair;
  Produces<aod::DsHadronRecoInfo> entryDsHadronRecoInfo;
  Produces<aod::DsHadronGenInfo> entryDsHadronGenInfo;

  Preslice<aod::AssocTrackReds> tracksPerCol = aod::hf_assoc_track_reduced::HfcRedCollisionId;
  Preslice<aod::DsCandReduceds> candsPerCol = aod::hf_candidate_reduced::HfcRedCollisionId;

  Configurable<bool> fillHistoData{"fillHistoData", true, "Flag for filling histograms in data processes"};
  Configurable<bool> fillHistoMcRec{"fillHistoMcRec", true, "Flag for filling histograms in MC Rec processes"};
  Configurable<bool> fillHistoMcGen{"fillHistoMcGen", true, "Flag for filling histograms in MC Gen processes"};
  Configurable<bool> removeCollWSplitVtx{"removeCollWSplitVtx", false, "Flag for rejecting the splitted collisions"};
  Configurable<bool> useSel8{"useSel8", true, "Flag for applying sel8 for collision selection"};
  Configurable<bool> selNoSameBunchPileUpColl{"selNoSameBunchPileUpColl", true, "Flag for rejecting the collisions associated with the same bunch crossing"};
  Configurable<int> selectionFlagDs{"selectionFlagDs", 7, "Selection Flag for Ds (avoid the case of flag = 0, no outputMlScore)"};
  Configurable<int> numberEventsMixed{"numberEventsMixed", 5, "Number of events mixed in ME process"};
  Configurable<int> decayChannel{"decayChannel", 1, "Decay channels: 1 for Ds->PhiPi->KKpi, 2 for Ds->K0*K->KKPi"};
  Configurable<bool> applyEfficiency{"applyEfficiency", true, "Flag for applying D-meson efficiency weights"};
  Configurable<float> yCandMax{"yCandMax", 0.8, "max. cand. rapidity"};
  Configurable<float> yCandGenMax{"yCandGenMax", 0.5, "max. gen. cand. rapidity"};
  Configurable<float> etaTrackMax{"etaTrackMax", 0.8, "max. eta of tracks"};
  Configurable<float> dcaXYTrackMax{"dcaXYTrackMax", 2., "max. DCA_xy of tracks"};
  Configurable<float> dcaZTrackMax{"dcaZTrackMax", 2., "max. DCA_z of tracks"};
  Configurable<float> ptCandMin{"ptCandMin", 1., "min. cand. pT"};
  Configurable<float> ptCandMax{"ptCandMax", 50., "max. cand pT"};
  Configurable<float> ptTrackMin{"ptTrackMin", 0.3, "min. track pT"};
  Configurable<float> ptTrackMax{"ptTrackMax", 50., "max. track pT"};
  Configurable<std::vector<int>> classMl{"classMl", {0, 1, 2}, "Indexes of ML scores to be stored. Three indexes max."};
  Configurable<std::vector<double>> binsPtD{"binsPtD", std::vector<double>{o2::analysis::hf_cuts_ds_to_k_k_pi::vecBinsPt}, "pT bin limits for candidate mass plots"};
  Configurable<std::vector<double>> binsPtHadron{"binsPtHadron", std::vector<double>{0.3, 2., 4., 8., 12., 50.}, "pT bin limits for assoc particle"};
  Configurable<std::vector<double>> binsPtEfficiencyD{"binsPtEfficiencyD", std::vector<double>{o2::analysis::hf_cuts_ds_to_k_k_pi::vecBinsPt}, "pT bin limits for efficiency"};
  Configurable<std::vector<double>> efficiencyD{"efficiencyD", {1., 1., 1., 1., 1., 1.}, "efficiency values for Ds meson"};
  ConfigurableAxis zPoolBins{"zPoolBins", {VARIABLE_WIDTH, -10.0, -2.5, 2.5, 10.0}, "z vertex position pools"};
  ConfigurableAxis multPoolBins{"multPoolBins", {VARIABLE_WIDTH, 0., 900., 1800., 6000.}, "event multiplicity pools (FT0M)"};
  ConfigurableAxis binsMassD{"binsMassD", {200, 1.7, 2.25}, "inv. mass (K^{#pm}K^{-}#pi^{+}) (GeV/#it{c}^{2})"};
  ConfigurableAxis binsEta{"binsEta", {50, -2., 2.}, "#it{#eta}"};
  ConfigurableAxis binsPhi{"binsPhi", {64, -PIHalf, 3. * PIHalf}, "#it{#varphi}"};
  ConfigurableAxis binsMultiplicity{"binsMultiplicity", {200, 0., 800.}, "Multiplicity"};
  ConfigurableAxis binsMultFT0M{"binsMultFT0M", {600, 0., 6000.}, "Multiplicity as FT0M signal amplitude"};
  ConfigurableAxis binsPosZ{"binsPosZ", {100, -10., 10.}, "primary vertex z coordinate"};
  ConfigurableAxis binsPoolBin{"binsPoolBin", {9, 0., 9.}, "PoolBin"};

  HfHelper hfHelper;
  SliceCache cache;

  HistogramRegistry registry{"registry", {}, OutputObjHandlingPolicy::AnalysisObject};

  void init(InitContext&)
  {
    AxisSpec axisMassD = {binsMassD, "inv. mass (K^{#pm}K^{-}#pi^{+}) (GeV/#it{c}^{2})"};
    AxisSpec axisEta = {binsEta, "#it{#eta}"};
    AxisSpec axisPhi = {binsPhi, "#it{#varphi}"};
    AxisSpec axisPtD = {(std::vector<double>)binsPtD, "#it{p}_{T} (GeV/#it{c})"};
    AxisSpec axisPtHadron = {(std::vector<double>)binsPtHadron, "#it{p}_{T} Hadron (GeV/#it{c})"};
    AxisSpec axisMultiplicity = {binsMultiplicity, "Multiplicity"};
    AxisSpec axisMultFT0M = {binsMultFT0M, "MultiplicityFT0M"};
    AxisSpec axisPosZ = {binsPosZ, "PosZ"};
    AxisSpec axisPoolBin = {binsPoolBin, "PoolBin"};
    AxisSpec axisStatus = {15, 0.5, 15.5, "Selection status"};

    // Histograms for data analysis
    if (fillHistoData) {
      registry.add("hPtCand", "Ds candidates pt", {HistType::kTH1F, {axisPtD}});
      registry.add("hSelectionStatusDsToKKPi", "Ds candidates selection", {HistType::kTH1F, {axisStatus}});
      registry.add("hSelectionStatusDsToPiKK", "Ds candidates selection", {HistType::kTH1F, {axisStatus}});
      registry.add("hCountSelectionStatusDsToKKPiAndToPiKK", "Ds candidates selection", {HistType::kTH1F, {{1, -0.5, 0.5}}});
      registry.add("hEta", "Ds candidates eta", {HistType::kTH1F, {axisEta}});
      registry.add("hEtaVsPtCand", "Ds candidates etaVsPt", {HistType::kTH2F, {{axisEta}, {axisPtD}}});
      registry.add("hEtaVsPtPartAssoc", "Particles associated etaVsPt", {HistType::kTH2F, {{axisEta}, {axisPtHadron}}});
      registry.add("hPhi", "Ds candidates phi", {HistType::kTH1F, {axisPhi}});
      registry.add("hPhiVsPtCand", "Ds candidates phiVsPt", {HistType::kTH2F, {{axisPhi}, {axisPtD}}});
      registry.add("hPhiVsPtPartAssoc", "Particles associated phiVsPt", {HistType::kTH2F, {{axisPhi}, {axisPtHadron}}});
      registry.add("hMultiplicity", "Multiplicity", {HistType::kTH1F, {axisMultiplicity}});
      registry.add("hMultFT0M", "Multiplicity FT0M", {HistType::kTH1F, {axisMultFT0M}});
      registry.add("hZVtx", "z vertex", {HistType::kTH1F, {axisPosZ}});
      registry.add("hMassDsVsPt", "Ds candidates massVsPt", {HistType::kTH2F, {{axisMassD}, {axisPtD}}});
      registry.add("hMassDsData", "Ds candidates mass", {HistType::kTH1F, {axisMassD}});
      registry.add("hCollisionPoolBin", "Ds candidates collision pool bin", {HistType::kTH1F, {axisPoolBin}});
      registry.add("hDsPoolBin", "Ds candidates pool bin", {HistType::kTH1F, {axisPoolBin}});
      registry.add("hTracksPoolBin", "Particles associated pool bin", {HistType::kTH1F, {axisPoolBin}});
    }
    // Histograms for MC Reco analysis
    if (fillHistoMcRec) {
      registry.add("hPtCandMcRecSig", "Ds,Hadron candidates - MC Reco", {HistType::kTH1F, {axisPtD}});
      registry.add("hPtCandMcRecSigPrompt", "Ds,Hadron candidates Prompt - MC Reco", {HistType::kTH1F, {axisPtD}});
      registry.add("hPtCandMcRecSigNonPrompt", "Ds,Hadron candidates Non Prompt - MC Reco", {HistType::kTH1F, {axisPtD}});
      registry.add("hPtCandMcRecBkg", "Ds,Hadron candidates - MC Reco", {HistType::kTH1F, {axisPtD}});
      registry.add("hSelectionStatusDsToKKPiMcRec", "Ds candidates selection", {HistType::kTH1F, {axisStatus}});
      registry.add("hSelectionStatusDsToPiKKMcRec", "Ds candidates selection", {HistType::kTH1F, {axisStatus}});
      registry.add("hCountSelectionStatusDsToKKPiAndToPiKKMcRec", "Ds candidates selection", {HistType::kTH1F, {{1, -0.5, 0.5}}});
      registry.add("hPtParticleAssocMcRec", "Associated Particle - MC Rec", {HistType::kTH1F, {axisPtHadron}});
      registry.add("hPtParticleAssocVsCandMcRec", "Associated Particle - MC Rec", {HistType::kTH2F, {{axisPtHadron}, {axisPtD}}});
      registry.add("hPtPrimaryParticleAssocVsCandMcRec", "Associated Particle - MC Rec", {HistType::kTH2F, {{axisPtHadron}, {axisPtD}}});
      registry.add("hEtaMcRecSig", "Ds,Hadron candidates - MC Reco", {HistType::kTH1F, {axisEta}});
      registry.add("hPhiMcRecSig", "Ds,Hadron candidates - MC Reco", {HistType::kTH1F, {axisPhi}});
      registry.add("hEtaMcRecBkg", "Ds,Hadron candidates - MC Reco", {HistType::kTH1F, {axisEta}});
      registry.add("hPhiMcRecBkg", "Ds,Hadron candidates - MC Reco", {HistType::kTH1F, {axisPhi}});
      registry.add("hPtVsMultiplicityMcRecPrompt", "Multiplicity FT0M - MC Rec Prompt", {HistType::kTH2F, {{axisPtD}, {axisMultFT0M}}});
      registry.add("hPtVsMultiplicityMcRecNonPrompt", "Multiplicity FT0M - MC Rec Non Prompt", {HistType::kTH2F, {{axisPtD}, {axisMultFT0M}}});
      registry.add("hMassDsMcRec", "Ds candidates", {HistType::kTH1F, {axisMassD}});
      registry.add("hMassDsVsPtMcRec", "Ds signal candidates - MC Reco", {HistType::kTH2F, {{axisMassD}, {axisPtD}}});
      registry.add("hMassDsMcRecSig", "Ds signal candidates - MC Reco", {HistType::kTH2F, {{axisMassD}, {axisPtD}}});
      registry.add("hMassDsMcRecBkg", "Ds background candidates - MC Reco", {HistType::kTH2F, {{axisMassD}, {axisPtD}}});
      registry.add("hFakeTracksMcRec", "Fake tracks - MC Rec", {HistType::kTH1F, {axisPtHadron}});
    }
    // Histograms for MC Gen analysis
    if (fillHistoMcGen) {
      registry.add("hPtCandMcGen", "Ds,Hadron particles - MC Gen", {HistType::kTH1F, {axisPtD}});
      registry.add("hPtCandMcGenPrompt", "Ds,Hadron particles - MC Gen Prompt", {HistType::kTH1F, {axisPtD}});
      registry.add("hPtCandMcGenNonPrompt", "Ds,Hadron particles - MC Gen Non Prompt", {HistType::kTH1F, {axisPtD}});
      registry.add("hPtParticleAssocMcGen", "Associated Particle - MC Gen", {HistType::kTH1F, {axisPtHadron}});
      registry.add("hEtaMcGen", "Ds,Hadron particles - MC Gen", {HistType::kTH1F, {axisEta}});
      registry.add("hPhiMcGen", "Ds,Hadron particles - MC Gen", {HistType::kTH1F, {axisPhi}});
      registry.add("hMultFT0AMcGen", "Ds,Hadron multiplicity FT0A - MC Gen", {HistType::kTH1F, {axisMultiplicity}});
    }
  }

  /// Fill histograms of quantities independent from the daugther-mass hypothesis for data
  /// \param candidate is candidate
  template <typename T1>
  void fillHisto(const T1& candidate)
  {
    registry.fill(HIST("hPtCand"), candidate.pt());
    registry.fill(HIST("hEta"), candidate.eta());
    registry.fill(HIST("hEtaVsPtCand"), candidate.eta(), candidate.pt());
    registry.fill(HIST("hPhi"), RecoDecay::constrainAngle(candidate.phi(), -PIHalf));
    registry.fill(HIST("hPhiVsPtCand"), RecoDecay::constrainAngle(candidate.phi(), -PIHalf), candidate.pt());
  }

  /// Fill histograms of quantities for the KKPi daugther-mass hypothesis for data
  /// \param candidate is candidate
  /// \param efficiencyWeight is the efficiency correction
  template <typename T1>
  void fillHistoKKPi(const T1& candidate, double efficiencyWeight)
  {
    registry.fill(HIST("hMassDsVsPt"), hfHelper.invMassDsToKKPi(candidate), candidate.pt(), efficiencyWeight);
    registry.fill(HIST("hMassDsData"), hfHelper.invMassDsToKKPi(candidate), efficiencyWeight);
    registry.fill(HIST("hSelectionStatusDsToKKPi"), candidate.isSelDsToKKPi());
  }

  /// Fill histograms of quantities for the PiKK daugther-mass hypothesis for data
  /// \param candidate is candidate
  /// \param efficiencyWeight is the efficiency correction
  template <typename T1>
  void fillHistoPiKK(const T1& candidate, double efficiencyWeight)
  {
    registry.fill(HIST("hMassDsVsPt"), hfHelper.invMassDsToPiKK(candidate), candidate.pt(), efficiencyWeight);
    registry.fill(HIST("hMassDsData"), hfHelper.invMassDsToPiKK(candidate), efficiencyWeight);
    registry.fill(HIST("hSelectionStatusDsToPiKK"), candidate.isSelDsToPiKK());
  }

  /// Fill histograms of quantities for the Ds signal for MC reco-level
  /// \param candidate is candidate
  /// \param multiplicityFT0M is the multiplicity
  template <typename T1>
  void fillHistoMcRecSig(const T1& candidate, float multiplicityFT0M)
  {
    registry.fill(HIST("hPtCandMcRecSig"), candidate.pt());
    registry.fill(HIST("hEtaMcRecSig"), candidate.eta());
    registry.fill(HIST("hPhiMcRecSig"), RecoDecay::constrainAngle(candidate.phi(), -PIHalf));

    // prompt and non-prompt division
    if (candidate.originMcRec() == RecoDecay::OriginType::Prompt) {
      registry.fill(HIST("hPtCandMcRecSigPrompt"), candidate.pt());
      registry.fill(HIST("hPtVsMultiplicityMcRecPrompt"), candidate.pt(), multiplicityFT0M);
    } else if (candidate.originMcRec() == RecoDecay::OriginType::NonPrompt) {
      registry.fill(HIST("hPtCandMcRecSigNonPrompt"), candidate.pt());
      registry.fill(HIST("hPtVsMultiplicityMcRecNonPrompt"), candidate.pt(), multiplicityFT0M);
    }
  }

  /// Fill histograms of quantities for the Ds backgroung for MC reco-level
  /// \param candidate is candidate
  template <typename T1>
  void fillHistoMcRecBkg(const T1& candidate)
  {
    registry.fill(HIST("hPtCandMcRecBkg"), candidate.pt());
    registry.fill(HIST("hEtaMcRecBkg"), candidate.eta());
    registry.fill(HIST("hPhiMcRecBkg"), RecoDecay::constrainAngle(candidate.phi(), -PIHalf));
  }

  /// Fill histograms of quantities for the Ds signal for MC reco-level
  /// \param particle is particle, Ds
  template <typename T1>
  void fillMcGenHisto(const T1& particle)
  {
    registry.fill(HIST("hPtCandMcGen"), particle.pt());
    registry.fill(HIST("hEtaMcGen"), particle.eta());
    registry.fill(HIST("hPhiMcGen"), RecoDecay::constrainAngle(particle.phi(), -PIHalf));

    // prompt and non-prompt division
    if (particle.originMcGen() == RecoDecay::OriginType::Prompt) {
      registry.fill(HIST("hPtCandMcGenPrompt"), particle.pt());
    } else if (particle.originMcGen() == RecoDecay::OriginType::NonPrompt) {
      registry.fill(HIST("hPtCandMcGenNonPrompt"), particle.pt());
    }
  }


  // Event Mixing
  /*void processDerivedDataME(aod::HfcRedCollisions::iterator const& collision,
                            aod::DsCandReduceds const& candidates,
                            aod::AssocTrackReds const& tracks)
  {
    BinningTypeDerived corrBinning{{zPoolBins, multPoolBins}, true};
    int poolBin = corrBinning.getBin(std::make_tuple(collision.posZ(), collision.multiplicity()));

    registry.fill(HIST("hMultFT0M"), collision.multiplicity());
    for (const auto& cand : candidates) {
      // Ds-Hadron correlation dedicated section
      for (const auto& pAssoc : tracks) {
        LOGF(info, "Candidate and tracks indeces (candId, trackId): Index = (%d, %d)", cand.hfcRedCollisionId(), pAssoc.hfcRedCollisionId());

        entryDsHadronPair(getDeltaPhi(pAssoc.phiAssocTrack(), cand.phiCand()),
                          pAssoc.etaAssocTrack() - cand.etaCand(),
                          cand.ptCand(),
                          pAssoc.ptAssocTrack(),
                          poolBin);
        entryDsHadronRecoInfo(cand.invMassDs(), false, false);
        entryDsHadronGenInfo(false, false, 0);
      }
    }
  }
  PROCESS_SWITCH(HfCorrelatorDsHadronsReduced, processDerivedDataME, "Process Mixed Event Derived Data", true);*/

  void processDerivedDataME(aod::HfcRedCollisions const& collisions,
                            aod::DsCandReduceds const& candidates,
                            aod::AssocTrackReds const& tracks)
  {

    BinningTypeDerived corrBinning{{zPoolBins, multPoolBins}, true};

    //auto tracksTuple = std::make_tuple(candidates, tracks);
    auto tracksTuple = std::make_tuple(tracks);
    for (const auto& collision : collisions) {
      LOG(info) << "collision: " << collision.globalIndex() << " multiplicity " << collision.multiplicity();
      registry.fill(HIST("hMultFT0M"), collision.multiplicity());
    }

    for (const auto& track : candidates) {
      LOG(info) << "cand: " << track.globalIndex() << " collision: " << track.hfcRedCollisionId() << " phi cand: " << track.phiCand();
    }

    for (const auto& track : tracks) {
      LOG(info) << "track: " << track.globalIndex() << " collision: " << track.hfcRedCollisionId() << " phi assoc track: " << track.phiAssocTrack();
    }

    LOG(info) << "creating pair";
    //Pair<aod::HfcRedCollisions, aod::AssocTrackReds, aod::DsCandReduceds, BinningTypeDerived> pairData{corrBinning, numberEventsMixed, -1, collisions, tracksTuple, &cache};
    SameKindPair<aod::HfcRedCollisions, aod::AssocTrackReds, BinningTypeDerived> pairData{corrBinning, numberEventsMixed, -1, collisions, tracksTuple, &cache};
    LOG(info) << "pair created";

    for (const auto& [c1, tracks1, c2, tracks2] : pairData) {
      if (tracks1.size() == 0) {
        continue;
      }

      int poolBin = corrBinning.getBin(std::make_tuple(c2.posZ(), c2.multiplicity()));
      int poolBinDs = corrBinning.getBin(std::make_tuple(c1.posZ(), c1.multiplicity()));
      registry.fill(HIST("hMultFT0M"), c1.multiplicity());
      registry.fill(HIST("hZVtx"), c1.posZ());
      registry.fill(HIST("hTracksPoolBin"), poolBin);
      registry.fill(HIST("hDsPoolBin"), poolBinDs);

      for (const auto& [cand, pAssoc] : o2::soa::combinations(o2::soa::CombinationsFullIndexPolicy(tracks1, tracks2))) {
        LOGF(info, "Mixed event tracks pair: (%d, %d) from events (%d, %d), track event: (%d, %d)", cand.index(), pAssoc.index(), c1.index(), c2.index(), cand.hfcRedCollisionId(), pAssoc.hfcRedCollisionId());

        entryDsHadronPair(getDeltaPhi(pAssoc.phiAssocTrack(), cand.phiCand()),
                          pAssoc.etaAssocTrack() - cand.etaCand(),
                          cand.ptCand(),
                          pAssoc.ptAssocTrack(),
                          poolBin);
        entryDsHadronRecoInfo(cand.invMassDs(), false, false);
        entryDsHadronGenInfo(false, false, 0);

      }
    }
  }
  PROCESS_SWITCH(HfCorrelatorDsHadronsReduced, processDerivedDataME, "Process Mixed Event Derived Data", true);
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{adaptAnalysisTask<HfCorrelatorDsHadronsReduced>(cfgc)};
}
