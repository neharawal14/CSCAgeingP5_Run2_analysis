#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>
#include <TList.h>
#include <iostream>
#include "TPaveStats.h"

void canvas_plotting(TH1F*h1, TString chamber_name, TString title, TString type_HV,  TString year);
void analyze_root_files(TString directory, TString year) {
    // List of chamber names to process
 std::vector<TString> chamberNames = {
        "ME12HV1", "ME12HV2", "ME12HV3", "ME13HV1", "ME13HV2", "ME13HV3",
        "ME21HV1", "ME21HV2", "ME21HV3", "ME22HV1", "ME22HV2", "ME22HV3",
        "ME22HV4", "ME22HV5", "ME31HV1", "ME31HV2", "ME31HV3", "ME32HV1",
        "ME32HV2", "ME32HV3", "ME32HV4", "ME32HV5", "ME41HV1", "ME41HV2",
        "ME41HV3", "ME42HV1", "ME42HV2", "ME42HV3", "ME42HV4", "ME42HV5"
    }; 

//    std::vector<TString> chamberNames = {"ME12HV1" , "ME42HV3"};
 
    // Open the directory and iterate over files
//    TSystemDirectory dir(directory.c_str(), directory.c_str());
//    TList *files = dir.GetListOfFiles();
//    if (!files) return;
//
//    TIter next(files);
//    TSystemFile *file;
//    TString chamber_name;
//    while ((file = (TSystemFile*)next())) {

    for(int i=0; i<chamberNames.size(); i++){
      TString chamber_name = chamberNames[i];
      TString input_file = directory + year+"_updated_after_removal/csc_output_"+year+"_"+chamber_name+"_tree_updated.root";
    // Define histograms for the different run number ranges
    TH1F* h1_nominal = new TH1F("h1_nominal", "nominal HV for RunNb < 324077",  150, 3500, 3650);
  
    h1_nominal->SetLineColor(kRed);
    TH1F* h1_read = new TH1F("h1_read", "read HV for RunNb < 324077",  150, 3500, 3650);
   
    h1_read->SetLineColor(kRed);
    TH1F* diff_h1 = new TH1F("diff_h1", "(read HV - nominal HV) for RunNb < 324077", 20, -10, 10);
  
    diff_h1->SetLineColor(kRed);
    TH1F* h2_nominal = new TH1F("h2_nominal", "nominal HV for RunNb >= 324077",  150, 3500, 3650);
  
    h2_nominal->SetLineColor(kRed);
    TH1F* h2_read = new TH1F("h2_read", "read HV for RunNb >= 324077",  150, 3500, 3650);
   
    h2_read->SetLineColor(kRed);
    TH1F* diff_h2 = new TH1F("diff_h2", "(read HV - nominal HV) for RunNb >= 324077", 20, -10, 10);
  
    diff_h2->SetLineColor(kRed);

            TFile *root_file = TFile::Open(input_file);

            if (!root_file || root_file->IsZombie()) {
                std::cout << "Failed to open " <<  ". It may be corrupted.\n";
                continue;
            }

            TTree* tree;
            root_file->GetObject("tree", tree); // Adjust tree name

            if (!tree) {
                std::cout << "No TTree found in " <<  ". Skipping this file.\n";
                root_file->Close();
                continue;
            }

            // Set branch addresses
            Double_t rhsumQ, rhsumQ_RAW;
            Double_t  HV_nominal;
            Double_t  HV;
            ULong64_t runNb;
            tree->SetBranchAddress("_rhsumQ", &rhsumQ);
            tree->SetBranchAddress("_HV", &HV);
            tree->SetBranchAddress("_HV_nominal", &HV_nominal);
            tree->SetBranchAddress("_rhsumQ_RAW", &rhsumQ_RAW);
            tree->SetBranchAddress("_runNb", &runNb);

            Long64_t nentries = tree->GetEntries();

            // Loop over all entries
            for (Long64_t i = 0; i < nentries; ++i) {
                tree->GetEntry(i);

		if(runNb <324077){
                    h1_nominal->Fill(HV_nominal);
                    h1_read->Fill(HV);
                    diff_h1 ->Fill(HV-HV_nominal);
                  }
		else if(runNb >=324077){
                    h2_nominal->Fill(HV_nominal);
                    h2_read->Fill(HV);
                    diff_h2 ->Fill(HV-HV_nominal);
                  }
           }

            root_file->Close();

 
             h1_nominal->Scale(1./h1_nominal->Integral());
             h1_read->Scale(1./h1_read->Integral());
             diff_h1->Scale(1./diff_h1->Integral());
             h2_nominal->Scale(1./h2_nominal->Integral());
             h2_read->Scale(1./h2_read->Integral());
             diff_h2->Scale(1./diff_h2->Integral());

    // Draw all histograms on the same canvas
    canvas_plotting(h1_nominal, h2_nominal, chamber_name, "nominal HV", "nominal_HV", year);
    canvas_plotting(diff_h1, diff_h2, chamber_name, "read_HV - nominal HV ", "diff_HV", year);

    canvas_plotting(h1_read, h2_read, chamber_name, "read HV", "read_HV", year);
    }

}

void canvas_plotting(TH1F*h1,  TString chamber_name, TString title ,TString type_HV,  TString year){
TCanvas *c = new TCanvas("c", "HV Distributions", 800, 600);
    c->SetTitle(title+" : "+chamber_name);
    gStyle->SetOptTitle(0);
    gStyle->SetOptStat(0);
    h1->Draw("HIST");
//    h1->DrawNormalized("HIST");
    c->Modified(); // Update the canvas to process any pending drawing operations.
    c->Update();   // Force an immediate update of the canvas.
    h1->GetXaxis()->SetTitle(type_HV);
    h1->GetYaxis()->SetTitle(" nb. of entries");
   
 
//    h2->DrawNormalized("same");
    c->Modified();
    c->Update();

        // Update the canvas to ensure all drawing actions are processed
   // Manually creating stats boxes using TPaveText
    TPaveText *stats1 = new TPaveText(0.65, 0.75, 0.88, 0.88, "NDC");
    stats1->SetFillColor(0);
    stats1->SetTextAlign(12);
    stats1->AddText(year);
    stats1->AddText(Form("Entries: %.0f", h1->GetEntries()));
    stats1->AddText(Form("Mean: %.2f", h1->GetMean()));
    stats1->AddText(Form("Std Dev: %.2f", h1->GetStdDev()));
    stats1->AddText(Form("Integral: %.2f", h1->Integral()));
    stats1->AddText(Form("Underflow : %.2f", h1->GetBinContent(0)));
    stats1->AddText(Form("Overflow : %.2f", h1->GetBinContent(h1->GetNbinsX()+1)));
    stats1->SetTextColor(kRed);
    stats1->Draw("same");

    TLegend * legend = new TLegend(0.65,0.3, 0.88, 0.45);
    legend->AddEntry(h1, year);
    legend->Draw("same");
//    TPaveStats *st1 = (TPaveStats*)h1->GetListOfFunctions()->FindObject("stats");
//    TPaveStats *st2 = (TPaveStats*)h2->GetListOfFunctions()->FindObject("stats");

 // Modify positions to avoid overlap and ensure visibility
//        st1->SetX1NDC(0.75); st1->SetX2NDC(0.95);
//        st1->SetY1NDC(0.8);  st1->SetY2NDC(0.9);
//        st2->SetX1NDC(0.75); st2->SetX2NDC(0.95);
//        st2->SetY1NDC(0.7);  st2->SetY2NDC(0.8);
//
  // Add a title using TPaveText
    TPaveText *pt = new TPaveText(0.1, 0.93, 0.9, 0.99, "NDC");
    pt->AddText(title+" : " + chamber_name);
    pt->SetFillColor(0);
    pt->SetTextColor(1);
    pt->SetTextAlign(22); // Center alignment
    pt->SetTextFont(42); // Helvetica, normal
    pt->Draw("same");
//    st1->Draw("same");
//    st2->Draw("same");

//    c->BuildLegend();
    c->SaveAs("HV_"+year+"_plots/"+year+"_"+chamber_name+"_"+type_HV+"_Distributions.pdf");
}

void reading_HV_2017_2018() {
    TString directory_path = "/afs/cern.ch/user/n/nrawal/eos/CSCAgeing/Run2_Reprocessed/";
//   directory_path = argv[1]; // Optionally use directory path from command line
   TString year = "2017";
    analyze_root_files(directory_path, year);
}

