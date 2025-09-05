#!/bin/bash
echo "$1"
echo "$2"
cd /afs/cern.ch/work/n/nrawal/Brilcal_new_env/CMSSW_14_1_7/src/
cmsenv
cd /afs/cern.ch/work/n/nrawal/CSC_Run2_analysis/Counting_Z_events/Input_withSelection/
python3 Updating_lumi_dataset.py --year $1 --dataset $2
