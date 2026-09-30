#!/bin/bash
## The code is to execute the analysis code, depending on what code you execute
g++ -I $ROOTSYS/include ../AnalysisGasGain_optimising.cpp  `root-config --glibs` `root-config --libs` `root-config --cflags`  -L $ROOTSYS/lib -o executable_Gasgain_analysis_optimising
#g++ -I $ROOTSYS/include ../AnalysisGasGain_main_edited.cpp  `root-config --glibs` `root-config --libs` `root-config --cflags`  -L $ROOTSYS/lib -o executable_Gasgain_analysis_main_edited
#g++ -I $ROOTSYS/include ../AnalysisGasGain_main.cpp  `root-config --glibs` `root-config --libs` `root-config --cflags`  -L $ROOTSYS/lib -o executable_Gasgain_analysis_main
#g++ -I $ROOTSYS/include ../AnalysisGasGain_oddLumiBlock.cpp  `root-config --glibs` `root-config --libs` `root-config --cflags`  -L $ROOTSYS/lib -o executable_Gasgain_analysis_oddLumiBlock
#g++ -I $ROOTSYS/include ../AnalysisGasGain_evenLumiBlock.cpp  `root-config --glibs` `root-config --libs` `root-config --cflags`  -L $ROOTSYS/lib -o executable_Gasgain_analysis_evenLumiBlock
#g++ -I $ROOTSYS/include ../AnalysisGasGain_evenCFEB.cpp  `root-config --glibs` `root-config --libs` `root-config --cflags`  -L $ROOTSYS/lib -o executable_Gasgain_analysis_evenCFEB
#g++ -I $ROOTSYS/include ../AnalysisGasGain_oddCFEB.cpp  `root-config --glibs` `root-config --libs` `root-config --cflags`  -L $ROOTSYS/lib -o executable_Gasgain_analysis_oddCFEB
