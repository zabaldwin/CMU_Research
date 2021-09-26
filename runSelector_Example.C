// macro to process analysis TTree with TSelector
// to run from the terminal: root runSelector_Example.C
#include <iostream> 

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TSystem.h"
#include <stdlib.h>

  
void runSelector_Example(TString runNumber = "30274", int Proof_Nthreads = 8, TString myPath = "/raid4/zbaldwin/RunPeriod-2017-01/analysis/ver39/tree_pi0pi0pippim__B4/merged/" /*Fill out path to root desired TTree file*/)               
{

  // Load DSelector library
  gROOT->ProcessLine(".x $(ROOT_ANALYSIS_HOME)/scripts/Load_DSelector.C");

  // process signal 
  TString sampleDir = myPath;
  cout<<"running selector on files in: "<<sampleDir.Data()<<endl;
 
  TChain *chain = new TChain("pi0pi0pippim__B4_Tree");
  TSystemDirectory dir(sampleDir, sampleDir);
  TList *files = dir.GetListOfFiles();
  if(files) {
	  TSystemFile *file;
	  TString fileName;
	  TIter next(files);
	  
	  // loop over files
	  while ((file=(TSystemFile*)next())) {
		  fileName = file->GetName();
		  if(fileName.Contains(runNumber)) {
			  cout<<fileName.Data()<<endl;
			  
			  // check if file corrupted
			  TFile f(sampleDir+fileName);
			  if(f.TestBit(TFile::kRecovered)) {
				  cout<<"file corrupted -> skipping"<<endl;
				  continue;
			  }
			  if(f.IsZombie()) {
				  cout<<"file is a Zombie -> skipping"<<endl;
				  continue;
			  }
			  
			  // add file to chain
			  chain->Add(sampleDir+fileName);
		  }
	  }

	  cout<<"total entries in TChain = "<<chain->GetEntries()<<endl;
      gEnv->SetValue("ProofLite.Sandbox", "$PWD/.proof/"); // write all intermediate files to the local directory 	

	  DPROOFLiteManager::Process_Chain(chain, "DSelector_Example.C+", Proof_Nthreads, Form("hist_pi0eta_%s_Example.acc.root", runNumber.Data())); 
 }

  return;
}
