        //#####################################################################################//
        //#                                                                                   #//
        //#          Script used to process flat tree events, create formated trees, etc.     #//
        //#                              -Created by Zachary Baldwin, June 2019               #//
        //#                                                                                   #//
        //#####################################################################################//

#include <iostream>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "TFile.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TMath.h"
#include "TChain.h"
#include "TLorentzVector.h"
#include "TLegend.h"
#include "TVector3.h"
#include "TStyle.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TLine.h"
#include <TRandom3.h>
#include "TGraphErrors.h"
#include "TLatex.h"
#include "TPad.h"
#include "TStyle.h"
#include <numeric>
#include <stdio.h>
#include <chrono>

void ProcessEvents_FlatTree_Example()
{

    // Path to flattree data
    TFile *file_PathWay=new TFile("/raid4/zbaldwin/FlatTree_Pi0Eta_Data/flattree_sum_ALL.root");
    // Flattree name
    TTree *flattree=(TTree*)file_PathWay->Get("pi0pi0pippim__B4"); 
    TString outFileName = "flattree_sum_AMO.root";

    // Initialize particle information found in FlatTree
    double Beam_px, Beam_py, Beam_pz, Beam_E;
    double Target_px, Target_py, Target_pz, Target_E;
    double PiPlus_px, PiPlus_py, PiPlus_pz, PiPlus_E;
    double PiMinus_px, PiMinus_py, PiMinus_pz, PiMinus_E;
    double Proton_px, Proton_py, Proton_pz, Proton_E;
    double Eta_px, Eta_py, Eta_pz, Eta_E;
    double Pi0_px, Pi0_py, Pi0_pz, Pi0_E;
    double Gamma1_px, Gamma1_py, Gamma1_pz, Gamma1_E; 
    double Gamma2_px, Gamma2_py, Gamma2_pz, Gamma2_E;
    double Gamma3_px, Gamma3_py, Gamma3_pz, Gamma3_E;
    double Gamma4_px, Gamma4_py, Gamma4_pz, Gamma4_E;

    // Weighting factors
    double locHistAccidWeightFactor, locWeight;

    // Other useful values 
    double locDeltaT_RF;   

    // Initialize TLorentzVectors predefined in a flat tree
    TLorentzVector* beam_p4_kin = 0;
    TLorentzVector* decaypi01_p4_kin = 0;
    TLorentzVector* decaypi02_p4_kin = 0;
    TLorentzVector* pip_p4_kin = 0; 
    TLorentzVector* pim_p4_kin = 0;
    TLorentzVector* p_p4_kin = 0;
    TLorentzVector* g1_p4_kin = 0;
    TLorentzVector* g2_p4_kin = 0;
    TLorentzVector* g3_p4_kin = 0;
    TLorentzVector* g4_p4_kin = 0;

    // Particle Information
    flattree->SetBranchAddress("beam_p4_kin", &beam_p4_kin );
    flattree->SetBranchAddress("decaypi01_p4_kin", &decaypi01_p4_kin);
    flattree->SetBranchAddress("decaypi02_p4_kin", &decaypi02_p4_kin);
    flattree->SetBranchAddress("pip_p4_kin", &pip_p4_kin);    
    flattree->SetBranchAddress("pim_p4_kin", &pim_p4_kin); 
    flattree->SetBranchAddress("p_p4_kin", &p_p4_kin); 
    flattree->SetBranchAddress("g1_p4_kin", &g1_p4_kin);
    flattree->SetBranchAddress("g2_p4_kin", &g2_p4_kin);
    flattree->SetBranchAddress("g3_p4_kin", &g3_p4_kin);
    flattree->SetBranchAddress("g4_p4_kin", &g4_p4_kin);

    flattree->SetBranchAddress("Beam_px", &Beam_px);
    flattree->SetBranchAddress("Beam_py", &Beam_py);
    flattree->SetBranchAddress("Beam_pz", &Beam_pz);
    flattree->SetBranchAddress("Beam_E", &Beam_E);
    flattree->SetBranchAddress("Target_px", &Target_px);
    flattree->SetBranchAddress("Target_py", &Target_py);
    flattree->SetBranchAddress("Target_pz", &Target_pz);
    flattree->SetBranchAddress("Target_E", &Target_E);
    flattree->SetBranchAddress("PiPlus_px", &PiPlus_px);
    flattree->SetBranchAddress("PiPlus_py", &PiPlus_py);
    flattree->SetBranchAddress("PiPlus_pz", &PiPlus_pz);
    flattree->SetBranchAddress("PiPlus_E", &PiPlus_E);
    flattree->SetBranchAddress("PiMinus_px", &PiMinus_px);
    flattree->SetBranchAddress("PiMinus_py", &PiMinus_py);
    flattree->SetBranchAddress("PiMinus_pz", &PiMinus_pz);
    flattree->SetBranchAddress("PiMinus_E", &PiMinus_E);
    flattree->SetBranchAddress("Proton_px", &Proton_px);
    flattree->SetBranchAddress("Proton_py", &Proton_py);
    flattree->SetBranchAddress("Proton_pz", &Proton_pz);  
    flattree->SetBranchAddress("Proton_E", &Proton_E);
    flattree->SetBranchAddress("Eta_px", &Eta_px);
    flattree->SetBranchAddress("Eta_py", &Eta_py);
    flattree->SetBranchAddress("Eta_pz", &Eta_pz);
    flattree->SetBranchAddress("Eta_E", &Eta_E);
    flattree->SetBranchAddress("Pi0_px", &Pi0_px);
    flattree->SetBranchAddress("Pi0_py", &Pi0_py);
    flattree->SetBranchAddress("Pi0_pz", &Pi0_pz);
    flattree->SetBranchAddress("Pi0_E", &Pi0_E);
    flattree->SetBranchAddress("Gamma1_px", &Gamma1_px);
    flattree->SetBranchAddress("Gamma1_py", &Gamma1_py);
    flattree->SetBranchAddress("Gamma1_pz", &Gamma1_pz);
    flattree->SetBranchAddress("Gamma1_E", &Gamma1_E);
    flattree->SetBranchAddress("Gamma2_px", &Gamma2_px);          
    flattree->SetBranchAddress("Gamma2_py", &Gamma2_py);
    flattree->SetBranchAddress("Gamma2_pz", &Gamma2_pz);
    flattree->SetBranchAddress("Gamma2_E", &Gamma2_E);
    flattree->SetBranchAddress("Gamma3_px", &Gamma3_px);          
    flattree->SetBranchAddress("Gamma3_py", &Gamma3_py);
    flattree->SetBranchAddress("Gamma3_pz", &Gamma3_pz);
    flattree->SetBranchAddress("Gamma3_E", &Gamma3_E);
    flattree->SetBranchAddress("Gamma4_px", &Gamma4_px);      
    flattree->SetBranchAddress("Gamma4_py", &Gamma4_py);
    flattree->SetBranchAddress("Gamma4_pz", &Gamma4_pz);
    flattree->SetBranchAddress("Gamma4_E", &Gamma4_E);

    // Other values
    flattree->SetBranchAddress("AccidentalWeight", &locHistAccidWeightFactor);     
    flattree->SetBranchAddress("SidebandWeight", &locWeight);
    flattree->SetBranchAddress("DeltaT_RF", &locDeltaT_RF); 

    // Initialize Desired Histograms       
    TH1F* dHist_InvariantMass_Pi0Eta = new TH1F("InvariantMass_Pi0Eta", ";m(#pi^{0} #eta); Entries/20 MeV", 125, 0.5, 3.0); 

    TLorentzVector BeamP4, TargetP4;
    TLorentzVector ProtonP4, EtaP4, Pi0P4, PiPlusP4, PiMinusP4;
    TLorentzVector Gamma1P4, Gamma2P4, Gamma3P4, Gamma4P4;
    TLorentzVector DecayPi01P4, DecayPi02P4; 

    // Used to create new Amptools formatted output file
    //#if 0
    float Weight;
    float E_Beam;
    float Px_Beam;
    float Py_Beam;
    float Pz_Beam;
    float Target_Mass;
    int locNumFinalStateParticles = 3;
    int nPart; 
    int PID_FinalState[3]; 
    float E_FinalState[3];
    float Px_FinalState[3];
    float Py_FinalState[3];
    float Pz_FinalState[3];
    
    
    TFile *outFile=new TFile(outFileName,"RECREATE");
    TTree *OutTree= new TTree("kin", "kin");

    OutTree->Branch("Weight", &Weight, "Weight/F");

    OutTree->Branch("E_Beam", &E_Beam, "E_Beam/F");
    OutTree->Branch("Px_Beam", &Px_Beam, "Px_Beam/F");
    OutTree->Branch("Py_Beam", &Py_Beam, "Py_Beam/F");
    OutTree->Branch("Pz_Beam", &Pz_Beam, "Pz_Beam/F");

    OutTree->Branch("Target_Mass", &Target_Mass, "Target_Mass/F");
    Target_Mass = 0.938272;          // Proton mass in GeV.

    OutTree->Branch("NumFinalState", &nPart,"NumFinalState/I");
    nPart = 3;

    OutTree->Branch("PID_FinalState", &PID_FinalState, "PID_FinalState[3]/I");                                                                                    
    PID_FinalState[0] = 14;//proton
    PID_FinalState[1] = 7;//pi0
    PID_FinalState[2] = 17;//eta

    OutTree->Branch("E_FinalState", &E_FinalState, "E_FinalState[3]/F");
    OutTree->Branch("Px_FinalState", &Px_FinalState, "Px_FinalState[3]/F");
    OutTree->Branch("Py_FinalState", &Py_FinalState, "Py_FinalState[3]/F");
    OutTree->Branch("Pz_FinalState", &Pz_FinalState, "Pz_FinalState[3]/F");
   
     
    // User input 
    int value = 0;
    Long64_t NEntries = 0;
    cout << "How many events to be processed? [Enter 0 for all events to be processed]" << endl;;
    cin >> value;

    if(value >= 0){
        NEntries = value;
        TotalEntries = flattree->GetEntries();
        if(NEntries > TotalEntries){
            cout << "!!!!!WARNING!!!!!" << endl;
            cout << "You gave more entries than available to process. Output will be incorect" << endl;
            cout << "____________________________" << endl;
        
    }}

    if(value == 0){                                                                                                                                                               
        NEntries = flattree->GetEntries();
    } 

    // Shows how many events to be processed 
    cout<<"There are "<< NEntries <<" entries"<<endl;
    cout<< "____________________________" << endl; 
    
    auto start = chrono::high_resolution_clock::now();
    for (Long64_t i_entry=0; i_entry<NEntries; i_entry++) {
        flattree->GetEntry(i_entry);
        cout.width(10); 
        cout << right << i_entry << " of " << NEntries << " events processed \r";
        
        // Defined user particle information created from DSelector 
        // (best to use since information was uniquely tracked)
        BeamP4.SetPxPyPzE(Beam_px, Beam_py, Beam_pz, Beam_E);
        TargetP4.SetPxPyPzE(Target_px, Target_py, Target_pz, Target_E);
        PiPlusP4.SetPxPyPzE(PiPlus_px, PiPlus_py, PiPlus_pz, PiPlus_E);
        PiMinusP4.SetPxPyPzE(PiMinus_px, PiMinus_py, PiMinus_pz, PiMinus_E);
        ProtonP4.SetPxPyPzE(Proton_px, Proton_py, Proton_pz, Proton_E); 
        Gamma1P4.SetPxPyPzE(Gamma1_px, Gamma1_py, Gamma1_pz, Gamma1_E);       
        Gamma2P4.SetPxPyPzE(Gamma2_px, Gamma2_py, Gamma2_pz, Gamma2_E);  
        Gamma3P4.SetPxPyPzE(Gamma3_px, Gamma3_py, Gamma3_pz, Gamma3_E);         
        Gamma4P4.SetPxPyPzE(Gamma4_px, Gamma4_py, Gamma4_pz, Gamma4_E);
         
        // Pre-defined particle information created when filling a flat tree
        #if 0
        BeamP4.SetPxPyPzE(beam_p4_kin->Px(), beam_p4_kin->Py(), beam_p4_kin->Pz() , beam_p4_kin->E());
        TargetP4.SetPxPyPzE(Target_px, Target_py, Target_pz, Target_E);
        PiPlusP4.SetPxPyPzE(pip_p4_kin->Px(), pip_p4_kin->Py(), pip_p4_kin->Pz(), pip_p4_kin->E());
        PiMinusP4.SetPxPyPzE(pim_p4_kin->Px(), pim_p4_kin->Py(), pim_p4_kin->Pz(), pim_p4_kin->E());
        ProtonP4.SetPxPyPzE(p_p4_kin->Px(), p_p4_kin->Py(), p_p4_kin->Pz(), p_p4_kin->E());
        Gamma1P4.SetPxPyPzE(g1_p4_kin->Px(), g1_p4_kin->Py(), g1_p4_kin->Pz(), g1_p4_kin->E());        
        Gamma2P4.SetPxPyPzE(g2_p4_kin->Px(), g2_p4_kin->Py(), g2_p4_kin->Pz(), g2_p4_kin->E());  
        Gamma3P4.SetPxPyPzE(g3_p4_kin->Px(), g3_p4_kin->Py(), g3_p4_kin->Pz(), g3_p4_kin->E());        
        Gamma4P4.SetPxPyPzE(g4_p4_kin->Px(), g4_p4_kin->Py(), g4_p4_kin->Pz(), g4_p4_kin->E());
        
        DecayPi01P4.SetPxPyPzE(decaypi01_p4_kin->Px(), decaypi01_p4_kin->Py(), decaypi01_p4_kin->Pz(), decaypi01_p4_kin->E());
        DecayPi02P4.SetPxPyPzE(decaypi02_p4_kin->Px(), decaypi02_p4_kin->Py(), decaypi02_p4_kin->Pz(), decaypi02_p4_kin->E()); 
        #endif
 
        //Beam and Target      
        TLorentzVector locBeamP4 = BeamP4;
        TLorentzVector locTargetP4 = TargetP4;

        //Intial Particles
        TLorentzVector locPi01P4 = Gamma1P4 + Gamma2P4;
        TLorentzVector locPi02P4 = Gamma3P4 + Gamma4P4;
        TLorentzVector locEtaP4 = Gamma3P4 + Gamma4P4 + PiPlusP4 + PiMinusP4;
        TLorentzVector locPipP4 = PiPlusP4;
        TLorentzVector locPimP4 = PiMinusP4;
        TLorentzVector locProtonP4 = ProtonP4;

        //Final State
        TLorentzVector locPi0EtaP4 = Gamma1P4 + Gamma2P4 + Gamma3P4 + Gamma4P4 + PiPlusP4 + PiMinusP4;
        
        double InvariantMass_Pi01_KinFit = locPi01P4.M();
        double InvariantMass_Pi02_KinFit = locPi02P4.M();
        double InvariantMass_Eta_KinFit = locEtaP4.M();
        double InvariantMass_Pip_KinFit = locPimP4.M();
        double InvariantMass_Pim_KinFit = locPimP4.M();   
        double InvariantMass_Proton_KinFit = locProtonP4.M();

        double InvariantMass_Pi0Eta_KinFit = locPi0EtaP4.M(); 

 
        //_______ Momentum Transfer Variables ________//
        Double_t loct = -1.*(locTargetP4 - locProtonP4).M2();

        //________ Boost Setup ________//
        TLorentzVector locInitialStateP4 = locBeamP4 + locTargetP4;
        TVector3 locBoostVector_CM = -1.0*(locInitialStateP4.BoostVector()); //negative due to coordinate system convention

        //Boosts the beam photon into the center of mass frame, etc...
        TLorentzVector locBeamP4_CM(locBeamP4); locBeamP4_CM.Boost(locBoostVector_CM);
        TLorentzVector locEtaP4_CM(locEtaP4); locEtaP4_CM.Boost(locBoostVector_CM);
        TLorentzVector locPi01P4_CM(locPi01P4); locPi01P4_CM.Boost(locBoostVector_CM);
        TLorentzVector locPi01EtaP4_CM(locPi0EtaP4); locPi01EtaP4_CM.Boost(locBoostVector_CM);

        //Boost to resonance (EtaPi01) frame for angular distributions
        TVector3 resonanceBoost = -locPi01EtaP4_CM.BoostVector();

        TLorentzVector  locBeamP4_Resonance(locBeamP4_CM); locBeamP4_Resonance.Boost(resonanceBoost);
        TLorentzVector locEtaP4_Resonance(locEtaP4_CM); locEtaP4_Resonance.Boost(resonanceBoost);
        TLorentzVector locPi01P4_Resonance(locPi01P4_CM); locPi01P4_Resonance.Boost(resonanceBoost);
        
        //________ Center of Mass Reference Frame ________// 
        double Eta_cosTheta_CM = locEtaP4_CM.CosTheta();
        double Eta_phi_CM = locEtaP4_CM.Phi();

        double Pi01_cosTheta_CM = locPi01P4_CM.CosTheta();
        double Pi01_phi_CM = locPi01P4_CM.Phi();


        // __________________________ Defined Cuts __________________________ //
        
        // Beam Energy Selection (needed if not selected in DSelector)
        if(locBeamP4.E() <= 8.2 || locBeamP4.E() >= 8.8) continue; 
    
        dHist_InvariantMass_Pi0Eta->Fill(InvariantMass_Pi0Eta_KinFit, locWeight*locHistAccidWeightFactor);
        
        
        // Fills the correct values for AmpTools
        Weight = locWeight*locHistAccidWeightFactor;
        E_Beam = locBeamP4.E();
        Px_Beam = locBeamP4.Px();
        Py_Beam = locBeamP4.Py();
        Pz_Beam = locBeamP4.Pz();
        E_FinalState[0] = locProtonP4.E();
        Px_FinalState[0] = locProtonP4.Px();
        Py_FinalState[0] = locProtonP4.Py();
        Pz_FinalState[0] = locProtonP4.Pz();
        E_FinalState[1] = locPi01P4.E();
        Px_FinalState[1] = locPi01P4.Px();
        Py_FinalState[1] = locPi01P4.Py();
        Pz_FinalState[1] = locPi01P4.Pz();
        E_FinalState[2] = locEtaP4.E();
        Px_FinalState[2] = locEtaP4.Px();
        Py_FinalState[2] = locEtaP4.Py();
        Pz_FinalState[2] = locEtaP4.Pz();
        

        OutTree->Fill();       
}
    cout << ""<< endl;   
    cout << "All events process"<< endl;
    auto diff = chrono::high_resolution_clock::now() - start;
    auto t1_minutes = chrono::duration_cast<chrono::minutes>(diff);
    cout << "Time it took to complete: " << (t1_minutes.count()) << " minutes" << endl; 
 
    // Draw desired histograms below
        gStyle->SetPalette(kBird);
        gStyle->SetOptStat(0);
 
        TCanvas *pi0eta = new TCanvas("pi0eta","pi0eta",900,600);
        pi0eta->Divide(1,1);
        pi0eta->cd(1); 
        dHist_InvariantMass_Pi0Eta->SetMinimum(0.0001);
        dHist_InvariantMass_Pi0Eta->SetFillColor(kYellow);
        dHist_InvariantMass_Pi0Eta->Draw("HIST");

    


    OutTree->Write();
    cout<< "____________________________" << endl;
    cout << "Updated Flat Tree:  " << outFileName << endl;
    outFile->Close(); 
return 0;}
