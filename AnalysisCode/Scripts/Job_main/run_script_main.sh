#!/bin/bash
echo "$1"
echo "$2"
echo "$3"
## Create suitable environment for your job
cd /afs/cern.ch/work/n/nrawal/Brilcal_new_env/CMSSW_14_1_7/src/
cmsenv
cd /afs/cern.ch/work/n/nrawal/CSC_Run2_analysis/AnalysisCode/Scripts/Job_main/
../executable_Gasgain_analysis_main $1 $2 $3 $4 $5
