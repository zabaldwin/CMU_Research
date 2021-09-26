# CMU_Research
Files used for research at CMU's Hadronic Physics Group


This repository is a collection of files that will most definitely be used by students at CMU's Hadronic Physics Group. 

1) The hadd python script collects information from RCDB to seperate run-numbers into thier respected polarizations, as well as adds all polarizations together.
   
   -To run, simply use the command ./hadd_rcdb.py minrun-maxrun directory_pathway

2) jobPartition_Move_SLURM.sh is a very simple bash script that takes the users input of which partition they want to move their jobs to.

3) The submit_SLURM.sh and runDSelector.csh are scripts that are able to run over every TTree in a specified version by a pre-established DSelector. 
   
   -To run, simply use the command ./submit_SLURM.sh and answer the prompt
 
4) The example DSelectors show how to make basic cuts on events, how to uniquely track particles, and create fundamental branches to fill a flat tree. 
   
   -To run, simply use the command root runSelector_Example.C (to use without DPROOF-Lite, set ProofThreads = 0)
  
5) ProcessEvents_FlatTree_Example.C is a macro to loop over each event inside of a flat tree in order to remove the need of a DSelector (after a flattree is create of course). This script can perform fitting, cuts on combos, fill desired histograms, among other things (not shown in example, but an update soon to come).
   
   -Note: If creating a new, updated flat tree is not desired, comment out the specfic sections to speed up process (can use script to just draw desired histograms)

