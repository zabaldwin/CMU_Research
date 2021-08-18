# CMU_Research
Files used for research at CMU's Hadronic Physics Group


This repository is a collection of files that will most definitely be used by students at CMU's Hadronic Physics Group. 

1) The hadd python script collects information from RCDB to seperate run-numbers into thier respected polarizations, as well as adds all polarizations together.
   To run, simply use the command ./hadd_rcdb.py minrun-maxrun directory_pathway

2) jobPartition_Move_SLURM.sh is a very simple bash script that takes the users input of which partition they want to move their jobs to

3) The submit_SLURM.sh and runDSelector.csh are scripts that are able to run over every TTree in a specified version by a pre-established DSelector. 
   To run, simply use the command ./submit_SLURM.sh an answer the prompt

