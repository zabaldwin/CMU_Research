#!/bin/tcsh     

        #####################################################################################
        #                                                                                   #
        #        Tee C shell script used to run a DSelector on the SLURM queue @ CMU        # 
        #                              -Created by Zachary Baldwin, June 2019               #
        #                                                                                   # 
        #####################################################################################

#
#       The tcsh script that is called from submit_SLURM.sh that will run 
#                over a desired DSelector with defined inputs 
#
#
#

echo ----------------------JOB_INFO--------------------------------
echo 'Submitted from' $SLURM_SUBMIT_HOST
echo 'on' $SLURM_JOB_PARTITION 'queue'
echo 'Job identifier is' $SLURM_JOB_ID
echo 'Job name is' $SLURM_JOB_NAME
echo 'The username is' $SLURM_JOB_USER
echo -n 'Job is running on node '; echo $SLURM_JOB_NODELIST
echo --------------------------------------------------------------


echo ----------------------VARIABLE_INFO--------------------------------
echo $HOSTNAME
echo $MyRun
echo $MyCodeDir
echo $MyDataInDir
echo $MyDataOutDir
echo $MyProcess
echo $MySCP
echo -------------------------------------------------------------------

# Change into code directory so a copy can be made
cd $MyCodeDir
# Checks to see what is in code directory
ls -lhrt
echo $MyCodeDir

pwd
ls -al

# Add the PBS_ since this is what is called
echo $MyRunningDir
cd $MyRunningDir/PBS_${SLURM_JOB_ID}

# Set environments
setenv HALLD_MY $PWD
setenv PATH ${PWD}:$PATH
setenv LD_LIBRARY_PATH ${PWD}:$LD_LIBRARY_PATH

# Should source my setup.csh to make sure correct enviroment varibles are defined (i.e. $BMS_OSNAME) but may not be required
#cp /home/........./setup.csh ./
#source setup.csh
echo $BMS_OSNAME
env
echo 
echo

# Copy from the code directory which DSelector.C and .h files you want, along with rootlogon and runSelector or can not run over DSelector info
cp $MyCodeDir/DSelector.C ./
cp $MyCodeDir/DSelector.h ./
cp $MyCodeDir/runSelector.C ./
cp $MyCodeDir/rootlogon.C ./
echo

echo "Copying" $MyDataInDir
${MySCP} -l 10000 $MyDataInDir .      # limit scp to 10 MB/s

# See whats in scratch directory to make sure files copied over correctly
echo "This is what is in" 
echo $MyRunningDir/PBS_${SLURM_JOB_ID}
echo
ls -al

# Run over the run selector with the run value gathered from submit_SLURM.sh 
root.exe -l -b -q runSelector.C\(\"$MyRun\",\"$MyDataInDir\"\)
echo "runSelector has completed"
echo "_________________________"
echo "This is what is in"
echo $MyRunningDir/PBS_${SLURM_JOB_ID}
echo "after completing runSelector operation"
echo "_________________________"
echo
ls -al

# Now all that is left is to move the .root files out to Hist/TTree/FlatTree directory 
mv hist_*.root $MyDataOutDir
echo "Histograms have been transfered"
#mv tree_*.root $MyTreeOutDir
#echo "UpDated TTrees have been transfered"

# Sneaky way of renaming flattree output with correct run-number (not able to with TChain for some reason)
#mv FlatTree.root FlatTree_$MyRun.root
#echo "FlatTree has been renamed"
#mv FlatTree_$MyRun.root $MyFlatTreeOutDir
#echo "FlatTree has been transfered" 

exit 0


