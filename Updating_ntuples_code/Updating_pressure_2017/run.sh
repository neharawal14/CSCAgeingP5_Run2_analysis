#!/bin/bash
arg1=$1
arg2=$2
cd /afs/cern.ch/work/n/nrawal/CSC_Run2_analysis/Updating_ntuples_code/Updating_pressure_2017/
echo './executable year chamber'
./executable $arg2 $arg1
