#include <vector>                                                                                                                                                                                     
#include <iostream>
#include "TFile.h"
#include "TTree.h"

using namespace std;

void BestChi2Rank(){
    
    TString data_set[4] = {"30274_31057", "40856_42559", "50685_51768", "71350_73266"};
    TString pol[5] = {"AMO_", "PARA_0_", "PARA_135_", "PERP_45_", "PERP_90_"};
    
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 5; j++){

            // Input direct filename or loop over multiple files (like each run period, polarization, etc)
            //string filename = "pi0eta_Data_sum_FlatTreeFile_AMO_30274_31057_BasicInfo.root";
            string filename = "pi0eta_Data_sum_FlatTreeFile_";
            filename += pol[j];
            filename += data_set[i];
            filename += "_BasicInfo.root";
            cout << filename << endl;

            size_t pos = filename.rfind(".");
            string filename_filtered = filename.substr(0, pos) + "_filtered" + filename.substr(pos);

            TFile *inFile = TFile::Open(filename.c_str(), "READ");
            
            // Need TTree/Flattree key (once file is attached, gDirectory->ls())
            TTree *inTree = (TTree*)inFile->Get("pi0pi0pippim__B4");
            UInt_t run, kin_ndf;
            ULong64_t event;
            float kin_chisq;
            double locDeltaT_RF;
         
            inTree->SetBranchAddress("run", &run);
            inTree->SetBranchAddress("event", &event);
            inTree->SetBranchAddress("kin_chisq", &kin_chisq);
            inTree->SetBranchAddress("kin_ndf", &kin_ndf);
            // Only leaf needed that has to be self defined when creating FlatTree from DSelector 
            inTree->SetBranchAddress("DeltaT_RF", &locDeltaT_RF);
          
            inTree->GetEntry(0);
            UInt_t CurrentRun = run;
            ULong64_t CurrentEvent = event;
            float BestChi = kin_chisq/kin_ndf;
            int BestIndex = 0;
            vector<int> SelectedIndices;

            Long64_t NEntries = inTree->GetEntries();

            // Shows how many events to be processed 
            cout<<"There are "<< NEntries <<" entries"<<endl;
            cout<< "____________________________" << endl;

            //_______________________________ Process Start _______________________________//
            auto start = chrono::high_resolution_clock::now();
            for(int i_entry = 0; i_entry <= NEntries; i_entry++) {
                inTree->GetEntry(i_entry);
                cout.width(10); 
                cout << right << i_entry << " of " << NEntries << " events processed \r";

                // Cut RF accidentals out and only focus on in-time events                                
                if(fabs(locDeltaT_RF) >= 4.008*1.25) continue;

                // Determines the best chi-square per number of degrees of freedom for each event (no matter how many combos there are per event)
                if(i_entry == NEntries) { 
                    if(run == CurrentRun && event == CurrentEvent){                                                                                                                                                      
                        if((kin_chisq/kin_ndf) < BestChi) {
                        BestChi = kin_chisq/kin_ndf;
                        BestIndex = i_entry;
                        }
                    }

                    SelectedIndices.push_back(BestIndex);
                    BestChi = kin_chisq/kin_ndf;
                    BestIndex = i_entry;
                    CurrentRun = run;
                    CurrentEvent = event;
                }
                else {
                    if(run == CurrentRun && event == CurrentEvent) {
                        if((kin_chisq/kin_ndf) < BestChi) {
                            BestChi = kin_chisq/kin_ndf;
                            BestIndex = i_entry;
                        }
                    }
                    else { // If we are no longer in the same event/run, save the previous best index and reset
                        SelectedIndices.push_back(BestIndex);
                        BestChi = kin_chisq/kin_ndf;
                        BestIndex = i_entry;
                        CurrentRun = run;
                        CurrentEvent = event;
                    }
                }
            }
    
            cout << ""<< endl;   
            cout << "All events process"<< endl;
            auto diff = chrono::high_resolution_clock::now() - start;
            auto t1_minutes = chrono::duration_cast<chrono::minutes>(diff);
            cout << "Time it took to complete: " << (t1_minutes.count()) << " minutes" << endl;
        
            // Stores the filtered events with the combo that had the best chi-square back into a TTree with the same leaves defined in original input  
            TFile *outFile = new TFile(filename_filtered.c_str(), "RECREATE");
            TTree *outTree = inTree->CloneTree(0);

            for (int i_entry : SelectedIndices) {
                inTree->GetEntry(i_entry);
                outTree->Fill();
            }
              
            outTree->Write("", TObject::kOverwrite);
            outFile->Close();
            inFile->Close();
        }
    }
}                   
                  
