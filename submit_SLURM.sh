#!/bin/sh                                                                                                                                                                         

#***********************************MAKE SURE TO RUN ON ALBERT ONLY************************************#

        #####################################################################################
        #                                                                                   #
        #  Bourne shell script for submitting a job to the SLURM queue @ CMU using SBATCH   # 
        #                              -Created by Zachary Baldwin                          #
        #                                                                                   # 
        #####################################################################################

#
#       This bash script has the option of running over a single TTree or multiple TTrees.
#
#
#
#

# Can change depending on desired partition 
QUEUE=red

SLEEP=3

if [ "$QUEUE" == "green" ]; then
  CPUMEM=1590
  THREADS=40
elif [ "$QUEUE" == "red" ]; then
  CPUMEM=1990
  THREADS=32
elif [ "$QUEUE" == "blue" ]; then
  CPUMEM=990
  THREADS=8
else
  CPUMEM=990
fi

let MEM=$THREADS*CPUMEM

# Declare any process name i.e. pi0pi0pippim
MyProcess=

# Input pathway to DSelector and runSelector of choice
MyCodeDir=

# Input pathway to desired TTrees
MyDataInDir=

# Input specific TTree base name i.e. tree_pi0pi0pippim__B4
MyTreeName=

# Input specific pathway this bash script will be executed 
MyBashPath=

# Input pathway to /raid directory where histogram/TTree/FlatTree file will go
# (must specify atleast one pathway)
#
#          Example 
#
#          MyDataOutDirVar=pi0pi0pippim__B4
#          MyDataOutDir=/raid4/zbaldwin/RunPeriod-2018-08/${MyDataOutDirDir}/

MyDataOutDir=
#MyTreeOutDir=
#MyFlatTreeOutDir=


MyLogDir=$MyDataOutDir/../log/${MyDataOutDirDir}

# Makes sure that jobs run on scratch directory made when new job runs
MyRunningDir=/scratch
MySCP=/usr/bin/scp

# Creates directory pathways
mkdir -p $MyDataOutDir
mkdir -p $MyTreeOutDir
mkdir -p $MyLogDir

echo These are the pathways that were specified
echo "Input Directory: $MyDataInDir"
echo "Output Histogram Directory: $MyDataOutDir"
echo "Log Directory: $MyLogDir"
echo 
echo


echo "Submitting Jobs:"
echo
echo "Do you want to submit specific run-numbers or all run-numbers? "
echo "Hit space for speccifc run-numbers, type 'all' for every run-number"
read Varname


count=1
if [[ $Varname == "all" ]]; then 
    
    # Will now gather the run-number associated in the name of TTree
    
    #FOR ALL THE RUN NUMBERS
    for MyRun in ${MyDataInDir}/*; do
 

    MyRun=`basename ${MyRun}`
    
        #________ FOR THE TTREES ____________#
        # Must update for specfic TTree filenames
    MyRun=${MyRun:23:5}  # this gets run number from filename   Ex.  tree_pi0pi0pippim__B4_030279.root
                         #                                           12345678901234567890123  so 23 letters in name until the 3
    
    # This echo will make sure you are getting out the correct 5 run-numbers associated with the TTree file
    echo $MyRun
    
    # Submits a unqie job into SLURM
    sbatch --job-name=${MyProcess}_${MyRun} --ntasks=${THREADS}  --partition=${QUEUE} --time=00:40:00 --exclude=qcdcomp-0-0 --output=$MyLogDir/${MyProcess}_${MyRun}.out --error=$MyLogDir/${MyProcess}_${MyRun}.err --export=MyRun=$MyRun,MyDataInDir=$MyDataInDir,MyDataOutDir=$MyDataOutDir,MyTreeOutDir=$MyTreeOutDir,MyCodeDir=$MyCodeDir,MyEnv=$MyEnv,MyProcess=$MyProcess,MyRunningDir=$MyRunningDir,MyDataOutDirDir=$MyDataOutDirDir,MySCP=$MySCP $MyBashPath/runDSelector.csh 
    
    # In the past SLURM had issues, this sleep command fixed that (ISSUE RESOLVED JAN 2020)
    #sleep $SLEEP
  
    # Normally exclude one node for other peoples use

    done
    
else
    #FOR A SPECIFIC RUNNUMBER DESIGNATED BELOW (Can be single run number or multiple)
    
    RunNumberList="030274"
    
    
    arrRunNumberList=($RunNumberList)

    i=1
    while [ "$i" -le "${#arrRunNumberList[@]}" ]; do
            RunNumber="${arrRunNumberList[i-1]:1:5}"

            echo $RunNumber
            i=$(($i + 1))

        for MyRun in ${MyDataInDir}/${MyTreeName}_0${RunNumber}.root; do 
        

        MyRun=`basename ${MyRun}`

        MyRun=${MyRun:23:5}  # get run number from filename     tree_pi0pi0pippim__B4_030279.root
                             #                                  12345678901234567890123  so 23 letters in name until the 3
        
        # Echos myrun to make sure run number matches (if not, there are issues)
        echo $MyRun
    
        sbatch --job-name=${MyProcess}_${MyRun} --ntasks=${THREADS} --partition=${QUEUE} --exclude=qcdcomp-0-0 --time=10:00:00 --output=$MyLogDir/${MyProcess}_${MyRun}.out --error=$MyLogDir/${MyProcess}_${MyRun}.err --export=MyRun=$MyRun,MyDataInDir=$MyDataInDir,MyDataOutDir=$MyDataOutDir,MyTreeOutDir=$MyTreeOutDir,MyCodeDir=$MyCodeDir,MyEnv=$MyEnv,MyProcess=$MyProcess,MyRunningDir=$MyRunningDir,MyDataOutDirDir=$MyDataOutDirDir,MySCP=$MySCP $MyBashPath/runDSelector.csh
      

        done
    done

fi


