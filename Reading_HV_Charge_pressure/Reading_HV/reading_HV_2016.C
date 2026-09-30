#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>
#include <TList.h>
#include <iostream>
#include "TPaveStats.h"

void canvas_plotting(TH1F*h1, TH1F *h2, TH1F *h3, TString chamber_name, TString type_HV, TString save_name);
void analyze_root_files(TString directory) {
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

    TFile *output_file = new TFile("HV_diff_information_nonME11_2016.root","RECREATE");

    for(int i=0; i<chamberNames.size(); i++){
      TString chamber_name = chamberNames[i];
      TString input_file = directory + "2016_updated_after_removal/csc_output_2016_"+chamber_name+"_tree_updated.root";
    // Define histograms for the different run number ranges
    TH1F* h1_nominal = new TH1F("h1_nominal", "nominal HV for RunNb < 277792",  150, 3500, 3650);
    TH1F* h2_nominal = new TH1F("h2_nominal", "nominal HV for RunNb >= 277792 and RunNb <281613 ", 150, 3500, 3650);
    TH1F* h3_nominal = new TH1F("h3_nominal", "nominal HV for RunNb >= 281613 ", 150, 3500, 3650);
   
    h1_nominal->SetLineColor(kRed);
    h2_nominal->SetLineColor(kGreen);
    h3_nominal->SetLineColor(kBlue);

    TH1F* h1_read = new TH1F("h1_read", "read HV for RunNb < 277792",  150, 3500, 3650);
    TH1F* h2_read = new TH1F("h2_read", "read HV for RunNb >= 277792 and RunNb <281613 ", 150, 3500, 3650);
    TH1F* h3_read = new TH1F("h3_read", "read HV for RunNb >= 281613 ", 150, 3500, 3650);

   
    h1_read->SetLineColor(kRed);
    h2_read->SetLineColor(kGreen);
    h3_read->SetLineColor(kBlue);

    TH1F* diff_h1 = new TH1F("diff_h1", "(read HV - nominal HV) for RunNb < 277792", 40, -20, 20);
    TH1F* diff_h2 = new TH1F("diff_h2", "(read HV - nominal HV) for RunNb >= 277792 and RunNb <281613 ", 40, -20, 20);
    TH1F* diff_h3 = new TH1F("diff_h3", "(read HV - nominal HV) for RunNb >= 281613 ", 40, -20, 20);
   
    diff_h1->SetLineColor(kRed);
    diff_h2->SetLineColor(kGreen);
    diff_h3->SetLineColor(kBlue);

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

                if (runNb < 277792) {
                    h1_nominal->Fill(HV_nominal);
                    h1_read->Fill(HV);
                    diff_h1 ->Fill(HV-HV_nominal);
                } 
                else if (277792 <= runNb && runNb < 281613) {
                    h2_nominal->Fill(HV_nominal);
                    h2_read->Fill(HV);
                    diff_h2 ->Fill(HV-HV_nominal);
                } 

             else if(runNb >=281613){
                    h3_nominal->Fill(HV_nominal);
                    h3_read->Fill(HV);
                    diff_h3 ->Fill(HV-HV_nominal);
                }
            }

            root_file->Close();

 
             h1_nominal->Scale(1./h1_nominal->Integral());
             h2_nominal->Scale(1./h2_nominal->Integral());
             h3_nominal->Scale(1./h3_nominal->Integral());
             h1_read->Scale(1./h1_read->Integral());
             h2_read->Scale(1./h2_read->Integral());
             h3_read->Scale(1./h3_read->Integral());
             diff_h1->Scale(1./diff_h1->Integral());
             diff_h2->Scale(1./diff_h2->Integral());
             diff_h3->Scale(1./diff_h3->Integral());

    // Draw all histograms on the same canvas
    diff_h1->SetName("diff_h1_"+chamber_name);
    diff_h2->SetName("diff_h2_"+chamber_name);
    diff_h3->SetName("diff_h3_"+chamber_name);

    output_file->cd();
    diff_h1->Write(); 
    diff_h2->Write(); 
    diff_h3->Write(); 

    canvas_plotting(h1_nominal,h2_nominal,h3_nominal, chamber_name, "nominal HV", "nominal_HV");
    canvas_plotting(diff_h1,diff_h2,diff_h3, chamber_name, "read_HV - nominal HV ", "diff_HV");
    canvas_plotting(h1_read,h2_read,h3_read, chamber_name, "read HV", "read_HV");
    }

    output_file->Close();
}

void canvas_plotting(TH1F*h1, TH1F *h2, TH1F *h3, TString chamber_name, TString type_HV, TString save_name){
TCanvas *c = new TCanvas("c", "HV Distributions", 800, 600);
    c->SetTitle(type_HV+" : "+chamber_name);
    gStyle->SetOptTitle(0);
    gStyle->SetOptStat(0);
    h1->Draw("HIST");
//    h1->DrawNormalized("HIST");
    c->Modified(); // Update the canvas to process any pending drawing operations.
    c->Update();   // Force an immediate update of the canvas.
    //h2->DrawNormalized("same");
    h2->Draw("HIST same");
    c->Modified(); // Update the canvas to process any pending drawing operations.
    c->Update();   // Force an immediate update of the canvas.

    h3->Draw("HIST same");
    h3->GetXaxis()->SetTitle(type_HV);
    h3->GetYaxis()->SetTitle("nb of entries");

    h2->GetXaxis()->SetTitle(type_HV);
    h2->GetYaxis()->SetTitle("nb. of entries");
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
    stats1->AddText("runNb <277792 ");
    stats1->AddText(Form("Entries: %.0f", h1->GetEntries()));
    stats1->AddText(Form("Mean: %.2f", h1->GetMean()));
    stats1->AddText(Form("Std Dev: %.2f", h1->GetStdDev()));
    stats1->AddText(Form("Integral: %.2f", h1->Integral()));
    stats1->AddText(Form("Underflow : %.2f", h1->GetBinContent(0)));
    stats1->AddText(Form("Overflow : %.2f", h1->GetBinContent(h1->GetNbinsX()+1)));
    stats1->SetTextColor(kRed);
    stats1->Draw("same");

    TPaveText *stats2 = new TPaveText(0.65, 0.6, 0.88, 0.73, "NDC");
    stats2->SetFillColor(0);
    stats2->SetTextAlign(12);
    stats2->AddText("277792 <= runNb <281613 ");
    stats2->AddText(Form("Entries: %.0f", h2->GetEntries()));
    stats2->AddText(Form("Mean: %.2f", h2->GetMean()));
    stats2->AddText(Form("Std Dev: %.2f", h2->GetStdDev()));
    stats2->AddText(Form("Integral: %.2f", h2->Integral()));
    stats2->AddText(Form("Underflow : %.2f", h2->GetBinContent(0)));
    stats2->AddText(Form("Overflow : %.2f", h2->GetBinContent(h2->GetNbinsX()+1)));

    stats2->SetTextColor(kGreen);
    stats2->Draw("same");

    TPaveText *stats3 = new TPaveText(0.65, 0.46, 0.88, 0.59, "NDC");
    stats3->SetFillColor(0);
    stats3->SetTextAlign(12);
    stats3->AddText("runNb >=281613 ");
    stats3->AddText(Form("Entries: %.0f", h3->GetEntries()));
    stats3->AddText(Form("Mean: %.2f", h3->GetMean()));
    stats3->AddText(Form("Std Dev: %.2f", h3->GetStdDev()));
    stats3->AddText(Form("Integral: %.2f", h3->Integral()));
    stats3->AddText(Form("Underflow : %.2f", h3->GetBinContent(0)));
    stats3->AddText(Form("Overflow : %.2f", h3->GetBinContent(h3->GetNbinsX()+1)));

    stats3->SetTextColor(kBlue);
    stats3->Draw("same");

    TLegend * legend = new TLegend(0.65,0.3, 0.88, 0.45);
    legend->AddEntry(h1, "RunNb <277792");
    legend->AddEntry(h2, " 277792 <= RunNb <281613");
    legend->AddEntry(h3, "  RunNb >=281613");

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
    pt->AddText(type_HV+" : " + chamber_name);
    pt->SetFillColor(0);
    pt->SetTextColor(1);
    pt->SetTextAlign(22); // Center alignment
    pt->SetTextFont(42); // Helvetica, normal
    pt->Draw("same");
//    st1->Draw("same");
//    st2->Draw("same");

//    c->BuildLegend();
    c->SaveAs("HV_2016_plots_without_removal/2016_"+chamber_name+"_"+save_name+"_Distributions.pdf");

}

void reading_HV_2016() {
    TString directory_path = "/afs/cern.ch/user/n/nrawal/eos/CSCAgeing/Run2_Ntuples/";
//   directory_path = argv[1]; // Optionally use directory path from command line
    analyze_root_files(directory_path);
}

