#!/bin/bash
arg1=$1
arg2=$2
cd /afs/cern.ch/user/n/nrawal/work/Brilcal_new_env/CMSSW_14_1_7/src/
cmsenv
cd /afs/cern.ch/work/n/nrawal/CSC_Run2_analysis/Updating_ntuples_code/Updating_recuperated_lowbx_VanderMeer_new_files/
echo './executable year chamber'
./executable $arg2 $arg1
