// c++ -o QGl_v2 `root-config --glibs --cflags` -lm QGl_v2.cpp


#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>
#include <assert.h>
#include "TROOT.h" 
#include "TH1F.h"
#include "TH2F.h"
#include "TString.h"
#include "TFile.h"
#include "TChain.h"
#include "TNtuple.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TMath.h"
#include "TMap.h"
#include "TLorentzVector.h"
#include "conversions.h"
#include "vbf.h"
#include "vbf_histos.h"
#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
//using namespace ROOT::Math;
//using namespace std ;
//typedef LorentzVector<ROOT::Math::PxPyPzE4D<double> > lorentzVector ;



struct dist_sort: public std::binary_function<pair <lorentzVector *, lorentzVector *>, pair <lorentzVector *, lorentzVector *>, bool>
{
	bool operator() (const pair <lorentzVector *, lorentzVector *> & x, const pair <lorentzVector *, lorentzVector *> & y)
	{
		return  (deltaR2 (x.first->Eta (), x.first->Phi (), x.second->Eta (), x.second->Phi()) < 
				deltaR2 (y.first->Eta (), y.first->Phi (), y.second->Eta (), y.second->Phi())) ;
	}
} ;


struct local_TLVP_ptSort: public std::binary_function<const lorentzVector&,const lorentzVector& , bool>
{
	bool operator() (const lorentzVector& x, const lorentzVector& y)
	{
		return x.Pt () < y.Pt () ;

	}
} ;
struct sortPt
{
	bool operator()(TLorentzVector* s1, TLorentzVector* s2) const
	{
		return s1->Pt() >= s2->Pt();
	}
} mysortPt;



float my_deltaR_function(float eta1, float phi1, float eta2, float phi2)
{
	float delta_phi(float phi1, float phi2);
	const float PI=2.0*acos(0.);
	const float TWOPI=2.0*PI;
	float deta = eta1-eta2;
	float dphi = delta_phi(phi1,phi2);
	return sqrt(deta*deta + dphi*dphi);
}


// function defined..................................below.................
float delta_phi(float phi1, float phi2)
{
	const float PI=2.0*acos(0.);
	const float TWOPI=2.0*PI;
	float PHI=fabs(phi1-phi2);
	return (PHI<=PI)? PHI : TWOPI-PHI;
}



/*
   struct TLVP_ptSort: public std::binary_function<int, int, bool>
   {
   bool operator() (const lorentzVector * x, const lorentzVector * y)
   {
   return x->Pt () < y->Pt () ;
   }
   } ;

 */

int main (int argc, char** argv)
{
	if (argc != 3)
	{
		std::cout << ">>> Usage:   " << argv[0] << "   treeFile.root maxNumber deltaR" << std::endl;
		return -1;
	}

	int maxNumber = atoi (argv[1]) ;
	double maxDR = atof (argv[2]) ;
	//cout<<"   "<<maxDR<<endl;
	//PG input tree, variables, branches
	//PG ---- ---- ---- ---- ---- ---- ---- ---- ---- 

	//  TString inputFile = argv[1] ;
	//  TChain * WJet = new TChain ("WJet") ;
	//  WJet->Add ("/home/ajay/Work/LPC/Morion13/VBF/RD_mu_VBFHWWMH300_CMSSW532_private.root") ;
	//  WJet->Add (inputFile) ;
	//  if (WJet == 0) return 1 ;

	gROOT->Reset();
	TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/eos/uscms/store/user/ajay/QGStudy/ntuples/mu_W4Jets_CMSSW532.root");
	if (!f) {
		f = new TFile("/eos/uscms/store/user/ajay/QGStudy/ntuples/mu_W4Jets_CMSSW532.root");
		/*

		   TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("WmunuJetAnalysisntuple_1_1_gqg.root");
		   if (!f) {
		   f = new TFile("WmunuJetAnalysisntuple_1_1_gqg.root");
		 */
	}
	TTree * WJet;   
	f->GetObject("WJet",WJet);

	TFile * file = new TFile("Tree_W4Jets_vbf.root","RECREATE");
	TTree *QGl = new TTree("QGl","tree");



	// Declaration of leaf types
	Int_t           numPFCorJets;
	Int_t           numPFCorJetBTags;
	Float_t         JetPFCor_Px[8];
	Float_t         JetPFCor_Py[8];
	Float_t         JetPFCor_Pz[8];
	Float_t         JetPFCor_E[8];
	Float_t         JetPFCorVBFTag_Pt[8];
	Float_t         JetPFCorVBFTag_Eta[8];
	Float_t         JetPFCorVBFTag_Phi[8];
	Float_t         JetPFCorVBFTag_E[8];

	Float_t         JetPFCor_Et[8];
	Float_t         JetPFCor_Pt[8];
	Float_t         JetPFCor_Eta[8];
	Float_t         JetPFCor_Phi[8];
	Float_t         JetPFCorVBFTag_Et[8];


	Float_t         JetPFCor_QGLikelihood[8];
	Float_t         JetPFCorVBFTag_QGLikelihood[8];

	Float_t         hvbf_wjet1_QGd;
	Float_t         hvbf_wjet2_QGd;

	Float_t         W_gqmass_gen[20];
	Float_t         W_gq_px_gen[20];
	Float_t         W_gq_py_gen[20];
	Float_t         W_gq_pz_gen[20];
	Float_t         W_gq_e_gen[20];
	Float_t         W_gq_pt_gen[20];
	Float_t         W_gq_et_gen[20];
	Float_t         W_gq_eta_gen[20];
	Float_t         W_gq_phi_gen[20];
	Float_t         W_gq_vx_gen[20];
	Float_t         W_gq_vy_gen[20];
	Float_t         W_gq_vz_gen[20];
	Float_t         W_gq_y_gen[20];
	Int_t           W_gq_Id_gen[20];




	//PG the Higgs boson MC truth

	TBranch        *b_numPFCorJets ;   //!
	TBranch        *b_numPFCorJetBTags ;   //!
	TBranch        *b_JetPFCor_Px ;   //!
	TBranch        *b_JetPFCor_Py ;   //!
	TBranch        *b_JetPFCor_Pz ;   //!
	TBranch        *b_JetPFCor_E ;   //!

	TBranch        *b_JetPFCor_Pt ;   //!
	TBranch        *b_JetPFCor_Eta;   //!
	TBranch        *b_JetPFCor_Phi;   //!
	// TBranch        *b_JetPFCor_E ;   //!

	TBranch        *b_JetPFCorVBFTag_Pt;   //!
	TBranch        *b_JetPFCorVBFTag_Eta;   //!
	TBranch        *b_JetPFCorVBFTag_Phi;   //!
	TBranch        *b_JetPFCorVBFTag_E;   //!
	TBranch        *b_JetPFCor_QGLikelihood;   //!
	TBranch        *b_JetPFCorVBFTag_QGLikelihood;   //!


	TBranch        *b_W_gq_px_gen;   //!
	TBranch        *b_W_gq_py_gen;   //!
	TBranch        *b_W_gq_pz_gen;   //!
	TBranch        *b_W_gq_e_gen;   //!
	TBranch        *b_W_gq_pt_gen;   //!
	TBranch        *b_W_gq_et_gen;   //!
	TBranch        *b_W_gq_eta_gen;   //!
	TBranch        *b_W_gq_phi_gen;   //!
	TBranch        *b_W_gq_vx_gen;   //!
	TBranch        *b_W_gq_vy_gen;   //!
	TBranch        *b_W_gq_vz_gen;   //!
	TBranch        *b_W_gq_y_gen;   //!
	TBranch        *b_W_gq_Id_gen;   //!



	WJet->SetBranchAddress ("numPFCorJets",       &numPFCorJets,      &b_numPFCorJets      ) ;
	WJet->SetBranchAddress ("numPFCorJetBTags",   &numPFCorJetBTags,  &b_numPFCorJetBTags  ) ;
	WJet->SetBranchAddress ("JetPFCor_Px",        JetPFCor_Px,        &b_JetPFCor_Px       ) ;
	WJet->SetBranchAddress ("JetPFCor_Py",        JetPFCor_Py,        &b_JetPFCor_Py       ) ;
	WJet->SetBranchAddress ("JetPFCor_Pz",        JetPFCor_Pz,        &b_JetPFCor_Pz       ) ;
	WJet->SetBranchAddress ("JetPFCor_E",         JetPFCor_E,         &b_JetPFCor_E        ) ;
	WJet->SetBranchAddress("JetPFCor_Pt",JetPFCor_Pt);
	WJet->SetBranchAddress("JetPFCor_Eta",JetPFCor_Eta);
	WJet->SetBranchAddress("JetPFCor_Phi",JetPFCor_Phi);

	WJet->SetBranchAddress ("JetPFCorVBFTag_Pt",  JetPFCorVBFTag_Pt,  &b_JetPFCorVBFTag_Pt ) ;
	WJet->SetBranchAddress ("JetPFCorVBFTag_Eta", JetPFCorVBFTag_Eta, &b_JetPFCorVBFTag_Eta) ;
	WJet->SetBranchAddress ("JetPFCorVBFTag_Phi", JetPFCorVBFTag_Phi, &b_JetPFCorVBFTag_Phi) ;
	WJet->SetBranchAddress ("JetPFCorVBFTag_E",   JetPFCorVBFTag_E,   &b_JetPFCorVBFTag_E  ) ;
	WJet->SetBranchAddress("JetPFCor_QGLikelihood",JetPFCor_QGLikelihood);
	WJet->SetBranchAddress("JetPFCorVBFTag_QGLikelihood",JetPFCorVBFTag_QGLikelihood);
	WJet->SetBranchAddress("hvbf_wjet1_QGd",&hvbf_wjet1_QGd);
	WJet->SetBranchAddress("hvbf_wjet2_QGd",&hvbf_wjet2_QGd);


	WJet->SetBranchAddress("W_gqmass_gen[20]",W_gqmass_gen);
	WJet->SetBranchAddress("W_gq_px_gen[20]",W_gq_px_gen);
	WJet->SetBranchAddress("W_gq_py_gen[20]",W_gq_py_gen);
	WJet->SetBranchAddress("W_gq_pz_gen[20]",W_gq_pz_gen);
	WJet->SetBranchAddress("W_gq_e_gen[20]",W_gq_e_gen);
	WJet->SetBranchAddress("W_gq_pt_gen[20]",W_gq_pt_gen);
	WJet->SetBranchAddress("W_gq_et_gen[20]",W_gq_et_gen);
	WJet->SetBranchAddress("W_gq_eta_gen[20]",W_gq_eta_gen);
	WJet->SetBranchAddress("W_gq_phi_gen[20]",W_gq_phi_gen);
	WJet->SetBranchAddress("W_gq_vx_gen[20]",W_gq_vx_gen);
	WJet->SetBranchAddress("W_gq_vy_gen[20]",W_gq_vy_gen);
	WJet->SetBranchAddress("W_gq_vz_gen[20]",W_gq_vz_gen);
	WJet->SetBranchAddress("W_gq_y_gen[20]",W_gq_y_gen);
	WJet->SetBranchAddress("W_gq_Id_gen[20]",W_gq_Id_gen);




	//PG histograms
	//PG ----------


	//declaration of new tree leaves
	float h_qdeltaR=-999;
	float h_gdeltaR=-999;
	float h_qmatchedSize=-1;
	float h_gmatchedSize=-1;

	float h_quark_pt[8]={0};
	float h_quark_eta[8]={-999};

	float h_gluon_pt[8]={0};
	float h_gluon_eta[8]={-999};

	float h_quarklike_pt[8]={0};
	float h_quarklike_eta[8]={-999};
	float h_quarklike_QGl=-10;
	float h_gluonlike_QGl=-10;


	float h_gluonlike_pt[8]={0};
	float h_gluonlike_eta[8]={-999};

	QGl->Branch("h_qdeltaR",&h_qdeltaR, "h_qdeltaR/F");
	QGl->Branch("h_gdeltaR",&h_gdeltaR, "h_gdeltaR/F");
	QGl->Branch("h_qmatchedSize",&h_qmatchedSize, "h_qmatchedSize/F");
	QGl->Branch("h_gmatchedSize",&h_gmatchedSize, "h_gmatchedSize/F");


	QGl->Branch("h_quark_pt", &h_quark_pt ,"h_quark_pt[8]/F");
	QGl->Branch("h_quark_eta", &h_quark_eta ,"h_quark_eta[8]/F");
	QGl->Branch("h_gluon_pt", &h_gluon_pt ,"h_gluon_pt[8]/F");
	QGl->Branch("h_gluon_eta", &h_gluon_eta ,"h_gluon_eta[8]/F");
	QGl->Branch("h_quarklike_pt", &h_quarklike_pt ,"h_quarklike_pt[8]/F");
	QGl->Branch("h_quarklike_eta", &h_quarklike_eta ,"h_quarklike_eta[8]/F");
	QGl->Branch("h_gluonlike_pt", &h_gluonlike_pt ,"h_gluonlike_pt[8]/F");
	QGl->Branch("h_gluonlike_eta", &h_gluonlike_eta ,"h_gluonlike_eta[8]/F");


	QGl->Branch("h_quarklike_QGl", &h_quarklike_QGl ,"h_quarklike_QGl/F");
	QGl->Branch("h_gluonlike_QGl", &h_gluonlike_QGl ,"h_gluonlike_QGl/F");



	//PG loop over events
	Long64_t nentries = WJet->GetEntries();
	cout << nentries << endl ;
	Long64_t nbytes = 0;
	for (Long64_t ent=0; ent<nentries;ent++) {
		nbytes += WJet->GetEntry(ent);
		//        if (jentry % 1000 == 0) cout << "evnt " << jentry << endl ;
		//lorentzVector W_H_gen ;
		//		vector <lorentzVector> jets ;
		vector <TLorentzVector*> QGd ;

		vector<TLorentzVector*> jets;
		TMap jetsQGd;//Quark-Gluon Discriminator Map

		//std::map < lorentzVector ,lorentzVector> jetsQGd;  
		vector <TLorentzVector*> quarks ;
		vector <TLorentzVector*> gluons ;
		vector <TLorentzVector*> quarklike_jets;
		vector <TLorentzVector*> gluonlike_jets;

		//        vector <lorentzVector> quarks_v;

		//PG build the total default jet collection
		//PG ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
		int numFwdJets = 0 ;
		for (int i = 0 ; i < 8 ; ++i) if (JetPFCorVBFTag_Pt[i] > 1.) ++numFwdJets ;

		int numGPFJets = 0 ;
		//for (int i = 0 ; i < 2 ; ++i) if (W_Parton_pt[i] > 1.) ++numGPFJets ;
		for (int i = 0 ; i < 20 ; ++i) if ( W_gq_Id_gen[i]!=0 && W_gq_pt_gen[i] > 20.) ++numGPFJets ;

		int jetsNum = numPFCorJets + numFwdJets ;
		if (jetsNum < 1) continue ;

		int GjetsNum =  numGPFJets ;
		if (GjetsNum < 1) continue ;

		//PG read jets from the event and merge the collections
		for( int i = 0; i < numPFCorJets; ++i)
		{
			if(JetPFCor_Pt[i] > 20.0)
			{
				TLorentzVector *dummy = new TLorentzVector(0,0,0,0);
				dummy->SetPtEtaPhiE( JetPFCor_Pt[i], JetPFCor_Eta[i], JetPFCor_Phi[i], JetPFCor_E[i]);
				jets.push_back(dummy);
				Float_t *tmp_qgd = &JetPFCor_QGLikelihood[i];
				jetsQGd.Add(dummy,(TObject*)tmp_qgd);
				//           Float_t *tmp_btagCSV = &JetPFCor_bDiscriminatorCSV[i];
				//             jetsbtagCSV.Add(dummy,(TObject*)tmp_btagCSV);

			}
		}

		for( int j = 0; j < numFwdJets; ++j)
		{
			if(JetPFCorVBFTag_Pt[j] > 20.0)
			{
				TLorentzVector *dummy = new TLorentzVector(0,0,0,0);
				dummy->SetPtEtaPhiE( JetPFCorVBFTag_Pt[j], JetPFCorVBFTag_Eta[j], JetPFCorVBFTag_Phi[j], JetPFCorVBFTag_E[j]);
				//jets.push_back(dummy);
				//	Float_t *tmp_qgd = &JetPFCorVBFTag_QGLikelihood[j];
				//	jetsQGd.Add(dummy,(TObject*)tmp_qgd);
				//                Float_t *tmp_btagCSV = &JetPFCorVBFTag_bDiscriminatorCSV[j];
				//              jetsbtagCSV.Add(dummy,(TObject*)tmp_btagCSV);

			}
		}
		sort (jets.begin (), jets.end (), mysortPt);  //Sort the jet according to Jet Pt
		for (unsigned int i=0;i<jets.size();i++)
		{
		//	if(jets.size()==4)	cout<<"ent  "<<ent<<"  i   "<<i<<"    "<<jets.at(i)->Pt()<<endl;
			//	cout<<"forward qgl    "<<JetPFCorVBFTag_QGLikelihood[i]<<endl;
			//       cout<<" central qgl   "<<JetPFCor_QGLikelihood[i]<<endl;

		}
		//cout<<"  hvbf_wjet1_QGd  "<<hvbf_wjet1_QGd<<endl;
		//cout<<"  hvbf_wjet2_QGd  "<<hvbf_wjet2_QGd<<endl;
		//quarks
		for (int i = 0 ; i < numGPFJets ; ++i)
		{
			if((abs(W_gq_Id_gen[i]))<7 && (W_gq_Id_gen[i]!=0))
			{
				TLorentzVector *dummy1 = new TLorentzVector(0,0,0,0);
				dummy1->SetPtEtaPhiE(W_gq_pt_gen[i], W_gq_eta_gen[i], W_gq_phi_gen[i], W_gq_e_gen[i]);
				quarks.push_back (dummy1) ;
			}
		}
		//gluons
		for (int i = 0 ; i < numGPFJets ; ++i)
		{
			if((abs(W_gq_Id_gen[i]))==21)
			{
				TLorentzVector *dummy1 = new TLorentzVector(0,0,0,0);
				dummy1->SetPtEtaPhiE(W_gq_pt_gen[i], W_gq_eta_gen[i], W_gq_phi_gen[i], W_gq_e_gen[i]);
				gluons.push_back (dummy1) ;
			}
		}

		//	cout<<" quarks size    "<<quarks.size()<<endl;
		//	cout<<" gluons size    "<<gluons.size()<<endl;
		//	cout<<" jets size    "<<jets.size()<<endl;
		sort (quarks.begin (), quarks.end (), mysortPt) ;
		//cout<<" quarks size  after sorting   "<<quarks.size()<<endl;
		sort (gluons.begin (), gluons.end (), mysortPt) ;
		//cout<<" gluons size  after sorting   "<<gluons.size()<<endl;
		//PG      deltaR      (recoIndex , genIndex)
		std::map <float, std::pair<int, int> >q_matchingsMap ;
		for (int iReco = 0 ; iReco < jets.size (); ++iReco) // loop over reco jets
		{
			if(fabs(jets.at(iReco)->Eta())>2.5) continue;
			for (int iGen = 0 ; iGen < quarks.size (); ++iGen) // loop over quarks
			{
				if(fabs(quarks.at(iGen)->Eta())>2.5) continue;	
				float deltaR = my_deltaR_function (
						jets.at (iReco)->Eta (),jets.at (iReco)->Phi (), 
						quarks.at (iGen)->Eta (), quarks.at (iGen)->Phi ()) ;
				h_qdeltaR=deltaR ;
				q_matchingsMap[deltaR] = std::pair<int, int> (iReco, iGen) ;
			}
		}

		for (std::map<float, std::pair<int, int> >::const_iterator ipair = q_matchingsMap.begin () ;
				ipair != q_matchingsMap.end () ;
				++ipair)
		{
			//cout <<"delta R between jets and quarks : "<< ipair->first <<" reco jet index  "<<ipair->second.first<<" quark index  "<<ipair->second.second << endl;
			//cout<<"......  "<<endl;
		}



		vector<int> q_recoUsed (jets.size (), 0) ;   //PG vector of flags to know if a jet is already used
		vector<int> q_genUsed (quarks.size (), 0) ; //PG vector of flags to know if a jet is already used
		std::map<float, std::pair<int, int> > q_cleanedMatchings ;
		// remove elements with jets that have been already used
		for (std::map<float, std::pair<int, int> >::const_iterator iMap = q_matchingsMap.begin () ;
				iMap != q_matchingsMap.end () ;
				++iMap)
		{
			if (iMap->first > maxDR) break ;
			if (q_recoUsed.at (iMap->second.first) == 1 ||
					q_genUsed.at (iMap->second.second) == 1)
				continue ; 
			q_cleanedMatchings[iMap->first] = iMap->second ;
			q_recoUsed.at (iMap->second.first) = 1 ;
			q_genUsed.at (iMap->second.second) = 1 ;

		}  
		for (std::map<float, std::pair<int, int> >::const_iterator ipair1 = q_cleanedMatchings.begin () ;
				ipair1 != q_cleanedMatchings.end () ;
				++ipair1)
		{
			//	cout <<"delta R between clean jets and quarks : "<< ipair1->first <<" reco jet index  "<<ipair1->second.first<<" quark index  "<<ipair1->second.second << endl;
			//		h_quarklikejet1_pt.Fill (jets.at (ipair1->second.first ).Pt());
			// cout<<"......  "<<endl;
		}
		h_qmatchedSize=q_cleanedMatchings.size () ;

		//gluons matching
		std::map <float, std::pair<int, int> > g_matchingsMap ;
		for (int iReco = 0 ; iReco < jets.size (); ++iReco) // loop over reco jets
		{
			if(fabs(jets.at(iReco)->Eta())>2.5) continue;
			for (int iGen = 0 ; iGen < gluons.size (); ++iGen) // loop over gluons
			{
				if(fabs(gluons.at(iGen)->Eta())>2.5) continue;
				float deltaR = my_deltaR_function (
						jets.at (iReco)->Eta (),jets.at (iReco)->Phi (),
						gluons.at (iGen)->Eta (), gluons.at (iGen)->Phi ()) ;
				h_gdeltaR=deltaR;
				g_matchingsMap[deltaR] = std::pair<int, int> (iReco, iGen) ;
			}
		}

		vector<int> g_recoUsed (jets.size (), 0) ;   //PG vector of flags to know if a jet is already used
		vector<int> g_genUsed (gluons.size (), 0) ; //PG vector of flags to know if a jet is already used
		std::map<float, std::pair<int, int> > g_cleanedMatchings ;
		// remove elements with jets that have been already used
		for (std::map<float, std::pair<int, int> >::const_iterator iMap = g_matchingsMap.begin () ;
				iMap != g_matchingsMap.end () ;
				++iMap)
		{
			if (iMap->first > maxDR) break ;
			if (g_recoUsed.at (iMap->second.first) == 1 ||
					g_genUsed.at (iMap->second.second) == 1)
				continue ;
			g_cleanedMatchings[iMap->first] = iMap->second ;
			g_recoUsed.at (iMap->second.first) = 1 ;
			g_genUsed.at (iMap->second.second) = 1 ;

		}
		h_gmatchedSize=g_cleanedMatchings.size () ;


		for (std::map<float, std::pair<int, int> >::const_iterator ipair2 = g_cleanedMatchings.begin () ;
				ipair2 != g_cleanedMatchings.end () ;
				++ipair2)
		{
			//	cout <<"delta R between clean jets and gluons : "<< ipair2->first <<" reco jet index  "<<ipair2->second.first<<" gluon index  "<<ipair2->second.second << endl;
			//		h_gluonlikejet1_pt.Fill (jets.at (ipair2->second.first ).Pt());
			// cout<<"......  "<<endl;
		}


		quarklike_jets.clear();

		for (std::map<float, std::pair<int, int> >::const_iterator iMap = q_cleanedMatchings.begin () ;
				iMap != q_cleanedMatchings.end () ;
				++iMap)
		{
			TLorentzVector *dummy3 = new TLorentzVector(0,0,0,0);
			dummy3->SetPtEtaPhiE( 
					jets.at (iMap->second.first)->Pt (),
					jets.at (iMap->second.first)->Eta (),
					jets.at (iMap->second.first)->Phi (),
					jets.at (iMap->second.first)->E ()
					) ;
			//      quarklike_jets.clear();
			quarklike_jets.push_back(dummy3) ;
			if((jetsQGd.FindObject(jets.at(iMap->second.first)) != 0) )
			{
				Float_t *tmpqg = (Float_t*) jetsQGd.GetValue(jets[iMap->second.first]);
				h_quarklike_QGl = *tmpqg;
			if(h_quarklike_QGl>-1)	cout<<"  quark QGl value    "<<h_quarklike_QGl<<endl;
			}
		}

		gluonlike_jets.clear();


		for (std::map<float, std::pair<int, int> >::const_iterator iMap = g_cleanedMatchings.begin () ;
				iMap != g_cleanedMatchings.end () ;
				++iMap)
		{
			TLorentzVector *dummy4 = new TLorentzVector(0,0,0,0);
			dummy4->SetPtEtaPhiE( 
					jets.at (iMap->second.first)->Pt (),
					jets.at (iMap->second.first)->Eta (),
					jets.at (iMap->second.first)->Phi (),
					jets.at (iMap->second.first)->E ()
					) ;
			gluonlike_jets.push_back(dummy4) ;
			//cout <<"delta R  : "<< iMap->first <<" reco jet index  "<<iMap->second.first<<" gluon index  "<<iMap->second.second << endl;
			//cout<<" test  "<<jetsQGd.FindObject(jets[iMap->second.first])<<endl;
			if(jetsQGd.FindObject(jets[iMap->second.first]) != 0 )
			{
			//cout <<"delta R  : "<< iMap->first <<" reco jet index  "<<iMap->second.first<<" gluon index  "<<iMap->second.second << endl;
				Float_t *tmpqg = (Float_t*) jetsQGd.GetValue(jets[iMap->second.first]);
				h_gluonlike_QGl = *tmpqg;
		if(h_gluonlike_QGl>-1)	cout<<"  gluon QGl value    "<<h_gluonlike_QGl<<endl;
			}


		}
		sort (gluonlike_jets.begin (), gluonlike_jets.end (),mysortPt ) ;
		sort (quarklike_jets.begin (), quarklike_jets.end (),mysortPt ) ;
		/*
		   for(int i =0;i<quarklike_jets.size();++i)
		   {
		   Float_t *tmpqg = (Float_t*) jetsQGd.GetValue(quarklike_jets[i]);
		   h_quarklike_QGl = *tmpqg;
		   cout<<"  quark QGl value    "<<h_quarklike_QGl<<endl;

		   }

		   for(int i=0;i<gluonlike_jets.size();++i)
		   {
		   Float_t *tmpqg = (Float_t*) jetsQGd.GetValue(gluonlike_jets[i]);
		   h_gluonlike_QGl = *tmpqg;
		   cout<<"  gluon QGl value    "<<h_gluonlike_QGl<<endl;
		   }
		 */
		//cout<<"gluonlike_jets size "<<gluonlike_jets.size()<<endl;
		//cout<<"quarklike_jets size "<<quarklike_jets.size()<<endl;
		// fill variables
		for( int i= 0;i<quarks.size();++i)
		{
			h_quark_pt[i]=quarks.at (i)->Pt();
			h_quark_eta[i]=quarks.at (i)->Eta();
		}
		for(int i=0;i<gluons.size();++i)
		{
			h_gluon_pt[i]=gluons.at (i)->Pt();
			h_gluon_eta[i]=gluons.at (i)->Eta();
		}
		for( int i= 0;i<quarklike_jets.size();++i)
		{
			h_quarklike_pt[i]=quarklike_jets.at(i)->Pt();
			h_quarklike_eta[i]=quarklike_jets.at (i)->Eta();
		}
		for(int i=0;i<gluonlike_jets.size();++i) 
		{
			h_gluonlike_pt[i]=gluonlike_jets.at(i)->Pt();
			h_gluonlike_eta[i]=gluonlike_jets.at (i)->Eta();
		}

		QGl->Fill();

	}   //PG loop over events
	QGl->Write();
	file->Write();
	/*	TString outFileName = "output_" ;
		outFileName += maxNumber;
		outFileName += "_";
		outFileName += int (maxDR * 100);
		outFileName += ".root" ;
		TFile outFile (outFileName, "recreate") ;
	 */
	//outFile.Close () ;

	return 0 ;
	}

