#include "pressurecsc_2017.h"
#include "TFile.h"
#include "TTree.h"
#include <fstream>
#include <string>
#include <iostream>
#include "Rtypes.h"
#include "TROOT.h"
using namespace std;
//int Update_pressure_2017() {
int main(int argc, char *argv[]) {
    // Open input file and tree

    TString chamber =(TString) argv[1];
    TFile* infile = TFile::Open("/afs/cern.ch/user/n/nrawal/eos/CSCAgeing/Run2_Ntuples/2017_updated/csc_output_2017_"+chamber+"_tree_updated.root", "READ");
    //TFile* infile = TFile::Open("/eos/home-n/nrawal/CSCAgeing/Run2_Reprocessed/2017_all/csc_output_2017_"+chamber+"_tree.root", "READ");
    if (!infile || infile->IsZombie()) {
        std::cout << "Error opening input file." << std::endl;
        return 1;
    }

    TTree* intree = (TTree*)infile->Get("tree");
    if (!intree) {
        std::cout << "Tree 'tree' not found in file." << std::endl;
        infile->Close();
        return 1;
    }

    // Set up input branch
    Long64_t _timesecond = 0;
    Double_t _pressure; 
    Double_t pressure;
    intree->SetBranchAddress("_timesecond", &_timesecond);
    intree->SetBranchAddress("_pressure", &pressure);

    // Create output file and new tree
    TFile* outfile = TFile::Open("/afs/cern.ch/user/n/nrawal/eos/CSCAgeing/Run2_Ntuples/2017_pressure_updated/csc_output_2017_"+chamber+"_tree_updated.root", "RECREATE");
    TTree* outtree = intree->CloneTree(0); // Clone structure, no entries yet
    outtree->SetBranchAddress("_pressure", &pressure); // reuse same address
	

    Long64_t nentries = intree->GetEntries();
    std::cout << " Processing " << nentries << " entries..." << std::endl;

   for (Long64_t i = 0; i < nentries; ++i) {
   //for (Long64_t i = 0; i < 10000; ++i) {
        intree->GetEntry(i);
        pressure = getpressure2017(_timesecond);
	//std::cout<<" time "<<_timesecond<<" pressure "<<pressure<<std::endl;
        outtree->Fill();
    }

    outtree->Write();
    outfile->Close();
    infile->Close();
    std::cout << " Done: Output saved to output_with_pressure.root" << std::endl;
    return 0;
}
