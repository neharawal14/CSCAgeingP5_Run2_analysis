#!/bin/bash
echo "$1"
echo "$2"
cd /afs/cern.ch/work/n/nrawal/Brilcal_new_env/CMSSW_14_1_7/src/
cmsenv
cd /afs/cern.ch/work/n/nrawal/CSC_Run2_analysis/Updating_ntuples_code/Updating_HV_Lumi_Charge_intlumi_instlumi/
python3 Updating_lumi_ME11.py --chamber $1 --year $2
