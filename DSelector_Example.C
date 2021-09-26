#include "DSelector_Example.h"

void DSelector_Example::Init(TTree *locTree)
{
	// USERS: IN THIS FUNCTION, ONLY MODIFY SECTIONS WITH A "USER" OR "EXAMPLE" LABEL. LEAVE THE REST ALONE.

	// The Init() function is called when the selector needs to initialize a new tree or chain.
	// Typically here the branch addresses and branch pointers of the tree will be set.
	// Init() will be called many times when running on PROOF (once per file to be processed).

	//USERS: SET OUTPUT FILE NAME //can be overriden by user in PROOF
	dOutputFileName = "Example.root"; //"" for none
	dOutputTreeFileName = ""; //"" for none
	dFlatTreeFileName = "FlatTree_Example.root"; //output flat tree (one combo per tree entry), "" for none
	dFlatTreeName = ""; //if blank, default name will be chosen
	//dSaveDefaultFlatBranches = true; // False: don't save default branches, reduce disk footprint.

	//Because this function gets called for each TTree in the TChain, we must be careful:
		//We need to re-initialize the tree interface & branch wrappers, but don't want to recreate histograms
	bool locInitializedPriorFlag = dInitializedFlag; //save whether have been initialized previously
	DSelector::Init(locTree); //This must be called to initialize wrappers for each new TTree
	//gDirectory now points to the output file with name dOutputFileName (if any)
	if(locInitializedPriorFlag)
		return; //have already created histograms, etc. below: exit

	Get_ComboWrappers();
	dPreviousRunNumber = 0;

	/*********************************** EXAMPLE USER INITIALIZATION: ANALYSIS ACTIONS **********************************/

	// EXAMPLE: Create deque for histogramming particle masses:
	// // For histogramming the phi mass in phi -> K+ K-
	// // Be sure to change this and dAnalyzeCutActions to match reaction
	std::deque<Particle_t> MyPhi;
	MyPhi.push_back(KPlus); MyPhi.push_back(KMinus);

	//ANALYSIS ACTIONS: //Executed in order if added to dAnalysisActions
	//false/true below: use measured/kinfit data

	//PID
	dAnalysisActions.push_back(new DHistogramAction_ParticleID(dComboWrapper, false));
	//below: value: +/- N ns, Unknown: All PIDs, SYS_NULL: all timing systems
	//dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper, false, 0.5, KPlus, SYS_BCAL));

	//PIDFOM (for charged tracks)
	dAnalysisActions.push_back(new DHistogramAction_PIDFOM(dComboWrapper));
	//dAnalysisActions.push_back(new DCutAction_PIDFOM(dComboWrapper, KPlus, 0.1));
	//dAnalysisActions.push_back(new DCutAction_EachPIDFOM(dComboWrapper, 0.1));

	//MASSES
	//dAnalysisActions.push_back(new DHistogramAction_InvariantMass(dComboWrapper, false, Lambda, 1000, 1.0, 1.2, "Lambda"));
	//dAnalysisActions.push_back(new DHistogramAction_MissingMassSquared(dComboWrapper, false, 1000, -0.1, 0.1));

	//KINFIT RESULTS
	dAnalysisActions.push_back(new DHistogramAction_KinFitResults(dComboWrapper));

	//CUT MISSING MASS
	//dAnalysisActions.push_back(new DCutAction_MissingMassSquared(dComboWrapper, false, -0.03, 0.02));

	//CUT ON SHOWER QUALITY
	//dAnalysisActions.push_back(new DCutAction_ShowerQuality(dComboWrapper, SYS_FCAL, 0.5));

	//BEAM ENERGY
	dAnalysisActions.push_back(new DHistogramAction_BeamEnergy(dComboWrapper, false));
	//dAnalysisActions.push_back(new DCutAction_BeamEnergy(dComboWrapper, false, 8.2, 8.8));  // Coherent peak for runs in the range 30000-59999

	//KINEMATICS
	dAnalysisActions.push_back(new DHistogramAction_ParticleComboKinematics(dComboWrapper, false));

	// ANALYZE CUT ACTIONS
	// // Change MyPhi to match reaction
	dAnalyzeCutActions = new DHistogramAction_AnalyzeCutActions( dAnalysisActions, dComboWrapper, false, 0, MyPhi, 1000, 0.9, 2.4, "CutActionEffect" );

	//INITIALIZE ACTIONS
	//If you create any actions that you want to run manually (i.e. don't add to dAnalysisActions), be sure to initialize them here as well
	Initialize_Actions();
	dAnalyzeCutActions->Initialize(); // manual action, must call Initialize()

    //Example: Creates subdirectories to sort histograms by specific information (e.g. beam energy,
    //         bins of -t, vanHove angle, invariant mass etc.) Also includes additional naming label
    //         for sorting purposes if desired.
    TDirectory *savdir = gDirectory;    
    TDirectory *adir = savdir->mkdir("InvariantMass"); //Subdirectory name

    TString locBeamEBinLabel[1] = {"Egamma8.2_8.8"}; //Naming label

    Int_t a;
    TDirectory *directory[1];
    for(a=0; a<1; a++){
        TString dirname = locBeamEBinLabel[a];
        directory[a] = adir->mkdir(dirname.Data());}
	/******************************** EXAMPLE USER INITIALIZATION: STAND-ALONE HISTOGRAMS *******************************/

	//EXAMPLE MANUAL HISTOGRAMS:
	dHist_MissingMassSquared = new TH1I("MissingMassSquared", ";Missing Mass Squared (GeV/c^{2})^{2}", 600, -0.06, 0.06);
	dHist_BeamEnergy = new TH1I("BeamEnergy", ";Beam Energy (GeV)", 600, 0.0, 12.0);

    //Will loop through the beam energies to create new histogram for each beam energy desired and sort it into correct subdirectory 
    for(int locBeamEBin=0; locBeamEBin<1; locBeamEBin++){
        directory[locBeamEBin]->cd();
        dHist_InvariantMass_Pi01Pi02PipPim_KinFit[locBeamEBin] = new TH1F("InvariantMass_Pi01Pi02PipPim_KinFit"+locBeamEBinLabel[locBeamEBin],";M(#pi^{0}#pi^{0}#pi^{+}#pi^{-}) (GeV/c^{2}); Entries/10 MeV", 350, 0.0, 3.5);
}


/************************** EXAMPLE USER INITIALIZATION: CUSTOM OUTPUT BRANCHES - MAIN TREE *************************/

//EXAMPLE MAIN TREE CUSTOM BRANCHES (OUTPUT ROOT FILE NAME MUST FIRST BE GIVEN!!!! (ABOVE: TOP)):
//The type for the branch must be included in the brackets
//1st function argument is the name of the branch
//2nd function argument is the name of the branch that contains the size of the array (for fundamentals only)
/*
dTreeInterface->Create_Branch_Fundamental<Int_t>("my_int"); //fundamental = char, int, float, double, etc.
dTreeInterface->Create_Branch_FundamentalArray<Int_t>("my_int_array", "my_int");
dTreeInterface->Create_Branch_FundamentalArray<Float_t>("my_combo_array", "NumCombos");
dTreeInterface->Create_Branch_NoSplitTObject<TLorentzVector>("my_p4");
dTreeInterface->Create_Branch_ClonesArray<TLorentzVector>("my_p4_array");
*/

	/************************** EXAMPLE USER INITIALIZATION: CUSTOM OUTPUT BRANCHES - FLAT TREE *************************/

	//EXAMPLE FLAT TREE CUSTOM BRANCHES (OUTPUT ROOT FILE NAME MUST FIRST BE GIVEN!!!! (ABOVE: TOP)):
	//The type for the branch must be included in the brackets
	//1st function argument is the name of the branch
	//2nd function argument is the name of the branch that contains the size of the array (for fundamentals only)
	/*
	dFlatTreeInterface->Create_Branch_Fundamental<Int_t>("flat_my_int"); //fundamental = char, int, float, double, etc.
	dFlatTreeInterface->Create_Branch_FundamentalArray<Int_t>("flat_my_int_array", "flat_my_int");
	dFlatTreeInterface->Create_Branch_NoSplitTObject<TLorentzVector>("flat_my_p4");
	dFlatTreeInterface->Create_Branch_ClonesArray<TLorentzVector>("flat_my_p4_array");
	*/


    //Example: Flat tree custom branch for each particle information found in a pi0 eta recation
    //         including beam, target, pi plus, pi minus, proton, and gamma values in px, py
    //         pz, and E. Note- other values including phi, T, etc. can be incorrporated as well.
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Beam_px"); //fundamental = char, int, float, double, etc.
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Beam_py");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Beam_pz");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Beam_E");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Target_px");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Target_py");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Target_pz");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Target_E");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("PiPlus_px");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("PiPlus_py");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("PiPlus_pz");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("PiPlus_E");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("PiMinus_px");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("PiMinus_py");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("PiMinus_pz");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("PiMinus_E");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Proton_px");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Proton_py");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Proton_pz");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Proton_E");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Eta_px");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Eta_py");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Eta_pz");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Eta_E");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Pi0_px");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Pi0_py");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Pi0_pz");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Pi0_E");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Gamma1_px");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Gamma1_py");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Gamma1_pz");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Gamma1_E");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Gamma2_px");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Gamma2_py");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Gamma2_pz");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Gamma2_E");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Gamma3_px");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Gamma3_py");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Gamma3_pz");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Gamma3_E");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Gamma4_px");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Gamma4_py");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Gamma4_pz");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Gamma4_E");
    
    //This branch fundamental will be used to check final state invariant mass (in order to make sure no mistakes
    //were made in filling out information
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("InvariantMass_Pi01Eta");

    //Other values can be stored in a flat tree in order to use them later
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("Chi2_NDF_KinFit");   
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("AccidentalWeight");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("SidebandWeight");
    dFlatTreeInterface->Create_Branch_Fundamental<Double_t>("MissingMassSquared");
	

    //Can store information in a specific format for further analysis if executable code requires it (i.e. Amptools,
    //PyPWA, etc.)
    //
    //Amptools format:
    dFlatTreeInterface->Create_Branch_Fundamental<Float_t>("Weight");
    dFlatTreeInterface->Create_Branch_Fundamental<Float_t>("E_Beam");
    dFlatTreeInterface->Create_Branch_Fundamental<Float_t>("Px_Beam");
    dFlatTreeInterface->Create_Branch_Fundamental<Float_t>("Py_Beam");
    dFlatTreeInterface->Create_Branch_Fundamental<Float_t>("Pz_Beam");
    dFlatTreeInterface->Create_Branch_Fundamental<Float_t>("Target_Mass");
    dFlatTreeInterface->Create_Branch_Fundamental<Int_t>("NumFinalState");
    dFlatTreeInterface->Create_Branch_FundamentalArray<Int_t>("PID_FinalState", "NumFinalState");
    dFlatTreeInterface->Create_Branch_FundamentalArray<Float_t>("E_FinalState", "NumFinalState");
    dFlatTreeInterface->Create_Branch_FundamentalArray<Float_t>("Px_FinalState", "NumFinalState");
    dFlatTreeInterface->Create_Branch_FundamentalArray<Float_t>("Py_FinalState", "NumFinalState");
    dFlatTreeInterface->Create_Branch_FundamentalArray<Float_t>("Pz_FinalState", "NumFinalState");
    /************************************* ADVANCED EXAMPLE: CHOOSE BRANCHES TO READ ************************************/

	//TO SAVE PROCESSING TIME
		//If you know you don't need all of the branches/data, but just a subset of it, you can speed things up
		//By default, for each event, the data is retrieved for all branches
		//If you know you only need data for some branches, you can skip grabbing data from the branches you don't need
		//Do this by doing something similar to the commented code below

	//dTreeInterface->Clear_GetEntryBranches(); //now get none
	//dTreeInterface->Register_GetEntryBranch("Proton__P4"); //manually set the branches you want

	/************************************** DETERMINE IF ANALYZING SIMULATED DATA *************************************/

	dIsMC = (dTreeInterface->Get_Branch("MCWeight") != NULL);

}

Bool_t DSelector_Example::Process(Long64_t locEntry)
{
	// The Process() function is called for each entry in the tree. The entry argument
	// specifies which entry in the currently loaded tree is to be processed.
	//
	// This function should contain the "body" of the analysis. It can contain
	// simple or elaborate selection criteria, run algorithms on the data
	// of the event and typically fill histograms.
	//
	// The processing can be stopped by calling Abort().
	// Use fStatus to set the return value of TTree::Process().
	// The return value is currently not used.

	//CALL THIS FIRST
	DSelector::Process(locEntry); //Gets the data from the tree for the entry
	//cout << "RUN " << Get_RunNumber() << ", EVENT " << Get_EventNumber() << endl;
	//TLorentzVector locProductionX4 = Get_X4_Production();

	/******************************************** GET POLARIZATION ORIENTATION ******************************************/

	//Only if the run number changes
	//RCDB environment must be setup in order for this to work! (Will return false otherwise)
	UInt_t locRunNumber = Get_RunNumber();
	if(locRunNumber != dPreviousRunNumber)
	{
		dIsPolarizedFlag = dAnalysisUtilities.Get_IsPolarizedBeam(locRunNumber, dIsPARAFlag);
		dPreviousRunNumber = locRunNumber;
	}

	/********************************************* SETUP UNIQUENESS TRACKING ********************************************/

	//ANALYSIS ACTIONS: Reset uniqueness tracking for each action
	//For any actions that you are executing manually, be sure to call Reset_NewEvent() on them here
	Reset_Actions_NewEvent();
	dAnalyzeCutActions->Reset_NewEvent(); // manual action, must call Reset_NewEvent()

	//PREVENT-DOUBLE COUNTING WHEN HISTOGRAMMING
		//Sometimes, some content is the exact same between one combo and the next
			//e.g. maybe two combos have different beam particles, but the same data for the final-state
		//When histogramming, you don't want to double-count when this happens: artificially inflates your signal (or background)
		//So, for each quantity you histogram, keep track of what particles you used (for a given combo)
		//Then for each combo, just compare to what you used before, and make sure it's unique

	//EXAMPLE 1: Particle-specific info:
	set<Int_t> locUsedSoFar_BeamEnergy; //Int_t: Unique ID for beam particles. set: easy to use, fast to search

	//EXAMPLE 2: Combo-specific info:
		//In general: Could have multiple particles with the same PID: Use a set of Int_t's
		//In general: Multiple PIDs, so multiple sets: Contain within a map
		//Multiple combos: Contain maps within a set (easier, faster to search)
	set<map<Particle_t, set<Int_t> > > locUsedSoFar_MissingMass;

	//INSERT USER ANALYSIS UNIQUENESS TRACKING HERE
        set<map<Particle_t, set<Int_t> > > locUsedSoFar_Pi01Pi02PipPimMass;
	/**************************************** EXAMPLE: FILL CUSTOM OUTPUT BRANCHES **************************************/

	/*
	Int_t locMyInt = 7;
	dTreeInterface->Fill_Fundamental<Int_t>("my_int", locMyInt);

	TLorentzVector locMyP4(4.0, 3.0, 2.0, 1.0);
	dTreeInterface->Fill_TObject<TLorentzVector>("my_p4", locMyP4);

	for(int loc_i = 0; loc_i < locMyInt; ++loc_i)
		dTreeInterface->Fill_Fundamental<Int_t>("my_int_array", 3*loc_i, loc_i); //2nd argument = value, 3rd = array index
	*/

	/************************************************* LOOP OVER COMBOS *************************************************/

	//Loop over combos
	for(UInt_t loc_i = 0; loc_i < Get_NumCombos(); ++loc_i)
	{
		//Set branch array indices for combo and all combo particles
		dComboWrapper->Set_ComboIndex(loc_i);

		// Is used to indicate when combos have been cut
		if(dComboWrapper->Get_IsComboCut()) // Is false when tree originally created
			continue; // Combo has been cut previously

		/********************************************** GET PARTICLE INDICES *********************************************/

		//Used for tracking uniqueness when filling histograms, and for determining unused particles

		//Step 0
		Int_t locBeamID = dComboBeamWrapper->Get_BeamID();
		Int_t locPiPlusTrackID = dPiPlusWrapper->Get_TrackID();
		Int_t locPiMinusTrackID = dPiMinusWrapper->Get_TrackID();
		Int_t locProtonTrackID = dProtonWrapper->Get_TrackID();

		//Step 1
		Int_t locPhoton1NeutralID = dPhoton1Wrapper->Get_NeutralID();
		Int_t locPhoton2NeutralID = dPhoton2Wrapper->Get_NeutralID();

		//Step 2
		Int_t locPhoton3NeutralID = dPhoton3Wrapper->Get_NeutralID();
		Int_t locPhoton4NeutralID = dPhoton4Wrapper->Get_NeutralID();

		/*********************************************** GET FOUR-MOMENTUM **********************************************/

		// Get P4's: //is kinfit if kinfit performed, else is measured
		//dTargetP4 is target p4
		//Step 0
		TLorentzVector locBeamP4 = dComboBeamWrapper->Get_P4();
		TLorentzVector locPiPlusP4 = dPiPlusWrapper->Get_P4();
		TLorentzVector locPiMinusP4 = dPiMinusWrapper->Get_P4();
		TLorentzVector locProtonP4 = dProtonWrapper->Get_P4();
		//Step 1
		TLorentzVector locDecayingPi01P4 = dDecayingPi01Wrapper->Get_P4();
		TLorentzVector locPhoton1P4 = dPhoton1Wrapper->Get_P4();
		TLorentzVector locPhoton2P4 = dPhoton2Wrapper->Get_P4();
		//Step 2
		TLorentzVector locDecayingPi02P4 = dDecayingPi02Wrapper->Get_P4();
		TLorentzVector locPhoton3P4 = dPhoton3Wrapper->Get_P4();
		TLorentzVector locPhoton4P4 = dPhoton4Wrapper->Get_P4();

		// Get Measured P4's:
		//Step 0
		TLorentzVector locBeamP4_Measured = dComboBeamWrapper->Get_P4_Measured();
		TLorentzVector locPiPlusP4_Measured = dPiPlusWrapper->Get_P4_Measured();
		TLorentzVector locPiMinusP4_Measured = dPiMinusWrapper->Get_P4_Measured();
		TLorentzVector locProtonP4_Measured = dProtonWrapper->Get_P4_Measured();
		//Step 1
		TLorentzVector locPhoton1P4_Measured = dPhoton1Wrapper->Get_P4_Measured();
		TLorentzVector locPhoton2P4_Measured = dPhoton2Wrapper->Get_P4_Measured();
		//Step 2
		TLorentzVector locPhoton3P4_Measured = dPhoton3Wrapper->Get_P4_Measured();
		TLorentzVector locPhoton4P4_Measured = dPhoton4Wrapper->Get_P4_Measured();

		/********************************************* GET COMBO RF TIMING INFO *****************************************/
        
        	//Very usefull code to perform accidental subtraction. Before hand, common to use (1/#BeamBunches) but
        	//was not specfic enough for each run number and did not skip nearest beam bunch in order to not subrtract 
        	//actual signal data. The below code is necessary to get correct photon data instead of incorrectly 
        	//tagged photons. 
		TLorentzVector locBeamX4_Measured = dComboBeamWrapper->Get_X4_Measured();
        	TLorentzVector locProtonX4_Measured = dProtonWrapper->Get_X4_Measured();

        	TLorentzVector locPhoton1X4_Measured = dPhoton1Wrapper->Get_X4_Shower();
        	TLorentzVector locPhoton2X4_Measured = dPhoton2Wrapper->Get_X4_Shower();
        	TLorentzVector locPhoton3X4_Measured = dPhoton3Wrapper->Get_X4_Shower();
        	TLorentzVector locPhoton4X4_Measured = dPhoton4Wrapper->Get_X4_Shower();

        
        	Double_t locBunchPeriod = dAnalysisUtilities.Get_BeamBunchPeriod(Get_RunNumber());
		Double_t locDeltaT_RF = dAnalysisUtilities.Get_DeltaT_RF(Get_RunNumber(), locBeamX4_Measured, dComboWrapper);
		Int_t locRelBeamBucket = dAnalysisUtilities.Get_RelativeBeamBucket(Get_RunNumber(), locBeamX4_Measured, dComboWrapper); // 0 for in-time events, non-zero integer for out-of-time photons
		Int_t locNumOutOfTimeBunchesInTree = 4; //YOU need to specify this number
		//Number of out-of-time beam bunches in tree (on a single side, so that total number out-of-time bunches accepted is 2 times this number for left + right bunches) 

		Bool_t locSkipNearestOutOfTimeBunch = true; // True: skip events from nearest out-of-time bunch on either side (recommended).
		Int_t locNumOutOfTimeBunchesToUse = locSkipNearestOutOfTimeBunch ? locNumOutOfTimeBunchesInTree-1:locNumOutOfTimeBunchesInTree; 
		Double_t locAccidentalScalingFactor = dAnalysisUtilities.Get_AccidentalScalingFactor(Get_RunNumber(), locBeamP4.E(), dIsMC); // Ideal value would be 1, but deviations require added factor, which is different for data and MC.
		Double_t locAccidentalScalingFactorError = dAnalysisUtilities.Get_AccidentalScalingFactorError(Get_RunNumber(), locBeamP4.E()); // Ideal value would be 1, but deviations observed, need added factor.
		Double_t locHistAccidWeightFactor = locRelBeamBucket==0 ? 1 : -locAccidentalScalingFactor/(2*locNumOutOfTimeBunchesToUse) ; // Weight by 1 for in-time events, ScalingFactor*(1/NBunches) for out-of-time
		if(locSkipNearestOutOfTimeBunch && abs(locRelBeamBucket)==1) { // Skip nearest out-of-time bunch: tails of in-time distribution also leak in
		dComboWrapper->Set_IsComboCut(true); 
		continue; 
		} 

		/********************************************* COMBINE FOUR-MOMENTUM ********************************************/

		// DO YOUR STUFF HERE

		// Combine 4-vectors
		TLorentzVector locMissingP4_Measured = locBeamP4_Measured + dTargetP4;
		locMissingP4_Measured -= locPiPlusP4_Measured + locPiMinusP4_Measured + locProtonP4_Measured + locPhoton1P4_Measured + locPhoton2P4_Measured + locPhoton3P4_Measured + locPhoton4P4_Measured;

        //Use TLorentzVector to define the particle information gathered from the correct wrappers. Best practice
        //is to use the kinamatic fitted values instead of measured (less background) but could be useful to see 
        //measured information (e.g. to check background leakage into particle data) 
        TLorentzVector locPhoton1P4_KinFit = locPhoton1P4;
        TLorentzVector locPhoton2P4_KinFit = locPhoton2P4;
        TLorentzVector locPhoton3P4_KinFit = locPhoton3P4;
        TLorentzVector locPhoton4P4_KinFit = locPhoton4P4;

        TLorentzVector locPhoton12P4_KinFit = locPhoton1P4 + locPhoton2P4;//First Pi0
        TLorentzVector locPhoton34P4_KinFit = locPhoton3P4 + locPhoton4P4;//Second Pi0
        TLorentzVector locPhoton13P4_KinFit = locPhoton1P4 + locPhoton3P4;
        TLorentzVector locPhoton24P4_KinFit = locPhoton2P4 + locPhoton4P4;
        TLorentzVector locPhoton14P4_KinFit = locPhoton1P4 + locPhoton4P4;
        TLorentzVector locPhoton23P4_KinFit = locPhoton2P4 + locPhoton3P4;

        //Could use the "DecayingPi0" information if desired but the same as photon combo info (shown below)
        TLorentzVector locPi01P4_KinFit = /*locDecayingPi01P4;*/locPhoton1P4 + locPhoton2P4;
        TLorentzVector locPi02P4_KinFit = /*locDecayingPi02P4;*/locPhoton3P4 + locPhoton4P4;
        TLorentzVector locPi01P4_Measured = locPhoton1P4_Measured + locPhoton2P4_Measured;
        TLorentzVector locPi02P4_Measured = locPhoton3P4_Measured + locPhoton4P4_Measured;

        TLorentzVector locEtaP4_KinFit = locPhoton1P4 + locPhoton2P4 + locPiPlusP4 + locPiMinusP4;
        TLorentzVector locEtaP4_Measured = locPhoton1P4_Measured + locPhoton2P4_Measured + locPiPlusP4 + locPiMinusP4;

        TLorentzVector locPipP4_KinFit = locPiPlusP4;
        TLorentzVector locPipP4_Measured = locPiPlusP4_Measured;

        TLorentzVector locPimP4_KinFit = locPiMinusP4;
        TLorentzVector locPimP4_Measured = locPiMinusP4_Measured;
		
        //FinalState Information
        TLorentzVector locPi01_EtaP4_KinFit = locPhoton1P4 + locPhoton2P4 + locPhoton3P4 + locPhoton4P4 + locPiPlusP4 + locPiMinusP4;

        /********************************************* FOUR-MOMENTUM VARIABLES ********************************************/
        Double_t InvariantMassProton_KinFit = locProtonP4.M();
        Double_t InvariantMassProton_Measured = locProtonP4_Measured.M();

        Double_t InvariantMassPi01_KinFit = locPi01P4_KinFit.M();
        Double_t InvariantMassPi01_Measured = locPi01P4_Measured.M();

        Double_t InvariantMassPi02_KinFit = locPi02P4_KinFit.M();
        Double_t InvariantMassPi02_Measured = locPi02P4_Measured.M();

        Double_t InvariantMassEta_KinFit = locEtaP4_KinFit.M();
        Double_t InvariantMassEta_Measured = locEtaP4_Measured.M();

        Double_t InvariantMassPip_KinFit = locPipP4_KinFit.M();
        Double_t InvariantMassPip_Measured = locPipP4_Measured.M();

        Double_t InvariantMassPim_KinFit = locPimP4_KinFit.M();
        Double_t InvariantMassPim_Measured = locPimP4_Measured.M();

        Double_t InvariantMass_Pi01Eta_KinFit = locPi01_EtaP4_KinFit.M();

        //Example of a Beam energy cut to select on the coherent peak 
        Int_t locBeamEBin = 0;
        if(locBeamP4.E() <= 8.2 || locBeamP4.E() >= 8.8) {
                dComboWrapper->Set_IsComboCut(true);
                continue;
         }

        /********************************************* REFERENCE FRAME SETUP ********************************************/
        //This defines the production CM frame using the initial state
        TLorentzVector locInitialStateP4 = locBeamP4 + dTargetP4;
        TVector3 locBoostVector_CM = -1.0*(locInitialStateP4.BoostVector()); //negative due to coordinate system convention
        
        //Boosts the beam photon into the center of mass frame, etc...
        TLorentzVector locBeamP4_CM(locBeamP4); locBeamP4_CM.Boost(locBoostVector_CM);
        TLorentzVector locEtaP4_CM(locEtaP4_KinFit); locEtaP4_CM.Boost(locBoostVector_CM);
        TLorentzVector locPi01P4_CM(locPi01P4_KinFit); locPi01P4_CM.Boost(locBoostVector_CM);

        //_______________________ Center of Mass Frame ________________________________
        double Eta_cosTheta_CM = locEtaP4_CM.CosTheta();
        double Eta_phi_CM = locEtaP4_CM.Phi();

        /********************************** UNUSED TRACKS CUT **********************************************/
        double locUnusedTracks = dComboWrapper->Get_NumUnusedTracks();

        if(locUnusedTracks > 3.0){
                dComboWrapper->Set_IsComboCut(true);
                continue;
        }
        
        //Sets up values for Amptools
        Double_t targetMass = 0.938;                                                                                             
        Int_t numFinalState = 3;
        Double_t locWeight = 1;
        /******************************************** EXECUTE ANALYSIS ACTIONS *******************************************/

		// Loop through the analysis actions, executing them in order for the active particle combo
		dAnalyzeCutActions->Perform_Action(); // Must be executed before Execute_Actions()
		if(!Execute_Actions()) //if the active combo fails a cut, IsComboCutFlag automatically set
			continue;

		//if you manually execute any actions, and it fails a cut, be sure to call:
			//dComboWrapper->Set_IsComboCut(true);

		/**************************************** EXAMPLE: FILL CUSTOM OUTPUT BRANCHES **************************************/

		/*
		TLorentzVector locMyComboP4(8.0, 7.0, 6.0, 5.0);
		//for arrays below: 2nd argument is value, 3rd is array index
		//NOTE: By filling here, AFTER the cuts above, some indices won't be updated (and will be whatever they were from the last event)
			//So, when you draw the branch, be sure to cut on "IsComboCut" to avoid these.
		dTreeInterface->Fill_Fundamental<Float_t>("my_combo_array", -2*loc_i, loc_i);
		dTreeInterface->Fill_TObject<TLorentzVector>("my_p4_array", locMyComboP4, loc_i);
		*/

		/**************************************** EXAMPLE: HISTOGRAM BEAM ENERGY *****************************************/

		//Histogram beam energy (if haven't already)
		if(locUsedSoFar_BeamEnergy.find(locBeamID) == locUsedSoFar_BeamEnergy.end())
		{
			dHist_BeamEnergy->Fill(locBeamP4.E()); // Fills in-time and out-of-time beam photon combos
			//dHist_BeamEnergy->Fill(locBeamP4.E(),locHistAccidWeightFactor); // Alternate version with accidental subtraction

			locUsedSoFar_BeamEnergy.insert(locBeamID);
		}

		/************************************ EXAMPLE: HISTOGRAM MISSING MASS SQUARED ************************************/

		//Missing Mass Squared
		double locMissingMassSquared = locMissingP4_Measured.M2();

		//Uniqueness tracking: Build the map of particles used for the missing mass
			//For beam: Don't want to group with final-state photons. Instead use "Unknown" PID (not ideal, but it's easy).
		map<Particle_t, set<Int_t> > locUsedThisCombo_MissingMass;
		locUsedThisCombo_MissingMass[Unknown].insert(locBeamID); //beam
		locUsedThisCombo_MissingMass[PiPlus].insert(locPiPlusTrackID);
		locUsedThisCombo_MissingMass[PiMinus].insert(locPiMinusTrackID);
		locUsedThisCombo_MissingMass[Proton].insert(locProtonTrackID);
		locUsedThisCombo_MissingMass[Gamma].insert(locPhoton1NeutralID);
		locUsedThisCombo_MissingMass[Gamma].insert(locPhoton2NeutralID);
		locUsedThisCombo_MissingMass[Gamma].insert(locPhoton3NeutralID);
		locUsedThisCombo_MissingMass[Gamma].insert(locPhoton4NeutralID);

		//compare to what's been used so far
		if(locUsedSoFar_MissingMass.find(locUsedThisCombo_MissingMass) == locUsedSoFar_MissingMass.end())
		{
			//unique missing mass combo: histogram it, and register this combo of particles
			dHist_MissingMassSquared->Fill(locMissingMassSquared); // Fills in-time and out-of-time beam photon combos
			//dHist_MissingMassSquared->Fill(locMissingMassSquared,locHistAccidWeightFactor); // Alternate version with accidental subtraction

			locUsedSoFar_MissingMass.insert(locUsedThisCombo_MissingMass);
		}

		//E.g. Cut
		//if((locMissingMassSquared < -0.04) || (locMissingMassSquared > 0.04))
		//{
		//	dComboWrapper->Set_IsComboCut(true);
		//	continue;
		//}


        /************************************************************************/

        //Uniqueness tracking makes ure particles are not double counted when filling histograms
        map<Particle_t, set<Int_t> > locUsedThisCombo_Pi01Pi02PipPimMass;
        locUsedThisCombo_Pi01Pi02PipPimMass[Unknown].insert(locBeamID); //beam
        locUsedThisCombo_Pi01Pi02PipPimMass[PiPlus].insert(locPiPlusTrackID);
        locUsedThisCombo_Pi01Pi02PipPimMass[PiMinus].insert(locPiMinusTrackID);
        locUsedThisCombo_Pi01Pi02PipPimMass[Gamma].insert(locPhoton1NeutralID);
        locUsedThisCombo_Pi01Pi02PipPimMass[Gamma].insert(locPhoton2NeutralID);
        locUsedThisCombo_Pi01Pi02PipPimMass[Gamma].insert(locPhoton3NeutralID);
        locUsedThisCombo_Pi01Pi02PipPimMass[Gamma].insert(locPhoton4NeutralID);

        if(locUsedSoFar_Pi01Pi02PipPimMass.find(locUsedThisCombo_Pi01Pi02PipPimMass) == locUsedSoFar_Pi01Pi02PipPimMass.end()){
            
            dHist_InvariantMass_Pi01Pi02PipPim_KinFit[locBeamEBin]->Fill(InvariantMass_Pi01Eta_KinFit);
            
            //Beam Information
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Beam_E", locBeamP4.E());
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Beam_px", locBeamP4.Px());
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Beam_py", locBeamP4.Py());
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Beam_pz", locBeamP4.Pz());
            
            //Target Information
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Target_E", dTargetP4.E());                                                              
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Target_px", dTargetP4.Px());
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Target_py", dTargetP4.Py());
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Target_pz", dTargetP4.Pz());

            //PiPlus Information
            dFlatTreeInterface->Fill_Fundamental<Double_t>("PiPlus_E", locPipP4_KinFit.E());                                                              
            dFlatTreeInterface->Fill_Fundamental<Double_t>("PiPlus_px", locPipP4_KinFit.Px());
            dFlatTreeInterface->Fill_Fundamental<Double_t>("PiPlus_py", locPipP4_KinFit.Py());
            dFlatTreeInterface->Fill_Fundamental<Double_t>("PiPlus_pz", locPipP4_KinFit.Pz());                
            
            //PiMinus Information
            dFlatTreeInterface->Fill_Fundamental<Double_t>("PiMinus_E", locPimP4_KinFit.E());                                                              
            dFlatTreeInterface->Fill_Fundamental<Double_t>("PiMinus_px", locPimP4_KinFit.Px());
            dFlatTreeInterface->Fill_Fundamental<Double_t>("PiMinus_py", locPimP4_KinFit.Py());
            dFlatTreeInterface->Fill_Fundamental<Double_t>("PiMinus_pz", locPimP4_KinFit.Pz()); 
            
            //Proton Information
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Proton_E", locProtonP4.E());                                                              
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Proton_px", locProtonP4.Px());
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Proton_py", locProtonP4.Py());
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Proton_pz", locProtonP4.Pz()); 

            //Eta Information
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Eta_E", locEtaP4_KinFit.E());                                                              
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Eta_px", locEtaP4_KinFit.Px());
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Eta_py", locEtaP4_KinFit.Py());
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Eta_pz", locEtaP4_KinFit.Pz()); 

            //Pi0 Information
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Pi0_E", locPi01P4_KinFit.E());                                                              
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Pi0_px", locPi01P4_KinFit.Px());
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Pi0_py", locPi01P4_KinFit.Py());
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Pi0_pz", locPi01P4_KinFit.Pz()); 

            //Gamma1 Information
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Gamma1_E", locPhoton1P4_KinFit.E());                                                              
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Gamma1_px", locPhoton1P4_KinFit.Px());
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Gamma1_py", locPhoton1P4_KinFit.Py());
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Gamma1_pz", locPhoton1P4_KinFit.Pz()); 

            //Gamma2 Information
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Gamma2_E", locPhoton2P4_KinFit.E());                                                              
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Gamma2_px", locPhoton2P4_KinFit.Px());
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Gamma2_py", locPhoton2P4_KinFit.Py());
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Gamma2_pz", locPhoton2P4_KinFit.Pz()); 

            //Gamma3 Information
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Gamma3_E", locPhoton3P4_KinFit.E());                             
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Gamma3_px", locPhoton3P4_KinFit.Px());
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Gamma3_py", locPhoton3P4_KinFit.Py());
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Gamma3_pz", locPhoton3P4_KinFit.Pz()); 

            //Gamma4 Information
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Gamma4_E", locPhoton4P4_KinFit.E());                         
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Gamma4_px", locPhoton4P4_KinFit.Px());                       
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Gamma4_py", locPhoton4P4_KinFit.Py());
            dFlatTreeInterface->Fill_Fundamental<Double_t>("Gamma4_pz", locPhoton4P4_KinFit.Pz());
            

            //Main Invariant Mass Information
            dFlatTreeInterface->Fill_Fundamental<Double_t>("InvariantMass_Pi01Eta", InvariantMass_Pi01Eta_KinFit);

            //Weight Information (can define weights (e.g. sidebandweight) before but now just using weight = 1)
            dFlatTreeInterface->Fill_Fundamental<Double_t>("AccidentalWeight", locHistAccidWeightFactor);                
            dFlatTreeInterface->Fill_Fundamental<Double_t>("SidebandWeight", locWeight);

            //AmpTools Information
            dFlatTreeInterface->Fill_Fundamental<Float_t>("Weight", locWeight*locHistAccidWeightFactor);

            dFlatTreeInterface->Fill_Fundamental<Float_t>("E_Beam", locBeamP4.E());
            dFlatTreeInterface->Fill_Fundamental<Float_t>("Px_Beam", locBeamP4.Px());
            dFlatTreeInterface->Fill_Fundamental<Float_t>("Py_Beam", locBeamP4.Py());
            dFlatTreeInterface->Fill_Fundamental<Float_t>("Pz_Beam", locBeamP4.Pz());

            dFlatTreeInterface->Fill_Fundamental<Float_t>("Target_Mass", targetMass);
            dFlatTreeInterface->Fill_Fundamental<Int_t>("NumFinalState", numFinalState);

            dFlatTreeInterface->Fill_Fundamental<Int_t>("PID_FinalState", 14, 0);
            dFlatTreeInterface->Fill_Fundamental<Float_t>("E_FinalState", locProtonP4.E(), 0);
            dFlatTreeInterface->Fill_Fundamental<Float_t>("Px_FinalState", locProtonP4.Px(), 0);
            dFlatTreeInterface->Fill_Fundamental<Float_t>("Py_FinalState", locProtonP4.Py(), 0);
            dFlatTreeInterface->Fill_Fundamental<Float_t>("Pz_FinalState", locProtonP4.Pz(), 0);
            dFlatTreeInterface->Fill_Fundamental<Int_t>("PID_FinalState", 7, 1);
            dFlatTreeInterface->Fill_Fundamental<Float_t>("E_FinalState", locPi01P4_KinFit.E(), 1);
            dFlatTreeInterface->Fill_Fundamental<Float_t>("Px_FinalState", locPi01P4_KinFit.Px(), 1);
            dFlatTreeInterface->Fill_Fundamental<Float_t>("Py_FinalState", locPi01P4_KinFit.Py(), 1);
            dFlatTreeInterface->Fill_Fundamental<Float_t>("Pz_FinalState", locPi01P4_KinFit.Pz(), 1);
            dFlatTreeInterface->Fill_Fundamental<Int_t>("PID_FinalState", 17, 2);
            dFlatTreeInterface->Fill_Fundamental<Float_t>("E_FinalState", locEtaP4_KinFit.E(), 2);
            dFlatTreeInterface->Fill_Fundamental<Float_t>("Px_FinalState", locEtaP4_KinFit.Px(), 2);
            dFlatTreeInterface->Fill_Fundamental<Float_t>("Py_FinalState", locEtaP4_KinFit.Py(), 2);
            dFlatTreeInterface->Fill_Fundamental<Float_t>("Pz_FinalState", locEtaP4_KinFit.Pz(), 2); 
     
        locUsedSoFar_Pi01Pi02PipPimMass.insert(locUsedThisCombo_Pi01Pi02PipPimMass);
        }
        else{
            dComboWrapper->Set_IsComboCut(true);
            continue; 
        }
		/****************************************** FILL FLAT TREE (IF DESIRED) ******************************************/

		/*
		//FILL ANY CUSTOM BRANCHES FIRST!!
		Int_t locMyInt_Flat = 7;
		dFlatTreeInterface->Fill_Fundamental<Int_t>("flat_my_int", locMyInt_Flat);

		TLorentzVector locMyP4_Flat(4.0, 3.0, 2.0, 1.0);
		dFlatTreeInterface->Fill_TObject<TLorentzVector>("flat_my_p4", locMyP4_Flat);

		for(int loc_j = 0; loc_j < locMyInt_Flat; ++loc_j)
		{
			dFlatTreeInterface->Fill_Fundamental<Int_t>("flat_my_int_array", 3*loc_j, loc_j); //2nd argument = value, 3rd = array index
			TLorentzVector locMyComboP4_Flat(8.0, 7.0, 6.0, 5.0);
			dFlatTreeInterface->Fill_TObject<TLorentzVector>("flat_my_p4_array", locMyComboP4_Flat, loc_j);
		}
		*/

		//FILL FLAT TREE
		Fill_FlatTree(); //for the active combo
	} // end of combo loop

	//FILL HISTOGRAMS: Num combos / events surviving actions
	Fill_NumCombosSurvivedHists();

	/******************************************* LOOP OVER THROWN DATA (OPTIONAL) ***************************************/
/*
	//Thrown beam: just use directly
	if(dThrownBeam != NULL)
		double locEnergy = dThrownBeam->Get_P4().E();

	//Loop over throwns
	for(UInt_t loc_i = 0; loc_i < Get_NumThrown(); ++loc_i)
	{
		//Set branch array indices corresponding to this particle
		dThrownWrapper->Set_ArrayIndex(loc_i);

		//Do stuff with the wrapper here ...
	}
*/
	/****************************************** LOOP OVER OTHER ARRAYS (OPTIONAL) ***************************************/
/*
	//Loop over beam particles (note, only those appearing in combos are present)
	for(UInt_t loc_i = 0; loc_i < Get_NumBeam(); ++loc_i)
	{
		//Set branch array indices corresponding to this particle
		dBeamWrapper->Set_ArrayIndex(loc_i);

		//Do stuff with the wrapper here ...
	}

	//Loop over charged track hypotheses
	for(UInt_t loc_i = 0; loc_i < Get_NumChargedHypos(); ++loc_i)
	{
		//Set branch array indices corresponding to this particle
		dChargedHypoWrapper->Set_ArrayIndex(loc_i);

		//Do stuff with the wrapper here ...
	}

	//Loop over neutral particle hypotheses
	for(UInt_t loc_i = 0; loc_i < Get_NumNeutralHypos(); ++loc_i)
	{
		//Set branch array indices corresponding to this particle
		dNeutralHypoWrapper->Set_ArrayIndex(loc_i);

		//Do stuff with the wrapper here ...
	}
*/

	/************************************ EXAMPLE: FILL CLONE OF TTREE HERE WITH CUTS APPLIED ************************************/
/*
	Bool_t locIsEventCut = true;
	for(UInt_t loc_i = 0; loc_i < Get_NumCombos(); ++loc_i) {
		//Set branch array indices for combo and all combo particles
		dComboWrapper->Set_ComboIndex(loc_i);
		// Is used to indicate when combos have been cut
		if(dComboWrapper->Get_IsComboCut())
			continue;
		locIsEventCut = false; // At least one combo succeeded
		break;
	}
	if(!locIsEventCut && dOutputTreeFileName != "")
		Fill_OutputTree();
*/

	return kTRUE;
}

void DSelector_Example::Finalize(void)
{
	//Save anything to output here that you do not want to be in the default DSelector output ROOT file.

	//Otherwise, don't do anything else (especially if you are using PROOF).
		//If you are using PROOF, this function is called on each thread,
		//so anything you do will not have the combined information from the various threads.
		//Besides, it is best-practice to do post-processing (e.g. fitting) separately, in case there is a problem.

	//DO YOUR STUFF HERE

	//CALL THIS LAST
	DSelector::Finalize(); //Saves results to the output file
}
