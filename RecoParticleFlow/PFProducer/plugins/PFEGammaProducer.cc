/**\class PFEGammaProducer
\brief Producer for particle flow reconstructed particles (PFCandidates)

This producer makes use of PFAlgo, the particle flow algorithm.

\author Colin Bernet
\date   July 2006
*/

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "RecoParticleFlow/PFClusterTools/interface/PFEnergyCalibration.h"
#include "RecoParticleFlow/PFClusterTools/interface/PFEnergyCalibrationHF.h"
#include "CondFormats/PhysicsToolsObjects/interface/PerformancePayloadFromTFormula.h"
#include "CondFormats/DataRecord/interface/PFCalibrationRcd.h"
#include "CondFormats/DataRecord/interface/GBRWrapperRcd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecHitFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecHit.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementSuperClusterFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementSuperCluster.h"
#include "CondFormats/DataRecord/interface/ESEEIntercalibConstantsRcd.h"
#include "CondFormats/DataRecord/interface/ESChannelStatusRcd.h"
#include "CondFormats/ESObjects/interface/ESEEIntercalibConstants.h"
#include "CondFormats/ESObjects/interface/ESChannelStatus.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateEGammaExtraFwd.h"
#include "DataFormats/EgammaReco/interface/PreshowerClusterFwd.h"
#include "DataFormats/EgammaReco/interface/PreshowerCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/CaloRecHit/interface/CaloClusterFwd.h"
#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlock.h"
#include "RecoParticleFlow/PFProducer/interface/PFEGammaAlgo.h"

#include <sstream>
#include <string>
#include <memory>

class PFEGammaProducer : public edm::stream::EDProducer<edm::GlobalCache<PFEGammaAlgo::GBRForests> > {
public:
  explicit PFEGammaProducer(const edm::ParameterSet&, const PFEGammaAlgo::GBRForests*);

  static std::unique_ptr<PFEGammaAlgo::GBRForests> initializeGlobalCache(const edm::ParameterSet& conf) {
    return std::unique_ptr<PFEGammaAlgo::GBRForests>(new PFEGammaAlgo::GBRForests(conf));
  }

  static void globalEndJob(PFEGammaAlgo::GBRForests const*) {}

  void produce(edm::Event&, const edm::EventSetup&) override;
  void beginRun(const edm::Run&, const edm::EventSetup&) override {}

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void setPFVertexParameters(reco::VertexCollection const& primaryVertices);

  void createSingleLegConversions(reco::PFCandidateEGammaExtraCollection& extras,
                                  reco::ConversionCollection& oneLegConversions,
                                  const edm::RefProd<reco::ConversionCollection>& convProd);

  const edm::EDGetTokenT<reco::PFBlockCollection> inputTagBlocks_;
  const edm::EDGetTokenT<reco::PFCluster::EEtoPSAssociation> eetopsSrc_;
  const edm::EDGetTokenT<reco::VertexCollection> vertices_;

  // Use vertices for Neutral particles ?
  const bool useVerticesForNeutral_;

  /// Variables for PFEGamma

  reco::Vertex primaryVertex_;

  /// particle flow algorithm
  std::unique_ptr<PFEGammaAlgo> pfeg_;

  const std::string ebeeClustersCollection_;
  const std::string esClustersCollection_;
};

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(PFEGammaProducer);

#ifdef PFLOW_DEBUG
#define LOGDRESSED(x) edm::LogInfo(x)
#else
#define LOGDRESSED(x) LogDebug(x)
#endif

PFEGammaProducer::PFEGammaProducer(const edm::ParameterSet& iConfig, const PFEGammaAlgo::GBRForests* gbrForests)
    : inputTagBlocks_(consumes<reco::PFBlockCollection>(iConfig.getParameter<edm::InputTag>("blocks"))),
      eetopsSrc_(consumes<reco::PFCluster::EEtoPSAssociation>(iConfig.getParameter<edm::InputTag>("EEtoPS_source"))),
      vertices_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertexCollection"))),
      useVerticesForNeutral_(iConfig.getParameter<bool>("useVerticesForNeutral")),
      primaryVertex_(reco::Vertex()),
      ebeeClustersCollection_("EBEEClusters"),
      esClustersCollection_("ESClusters") {
  PFEGammaAlgo::PFEGConfigInfo algo_config;

  algo_config.produceEGCandsWithNoSuperCluster = iConfig.getParameter<bool>("produceEGCandsWithNoSuperCluster");

  // register products
  produces<reco::PFCandidateCollection>();
  produces<reco::PFCandidateEGammaExtraCollection>();
  produces<reco::SuperClusterCollection>();
  produces<reco::CaloClusterCollection>(ebeeClustersCollection_);
  produces<reco::CaloClusterCollection>(esClustersCollection_);
  produces<reco::ConversionCollection>();

  //PFElectrons Configuration
  algo_config.mvaEleCut = iConfig.getParameter<double>("pf_electron_mvaCut");

  algo_config.applyCrackCorrections = iConfig.getParameter<bool>("pf_electronID_crackCorrection");

  algo_config.mvaConvCut = iConfig.getParameter<double>("pf_conv_mvaCut");

  //PFEGamma
  //for MVA pass PV if there is one in the collection otherwise pass a dummy
  if (!useVerticesForNeutral_) {  // create a dummy PV
    reco::Vertex::Error e;
    e(0, 0) = 0.0015 * 0.0015;
    e(1, 1) = 0.0015 * 0.0015;
    e(2, 2) = 15. * 15.;
    reco::Vertex::Point p(0, 0, 0);
    primaryVertex_ = reco::Vertex(p, e, 0, 0, 0);
  }
  pfeg_ = std::make_unique<PFEGammaAlgo>(algo_config, *gbrForests);
  pfeg_->setPrimaryVertex(primaryVertex_);
}

void PFEGammaProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  LOGDRESSED("PFEGammaProducer") << "START event: " << iEvent.id().event() << " in run " << iEvent.id().run()
                                 << std::endl;

  // output collections
  auto egCandidates_ = std::make_unique<reco::PFCandidateCollection>();
  auto egExtra_ = std::make_unique<reco::PFCandidateEGammaExtraCollection>();
  auto sClusters_ = std::make_unique<reco::SuperClusterCollection>();

  // Get the EE-PS associations
  pfeg_->setEEtoPSAssociation(iEvent.get(eetopsSrc_));

  // preshower conditions
  edm::ESHandle<ESEEIntercalibConstants> esEEInterCalibHandle_;
  iSetup.get<ESEEIntercalibConstantsRcd>().get(esEEInterCalibHandle_);
  pfeg_->setAlphaGamma_ESplanes_fromDB(esEEInterCalibHandle_.product());

  edm::ESHandle<ESChannelStatus> esChannelStatusHandle_;
  iSetup.get<ESChannelStatusRcd>().get(esChannelStatusHandle_);
  pfeg_->setESChannelStatus(esChannelStatusHandle_.product());

  //Assign the PFAlgo Parameters
  setPFVertexParameters(iEvent.get(vertices_));

  // get the collection of blocks

  LOGDRESSED("PFEGammaProducer") << "getting blocks" << std::endl;
  auto blocks = iEvent.getHandle(inputTagBlocks_);

  LOGDRESSED("PFEGammaProducer") << "EGPFlow is starting..." << std::endl;

#ifdef PFLOW_DEBUG
  assert(blocks.isValid() && "edm::Handle to blocks was null!");
  std::ostringstream str;
  //str<<(*pfAlgo_)<<std::endl;
  //    cout << (*pfAlgo_) << std::endl;
  LOGDRESSED("PFEGammaProducer") << str.str() << std::endl;
#endif

  // sort elements in three lists:
  std::list<reco::PFBlockRef> hcalBlockRefs;
  std::list<reco::PFBlockRef> ecalBlockRefs;
  std::list<reco::PFBlockRef> hoBlockRefs;
  std::list<reco::PFBlockRef> otherBlockRefs;

  for (unsigned i = 0; i < blocks->size(); ++i) {
    // reco::PFBlockRef blockref( blockh,i );
    //reco::PFBlockRef blockref = createBlockRef( *blocks, i);
    reco::PFBlockRef blockref(blocks, i);

    const edm::OwnVector<reco::PFBlockElement>& elements = blockref->elements();

    LOGDRESSED("PFEGammaProducer") << "Found " << elements.size() << " PFBlockElements in block: " << i << std::endl;

    bool singleEcalOrHcal = false;
    if (elements.size() == 1) {
      switch (elements[0].type()) {
        case reco::PFBlockElement::SC:
          edm::LogError("PFEGammaProducer") << "PFBLOCKALGO BUG!!!! Found a SuperCluster in a block by itself!";
          break;
        case reco::PFBlockElement::PS1:
        case reco::PFBlockElement::PS2:
        case reco::PFBlockElement::ECAL:
          ecalBlockRefs.push_back(blockref);
          singleEcalOrHcal = true;
          break;
        case reco::PFBlockElement::HFEM:
        case reco::PFBlockElement::HFHAD:
        case reco::PFBlockElement::HCAL:
          if (elements[0].clusterRef()->flags() & reco::CaloCluster::badHcalMarker)
            continue;
          hcalBlockRefs.push_back(blockref);
          singleEcalOrHcal = true;
          break;
        case reco::PFBlockElement::HO:
          // Single HO elements are likely to be noise. Not considered for now.
          hoBlockRefs.push_back(blockref);
          singleEcalOrHcal = true;
          break;
        default:
          break;
      }
    }

    if (!singleEcalOrHcal) {
      otherBlockRefs.push_back(blockref);
    }
  }  //loop blocks

  // loop on blocks that are not single ecal, single ps1, single ps2 , or
  // single hcal and produce unbiased collection of EGamma Candidates

  //printf("loop over blocks\n");
  unsigned nblcks = 0;

  // this auto is a const reco::PFBlockRef&
  for (const auto& blockref : otherBlockRefs) {
    ++nblcks;
    // this auto is a: const edm::OwnVector< reco::PFBlockElement >&
    const auto& elements = blockref->elements();
    // make a copy of the link data, which will be edited.
    //PFBlock::LinkData linkData =  block.linkData();

    auto output = (*pfeg_)(blockref);

    if (!output.candidates.empty()) {
      LOGDRESSED("PFEGammaProducer") << "Block with " << elements.size() << " elements produced "
                                     << output.candidates.size() << " e-g candidates!" << std::endl;
    }

    const size_t egsize = egCandidates_->size();
    egCandidates_->resize(egsize + output.candidates.size());
    std::move(output.candidates.begin(), output.candidates.end(), egCandidates_->begin() + egsize);

    const size_t egxsize = egExtra_->size();
    egExtra_->resize(egxsize + output.candidateExtras.size());
    std::move(output.candidateExtras.begin(), output.candidateExtras.end(), egExtra_->begin() + egxsize);

    const size_t rscsize = sClusters_->size();
    sClusters_->resize(rscsize + output.refinedSuperClusters.size());
    std::move(output.refinedSuperClusters.begin(), output.refinedSuperClusters.end(), sClusters_->begin() + rscsize);
  }

  LOGDRESSED("PFEGammaProducer") << "Running PFEGammaAlgo on all blocks produced = " << egCandidates_->size()
                                 << " e-g candidates!" << std::endl;

  edm::RefProd<reco::SuperClusterCollection> sClusterProd = iEvent.getRefBeforePut<reco::SuperClusterCollection>();

  edm::RefProd<reco::PFCandidateEGammaExtraCollection> egXtraProd =
      iEvent.getRefBeforePut<reco::PFCandidateEGammaExtraCollection>();

  //set the correct references to refined SC and EG extra using the refprods
  for (unsigned int i = 0; i < egCandidates_->size(); ++i) {
    reco::PFCandidate& cand = egCandidates_->at(i);
    reco::PFCandidateEGammaExtra& xtra = egExtra_->at(i);

    reco::PFCandidateEGammaExtraRef extraref(egXtraProd, i);
    reco::SuperClusterRef refinedSCRef(sClusterProd, i);

    xtra.setSuperClusterRef(refinedSCRef);
    cand.setSuperClusterRef(refinedSCRef);
    cand.setPFEGammaExtraRef(extraref);
  }

  //build collections of output CaloClusters from the used PFClusters
  auto caloClustersEBEE = std::make_unique<reco::CaloClusterCollection>();
  auto caloClustersES = std::make_unique<reco::CaloClusterCollection>();

  std::map<edm::Ptr<reco::CaloCluster>, unsigned int> pfClusterMapEBEE;  //maps of pfclusters to caloclusters
  std::map<edm::Ptr<reco::CaloCluster>, unsigned int> pfClusterMapES;

  for (const auto& sc : *sClusters_) {
    for (reco::CaloCluster_iterator pfclus = sc.clustersBegin(); pfclus != sc.clustersEnd(); ++pfclus) {
      if (!pfClusterMapEBEE.count(*pfclus)) {
        reco::CaloCluster caloclus(**pfclus);
        caloClustersEBEE->push_back(caloclus);
        pfClusterMapEBEE[*pfclus] = caloClustersEBEE->size() - 1;
      } else {
        throw cms::Exception("PFEgammaProducer::produce")
            << "Found an EB/EE pfcluster matched to more than one supercluster!" << std::dec << std::endl;
      }
    }
    for (reco::CaloCluster_iterator pfclus = sc.preshowerClustersBegin(); pfclus != sc.preshowerClustersEnd();
         ++pfclus) {
      if (!pfClusterMapES.count(*pfclus)) {
        reco::CaloCluster caloclus(**pfclus);
        caloClustersES->push_back(caloclus);
        pfClusterMapES[*pfclus] = caloClustersES->size() - 1;
      } else {
        throw cms::Exception("PFEgammaProducer::produce")
            << "Found an ES pfcluster matched to more than one supercluster!" << std::dec << std::endl;
      }
    }
  }

  //put calocluster output collections in event and get orphan handles to create ptrs
  auto const& caloClusHandleEBEE = iEvent.put(std::move(caloClustersEBEE), ebeeClustersCollection_);
  auto const& caloClusHandleES = iEvent.put(std::move(caloClustersES), esClustersCollection_);

  //relink superclusters to output caloclusters
  for (auto& sc : *sClusters_) {
    edm::Ptr<reco::CaloCluster> seedptr(caloClusHandleEBEE, pfClusterMapEBEE[sc.seed()]);
    sc.setSeed(seedptr);

    reco::CaloClusterPtrVector clusters;
    for (reco::CaloCluster_iterator pfclus = sc.clustersBegin(); pfclus != sc.clustersEnd(); ++pfclus) {
      edm::Ptr<reco::CaloCluster> clusptr(caloClusHandleEBEE, pfClusterMapEBEE[*pfclus]);
      clusters.push_back(clusptr);
    }
    sc.setClusters(clusters);

    reco::CaloClusterPtrVector psclusters;
    for (reco::CaloCluster_iterator pfclus = sc.preshowerClustersBegin(); pfclus != sc.preshowerClustersEnd();
         ++pfclus) {
      edm::Ptr<reco::CaloCluster> clusptr(caloClusHandleES, pfClusterMapES[*pfclus]);
      psclusters.push_back(clusptr);
    }
    sc.setPreshowerClusters(psclusters);
  }

  //create and fill references to single leg conversions
  edm::RefProd<reco::ConversionCollection> convProd = iEvent.getRefBeforePut<reco::ConversionCollection>();
  auto singleLegConv_ = std::make_unique<reco::ConversionCollection>();
  createSingleLegConversions(*egExtra_, *singleLegConv_, convProd);

  // release our demonspawn into the wild to cause havoc
  iEvent.put(std::move(sClusters_));
  iEvent.put(std::move(egExtra_));
  iEvent.put(std::move(singleLegConv_));
  iEvent.put(std::move(egCandidates_));
}

void PFEGammaProducer::setPFVertexParameters(reco::VertexCollection const& primaryVertices) {
  primaryVertex_ = primaryVertices.front();
  for (auto const& pv : primaryVertices) {
    if (pv.isValid() && !pv.isFake()) {
      primaryVertex_ = pv;
      break;
    }
  }

  pfeg_->setPrimaryVertex(primaryVertex_);
}

void PFEGammaProducer::createSingleLegConversions(reco::PFCandidateEGammaExtraCollection& extras,
                                                  reco::ConversionCollection& oneLegConversions,
                                                  const edm::RefProd<reco::ConversionCollection>& convProd) {
  math::Error<3>::type error;
  for (auto& extra : extras) {
    for (const auto& tkrefmva : extra.singleLegConvTrackRefMva()) {
      const reco::Track& trk = *tkrefmva.first;

      const reco::Vertex convVtx(trk.innerPosition(), error);
      std::vector<reco::TrackRef> OneLegConvVector;
      OneLegConvVector.push_back(tkrefmva.first);
      std::vector<float> OneLegMvaVector;
      OneLegMvaVector.push_back(tkrefmva.second);
      std::vector<reco::CaloClusterPtr> dummymatchingBC;
      reco::CaloClusterPtrVector scPtrVec;
      scPtrVec.push_back(edm::refToPtr(extra.superClusterRef()));

      std::vector<math::XYZPointF> trackPositionAtEcalVec;
      std::vector<math::XYZPointF> innPointVec;
      std::vector<math::XYZVectorF> trackPinVec;
      std::vector<math::XYZVectorF> trackPoutVec;
      math::XYZPointF trackPositionAtEcal(trk.outerPosition().X(), trk.outerPosition().Y(), trk.outerPosition().Z());
      trackPositionAtEcalVec.push_back(trackPositionAtEcal);

      math::XYZPointF innPoint(trk.innerPosition().X(), trk.innerPosition().Y(), trk.innerPosition().Z());
      innPointVec.push_back(innPoint);

      math::XYZVectorF trackPin(trk.innerMomentum().X(), trk.innerMomentum().Y(), trk.innerMomentum().Z());
      trackPinVec.push_back(trackPin);

      math::XYZVectorF trackPout(trk.outerMomentum().X(), trk.outerMomentum().Y(), trk.outerMomentum().Z());
      trackPoutVec.push_back(trackPout);

      float DCA = trk.d0();
      float mvaval = tkrefmva.second;
      reco::Conversion singleLegConvCandidate(scPtrVec,
                                              OneLegConvVector,
                                              trackPositionAtEcalVec,
                                              convVtx,
                                              dummymatchingBC,
                                              DCA,
                                              innPointVec,
                                              trackPinVec,
                                              trackPoutVec,
                                              mvaval,
                                              reco::Conversion::pflow);
      singleLegConvCandidate.setOneLegMVA(OneLegMvaVector);
      oneLegConversions.push_back(singleLegConvCandidate);

      reco::ConversionRef convref(convProd, oneLegConversions.size() - 1);
      extra.addSingleLegConversionRef(convref);
    }
  }
}

void PFEGammaProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<bool>("useVerticesForNeutral", true);
  desc.add<bool>("produceEGCandsWithNoSuperCluster", false)
      ->setComment("Allow building of candidates with no input or output supercluster?");
  desc.add<double>("pf_electron_mvaCut", -0.1);
  desc.add<bool>("pf_electronID_crackCorrection", false);
  desc.add<double>("pf_conv_mvaCut", 0.0);
  desc.add<edm::InputTag>("blocks", edm::InputTag("particleFlowBlock"))->setComment("PF Blocks label");
  desc.add<edm::InputTag>("EEtoPS_source", edm::InputTag("particleFlowClusterECAL"))
      ->setComment("EE to PS association");
  desc.add<edm::InputTag>("vertexCollection", edm::InputTag("offlinePrimaryVertices"));
  desc.add<edm::FileInPath>("pf_electronID_mvaWeightFile",
                            edm::FileInPath("RecoParticleFlow/PFProducer/data/PfElectrons23Jan_BDT.weights.xml.gz"));
  desc.add<edm::FileInPath>("pf_convID_mvaWeightFile",
                            edm::FileInPath("RecoParticleFlow/PFProducer/data/pfConversionAug0411_BDT.weights.xml.gz"));
  descriptions.add("particleFlowEGamma", desc);
}
