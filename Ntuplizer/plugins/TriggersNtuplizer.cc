#include "../interface/TriggersNtuplizer.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"

//===================================================================================================================        
TriggersNtuplizer::TriggersNtuplizer( edm::EDGetTokenT<edm::TriggerResults> tokens, 
                                      edm::EDGetTokenT<pat::TriggerObjectStandAloneCollection> object,
				      edm::EDGetTokenT<pat::PackedTriggerPrescales> prescale, 
				      edm::EDGetTokenT<edm::TriggerResults> noiseFilterToken, 
				      edm::EDGetTokenT<bool> HBHENoiseFilterLooseResultToken,
				      edm::EDGetTokenT<bool> HBHENoiseFilterTightResultToken,
				      edm::EDGetTokenT<bool> HBHENoiseIsoFilterResultToken,
				      NtupleBranches* nBranches, 
				      const edm::ParameterSet& iConfig, 
				      std::map< std::string, bool >& runFlags)
   : CandidateNtuplizer	( nBranches )
   , HLTtriggersToken_	( tokens )
   , triggerObjects_	( object )	
   , triggerPrescales_	( prescale )
   , noiseFilterToken_	( noiseFilterToken )
   , HBHENoiseFilterLoose_Selector_( HBHENoiseFilterLooseResultToken )
   , HBHENoiseFilterTight_Selector_( HBHENoiseFilterTightResultToken )
   , HBHENoiseIsoFilter_Selector_( HBHENoiseIsoFilterResultToken )
   , doTriggerDecisions_( runFlags["doTriggerDecisions"] )
   , doTriggerObjects_	( runFlags["doTriggerObjects"] )
   , doHltFilters_	( runFlags["doHltFilters"] )
   , runOnMC_           ( runFlags["runOnMC"] )
{
   
  HBHENoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_HBHENoiseFilter");  
  CSCHaloNoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_CSCTightHaloFilter");
  HCALlaserNoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_hcalLaserEventFilter");
  ECALDeadCellNoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_EcalDeadCellTriggerPrimitiveFilter");
  GoodVtxNoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_goodVertices");
  TrkFailureNoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_trackingFailureFilter");
  EEBadScNoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_eeBadScFilter");
  ECALlaserNoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_ecalLaserCorrFilter");
  TrkPOGNoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_trkPOGFilters");
  TrkPOG_manystrip_NoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_trkPOG_manystripclus53X");
  TrkPOG_toomanystrip_NoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_trkPOG_toomanystripclus53X");
  TrkPOG_logError_NoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_trkPOG_logErrorTooManyClusters");
  METFilters_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_metFilters");
   
}

//===================================================================================================================
TriggersNtuplizer::~TriggersNtuplizer( void )
{

}

//===================================================================================================================
bool TriggersNtuplizer::findTrigger( std::string trigName ){

   if( trigName.find("AK8PFJet360_TrimMass30") != std::string::npos ||
       trigName.find("AK8PFHT700_TrimR0p1PT0p03Mass50") != std::string::npos ||
       trigName.find("AK8PFHT650_TrimR0p1PT0p03Mass50") != std::string::npos ||
       trigName.find("AK8PFHT660_TrimR0p1PT0p03Mass50_BTagCSV") != std::string::npos ||
       trigName.find("AK8DiPFJet280_200_TrimMass30_BTagCSV") != std::string::npos ||
       trigName.find("AK8DiPFJet250_200_TrimMass30_BTagCSV") != std::string::npos ||
       trigName.find("PFHT650_WideJetMJJ950DEtaJJ1p5") != std::string::npos ||
       trigName.find("PFHT650_WideJetMJJ900DEtaJJ1p5") != std::string::npos ||
       trigName.find("PFHT400") != std::string::npos ||
       trigName.find("PFHT475") != std::string::npos ||
       trigName.find("PFHT600") != std::string::npos ||
       trigName.find("PFHT650") != std::string::npos ||
       trigName.find("PFHT800") != std::string::npos ||
       trigName.find("PFHT900") != std::string::npos ||
       trigName.find("PFJet320") != std::string::npos ||
       trigName.find("PFJet450") != std::string::npos ||         
       trigName.find("IsoMu17") != std::string::npos ||
       trigName.find("IsoMu18") != std::string::npos ||
       trigName.find("IsoMu20") != std::string::npos ||
       trigName.find("IsoMu24") != std::string::npos ||
       trigName.find("IsoMu27") != std::string::npos ||
       trigName.find("Mu45") != std::string::npos ||
       trigName.find("Mu50") != std::string::npos ||
       trigName.find("Ele27_eta2p1_WPLoose") != std::string::npos ||
       trigName.find("HLT_Ele27_eta2p1_WP75_Gsf") != std::string::npos ||
       trigName.find("Ele23_CaloIdL_TrackIdL_IsoVL") != std::string::npos ||
       trigName.find("Ele32_eta2p1_WP75_Gsf") != std::string::npos ||
       trigName.find("Ele105_CaloIdVT_GsfTrkIdT") != std::string::npos ||
       trigName.find("Ele115_CaloIdVT_GsfTrkIdT") != std::string::npos ||
       trigName.find("HLT_Ele22_eta2p1_WPLoose_Gsf_LooseIsoPFTau20") != std::string::npos ||
       trigName.find("HLT_Ele27_eta2p1_WPLoose_Gsf_DoubleMediumIsoPFTau35") != std::string::npos ||
       trigName.find("HLT_Ele27_eta2p1_WPLoose_Gsf_LooseIsoPFTau20") != std::string::npos ||
       trigName.find("HLT_IsoMu16_eta2p1_MET30_JetIDCleaned_LooseIsoPFTau50") != std::string::npos ||
     
       //H->tautau triggers
       trigName.find("HLT_LooseIsoPFTau50") != std::string::npos||
       trigName.find("HLT_Ele22_eta2p1_WP75_Gsf_LooseIsoPFTau20") != std::string::npos||
       trigName.find("HLT_Ele22_eta2p1_WP75_Gsf_v") != std::string::npos||
       trigName.find("HLT_Ele32_eta2p1_WP75_Gsf") != std::string::npos||
       trigName.find("HLT_Ele23_WPLoose_Gsf") != std::string::npos||
       trigName.find("HLT_DoubleMediumIsoPFTau40_Trk1_eta2p1_Reg") != std::string::npos||
       trigName.find("HLT_DoubleMediumIsoPFTau35_Trk1_eta2p1_Reg") != std::string::npos||

       //Alternative triggers
       trigName.find("HLT_PFMET120_PFMHT120_IDTight") != std::string::npos||
       trigName.find("HLT_MonoCentralPFJet80_PFMETNoMu120_JetIdCleaned_PFMHTNoMu120_IDTight")!= std::string::npos||
       trigName.find("HLT_PFMETNoMu120_JetIdCleaned_PFMHTNoMu120_IDTight")!= std::string::npos
   ) return true;
   else
     return false;

}

//===================================================================================================================
void TriggersNtuplizer::fillBranches( edm::Event const & event, const edm::EventSetup& iSetup ){
	
  event.getByToken(HLTtriggersToken_, HLTtriggers_);	  
  event.getByToken(triggerObjects_  , triggerObjects);
  event.getByToken(triggerPrescales_, triggerPrescales);

  const edm::TriggerNames& trigNames = event.triggerNames(*HLTtriggers_);
		
  if (doTriggerDecisions_) {
  	 for (unsigned int i = 0, n = HLTtriggers_->size(); i < n; ++i) {
	   //std::cout << "Trigger " << trigNames.triggerName(i) << ": " << (HLTtriggers_->accept(i) ? "PASS" : "fail (or not run)") << std::endl;
  	  if( findTrigger(trigNames.triggerName(i)) ){
   	     nBranches_->HLT_isFired[trigNames.triggerName(i)] = HLTtriggers_->accept(i);
   	  }
   	}
	  
  } //doTriggerDecisions_
	
  ////////////////// Trigger objects ///////////////////////////////////		
  if (doTriggerObjects_) {

     	std::vector<float> vfilterIDs; vfilterIDs.clear();
     	std::vector<int> vfiredTrigger; vfiredTrigger.clear();
		
  	for (pat::TriggerObjectStandAlone obj : *triggerObjects) { 
	
  		obj.unpackPathNames(trigNames);
		
  		std::vector<std::string> pathNamesAll  = obj.pathNames(false);
  		std::vector<std::string> pathNamesLast = obj.pathNames(true);
		
  		for (unsigned h = 0, n = pathNamesLast.size(); h < n; ++h) {
		
  			bool isBoth = obj.hasPathName( pathNamesLast[h], true , true );
                        bool isL3   = obj.hasPathName( pathNamesLast[h], false, true );
			
  			if( isBoth || isL3 ){
		
  			   nBranches_->triggerObject_pt	 .push_back(obj.pt());
  			   nBranches_->triggerObject_eta .push_back(obj.eta());
  			   nBranches_->triggerObject_phi .push_back(obj.phi());
  			   nBranches_->triggerObject_mass.push_back(obj.mass());
			
  			   for (unsigned h = 0; h < obj.filterIds().size(); ++h) vfilterIDs.push_back( obj.filterIds()[h]); // as defined in http://cmslxr.fnal.gov/lxr/source/DataFormats/HLTReco/interface/TriggerTypeDefs.h
				
  			   if( pathNamesLast[h] == "HLT_AK8PFJet360_TrimMass30_v1") vfiredTrigger.push_back( 0 );
  			   if( pathNamesLast[h] == "HLT_AK8PFHT700_TrimR0p1PT0p03Mass50_v1") vfiredTrigger.push_back( 1 );
  			   if( pathNamesLast[h] == "HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV0p41_v1") vfiredTrigger.push_back( 2 );
  			   if( pathNamesLast[h] == "HLT_PFHT650_WideJetMJJ950DEtaJJ1p5_v1") vfiredTrigger.push_back( 3 );
  			   if( pathNamesLast[h] == "HLT_PFHT650_WideJetMJJ900DEtaJJ1p5_v1") vfiredTrigger.push_back( 4 );
  			   if( pathNamesLast[h] == "HLT_PFHT900_v1") vfiredTrigger.push_back( 5 );
  			   if( pathNamesLast[h] == "HLT_IsoMu24_eta2p1_v1") vfiredTrigger.push_back( 6 );
  			   if( pathNamesLast[h] == "HLT_IsoMu24_eta2p1_v2") vfiredTrigger.push_back( 7 );
  			   if( pathNamesLast[h] == "HLT_Mu45_eta2p1_v1") vfiredTrigger.push_back( 8 );
  			   if( pathNamesLast[h] == "HLT_Mu50_eta2p1_v1") vfiredTrigger.push_back( 9 );
  			   if( pathNamesLast[h] == "HLT_Ele23_CaloIdL_TrackIdL_IsoVL_v1") vfiredTrigger.push_back( 10 );
  			   if( pathNamesLast[h] == "HLT_Ele32_eta2p1_WP75_Gsf_v1") vfiredTrigger.push_back( 11 );
  			   if( pathNamesLast[h] == "HLT_Ele105_CaloIdVT_GsfTrkIdT_v1") vfiredTrigger.push_back( 12 );
  			   if( pathNamesLast[h] == "HLT_Ele105_CaloIdVT_GsfTrkIdT_v2") vfiredTrigger.push_back( 13 );
  			   if( pathNamesLast[h] == "HLT_Ele115_CaloIdVT_GsfTrkIdT_v1") vfiredTrigger.push_back( 14 );
			  

  			   // else vfiredTrigger.push_back( -99 );
  			}
			
  		}
		
  		nBranches_->triggerObject_filterIDs.push_back(vfilterIDs);
  		nBranches_->triggerObject_firedTrigger.push_back(vfiredTrigger);
		
  	}
  } //doTriggerObjects_
    
  // HLT Noise Filters
  // for deprecation see https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2
  if (doHltFilters_) {
  
    event.getByToken(noiseFilterToken_, noiseFilterBits_);
    const edm::TriggerNames &names = event.triggerNames(*noiseFilterBits_);
      
    for (unsigned int i = 0, n = noiseFilterBits_->size(); i < n; ++i) {
      if (names.triggerName(i) == HBHENoiseFilter_Selector_)
        nBranches_->passFilter_HBHE_ = noiseFilterBits_->accept(i);
      if (names.triggerName(i) == CSCHaloNoiseFilter_Selector_)
        nBranches_->passFilter_CSCHalo_ = noiseFilterBits_->accept(i); // TO BE USED
      if (names.triggerName(i) == HCALlaserNoiseFilter_Selector_)
        nBranches_->passFilter_HCALlaser_ = noiseFilterBits_->accept(i); // DEPRECATED
      if (names.triggerName(i) == ECALDeadCellNoiseFilter_Selector_)
        nBranches_->passFilter_ECALDeadCell_ = noiseFilterBits_->accept(i); // under scrutiny
      if (names.triggerName(i) == GoodVtxNoiseFilter_Selector_)
        nBranches_->passFilter_GoodVtx_ = noiseFilterBits_->accept(i); // TO BE USED
      if (names.triggerName(i) == TrkFailureNoiseFilter_Selector_)
        nBranches_->passFilter_TrkFailure_ = noiseFilterBits_->accept(i); // DEPRECATED
      if (names.triggerName(i) == EEBadScNoiseFilter_Selector_)
        nBranches_->passFilter_EEBadSc_ = noiseFilterBits_->accept(i); // under scrutiny
      if (names.triggerName(i) == ECALlaserNoiseFilter_Selector_)
        nBranches_->passFilter_ECALlaser_ = noiseFilterBits_->accept(i); // DEPRECATED
      if (names.triggerName(i) == TrkPOGNoiseFilter_Selector_)
        nBranches_->passFilter_TrkPOG_ = noiseFilterBits_->accept(i); // DEPRECATED
      if (names.triggerName(i) == TrkPOG_manystrip_NoiseFilter_Selector_)
        nBranches_->passFilter_TrkPOG_manystrip_ = noiseFilterBits_->accept(i); // DEPRECATED
      if (names.triggerName(i) == TrkPOG_toomanystrip_NoiseFilter_Selector_)
        nBranches_->passFilter_TrkPOG_toomanystrip_ = noiseFilterBits_->accept(i); // DEPRECATED
      if (names.triggerName(i) == TrkPOG_logError_NoiseFilter_Selector_)
        nBranches_->passFilter_TrkPOG_logError_ = noiseFilterBits_->accept(i); // DEPRECATED
      if (names.triggerName(i) == METFilters_Selector_)
        nBranches_->passFilter_METFilters_ = noiseFilterBits_->accept(i); // DEPRECATED
    }
    
    //if( !runOnMC_ /*&& event.id().run() < 251585*/ ){

       edm::Handle<bool> HBHENoiseFilterLooseResultHandle;
       event.getByToken(HBHENoiseFilterLoose_Selector_, HBHENoiseFilterLooseResultHandle);
       bool HBHENoiseFilterLooseResult = *HBHENoiseFilterLooseResultHandle;
       if (!HBHENoiseFilterLooseResultHandle.isValid()) {
         LogDebug("") << "CaloTowerAnalyzer: Could not find HBHENoiseFilterResult" << std::endl;
       }
 
       nBranches_->passFilter_HBHELoose_ = HBHENoiseFilterLooseResult;

       edm::Handle<bool> HBHENoiseFilterTightResultHandle;
       event.getByToken(HBHENoiseFilterTight_Selector_, HBHENoiseFilterTightResultHandle);
       bool HBHENoiseFilterTightResult = *HBHENoiseFilterTightResultHandle;
       if (!HBHENoiseFilterTightResultHandle.isValid()) {
         LogDebug("") << "CaloTowerAnalyzer: Could not find HBHENoiseFilterResult" << std::endl;
       }
 
       nBranches_->passFilter_HBHETight_ = HBHENoiseFilterTightResult;
 
        edm::Handle<bool> HBHENoiseIsoFilterResultHandle;
       event.getByToken(HBHENoiseIsoFilter_Selector_, HBHENoiseIsoFilterResultHandle);
       bool HBHENoiseIsoFilterResult = *HBHENoiseIsoFilterResultHandle;
       if (!HBHENoiseIsoFilterResultHandle.isValid()) {
         LogDebug("") << "CaloTowerAnalyzer: Could not find HBHENoiseFilterResult" << std::endl;
       }
 
       nBranches_->passFilter_HBHEIso_ = HBHENoiseIsoFilterResult;
                          
    //}
    
  } //doHltFilters_

	
}
