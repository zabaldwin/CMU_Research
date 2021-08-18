#!/bin/tcsh                                                                                                                                                                       

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
echo $MyTreeOutDir
echo $MyDataOutDirVar
echo $MyEnv
echo $MyProcess
echo $MySCP
echo -------------------------------------------------------------------
cd $MyCodeDir
ls -lhrt
echo $MyCodeDir
