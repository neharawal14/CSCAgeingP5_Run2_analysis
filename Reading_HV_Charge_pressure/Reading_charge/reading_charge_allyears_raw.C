#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>
#include <TList.h>
#include <iostream>
#include "TPaveStats.h"

void drawing_overlapped_distributions(TH1F *h1,TH1F * h2,TH1F * h3, TH1F * h4,TH1F * h5, TH1F *h6, TString chamber_name, TString trimming);
 TH1F * trimmed_mean(TH1F *h);
 TH1F * trimmed_mean(TH1F *h){
    TH1F *h_trim = (TH1F*) h->Clone();
    TH1F * h_trim_new = (TH1F*) h->Clone();

    float integral = 0, trimmean= 0.85;
    // trim the histogram now
               h_trim_new->Reset();
               h_trim_new->ResetStats();
               // Counting the overflow entry also, for trimming purposes
               float normal = h_trim->Integral() + h_trim->GetBinContent(h_trim->GetNbinsX()+1);
               int last_bin = 0;
               for(int it = 1; it<=  h_trim->GetNbinsX() ;it++) {
                 if(integral < trimmean * normal){
                   integral+=h_trim->GetBinContent(it);
                   //std::cout<<" old bin entry "<<it<<" entry "<<h_trim->GetBinContent(it)<<std::endl;
                   last_bin =it;
                 }
               }
             double new_integral =0;
             int entries_last_bin;
             for(int it=1; it<last_bin; it++){
               new_integral += h_trim->GetBinContent(it);
             }

             entries_last_bin = (int) (trimmean*normal - new_integral);
             for(int it=1; it<last_bin ; it++) {
               h_trim_new->SetBinContent(it,h_trim->GetBinContent(it));
               //std::cout<<" new bin entry "<<it<<" entry "<<h_trim_new->GetBinContent(it)<<std::endl;
               h_trim_new->SetBinError(it,h_trim->GetBinError(it));
             }
             h_trim_new->SetBinContent(last_bin, entries_last_bin);
         if(entries_last_bin!=0) {
             h_trim_new->SetBinError(last_bin, h_trim->GetBinError(last_bin) * (entries_last_bin / h_trim->GetBinContent(last_bin)));
            }
            for(int it=last_bin+1; it<=h_trim->GetNbinsX() ; it++) {
              h_trim_new->SetBinContent(it,0);
              h_trim_new->SetBinError(it,0);
            }
            float final_integral = new_integral+entries_last_bin;
            float check_integral = normal * trimmean;
           // std::cout<<" final integral "<<final_integral<<" normal "<<check_integral<<std::endl;
           std::pair<float, float> trimmed_mean_value;
           trimmed_mean_value.first = h_trim_new->GetMean();
           trimmed_mean_value.second = h_trim_new->GetMeanError();
           return h_trim_new;

         }

  void drawing_overlapped_distributions(TH1F *h1,TH1F * h2,TH1F * h3, TH1F * h4,TH1F * h5, TH1F *h6, TString chamber_name, TString trimming){
    TCanvas *c = new TCanvas("c", "RHSumQ Distributions", 800, 600);
    c->SetLeftMargin(0.13);
    c->SetTitle("Raw charge : "+chamber_name);
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
     c->Modified(); // Update the canvas to process any pending drawing operations.
     c->Update();   // Force an immediate update of the canvas.
 
     h4->Draw("HIST same");
     c->Modified(); // Update the canvas to process any pending drawing operations.
     c->Update();   // Force an immediate update of the canvas.
     
     h5->Draw("HIST same");
     c->Modified(); // Update the canvas to process any pending drawing operations.
     c->Update();   // Force an immediate update of the canvas.
     
     h6->Draw("HIST same");


    h3->GetXaxis()->SetTitle("Charge (ADC)");
    h3->GetYaxis()->SetTitle("Normalized nb. of entries");

    h2->GetXaxis()->SetTitle("Charge (ADC)");
    h2->GetYaxis()->SetTitle("Normalized nb. of entries");
    h1->GetXaxis()->SetTitle("Charge (ADC)");
    h1->GetYaxis()->SetTitle("Normalized nb. of entries");
    h4->GetXaxis()->SetTitle("Charge (ADC)");
    h4->GetYaxis()->SetTitle("Normalized nb. of entries");
    h5->GetXaxis()->SetTitle("Charge (ADC)");
    h5->GetYaxis()->SetTitle("Normalized nb. of entries");
    h6->GetXaxis()->SetTitle("Charge (ADC)");
    h6->GetYaxis()->SetTitle("Normalized nb. of entries");

  
 
//    h2->DrawNormalized("same");
    c->Modified();
    c->Update();

        // Update the canvas to ensure all drawing actions are processed
   // Manually creating stats boxes using TPaveText
    TPaveText *stats1 = new TPaveText(0.65, 0.75, 0.88, 0.88, "NDC");
    stats1->SetFillColor(0);
    stats1->SetTextAlign(12);
    stats1->AddText("runNb <277792 (2016)");
    stats1->AddText(Form("Entries: %.0f", h1->GetEntries()));
    stats1->AddText(Form("Mean: %.2f", h1->GetMean()));
    stats1->AddText(Form("Std Dev: %.2f", h1->GetStdDev()));
    stats1->AddText(Form("Integral: %.2f", h1->Integral()));
    stats1->SetTextColor(kRed);
    stats1->Draw("same");

    TPaveText *stats2 = new TPaveText(0.65, 0.6, 0.88, 0.73, "NDC");
    stats2->SetFillColor(0);
    stats2->SetTextAlign(12);
    stats2->AddText("277792 <= runNb <281613 (2016)");
    stats2->AddText(Form("Entries: %.0f", h2->GetEntries()));
    stats2->AddText(Form("Mean: %.2f", h2->GetMean()));
    stats2->AddText(Form("Std Dev: %.2f", h2->GetStdDev()));
    stats2->AddText(Form("Integral: %.2f", h2->Integral()));
    stats2->SetTextColor(kGreen);
    stats2->Draw("same");

    TPaveText *stats3 = new TPaveText(0.65, 0.46, 0.88, 0.59, "NDC");
    stats3->SetFillColor(0);
    stats3->SetTextAlign(12);
    stats3->AddText("runNb >=281613 (2016)");
    stats3->AddText(Form("Entries: %.0f", h3->GetEntries()));
    stats3->AddText(Form("Mean: %.2f", h3->GetMean()));
    stats3->AddText(Form("Std Dev: %.2f", h3->GetStdDev()));
    stats3->AddText(Form("Integral: %.2f", h3->Integral()));
    stats3->SetTextColor(kBlue);
    stats3->Draw("same");

    TPaveText *stats4 = new TPaveText(0.54, 0.71, 0.64, 0.88, "NDC");
    stats4->SetFillColor(0);
    stats4->SetTextAlign(12);
    stats4->AddText("2017");
    stats4->AddText(Form("Entries: %.0f", h4->GetEntries()));
    stats4->AddText(Form("Mean: %.2f", h4->GetMean()));
    stats4->AddText(Form("Std Dev: %.2f", h4->GetStdDev()));
    stats4->AddText(Form("Integral: %.2f", h4->Integral()));
    stats4->SetTextColor(kMagenta);
    stats4->Draw("same");

    TPaveText *stats5 = new TPaveText(0.54, 0.53, 0.64, 0.70, "NDC");
    stats5->SetFillColor(0);
    stats5->SetTextAlign(12);
    stats5->AddText("runNb <324077 (2018)");
    stats5->AddText(Form("Entries: %.0f", h5->GetEntries()));
    stats5->AddText(Form("Mean: %.2f", h5->GetMean()));
    stats5->AddText(Form("Std Dev: %.2f", h5->GetStdDev()));
    stats5->AddText(Form("Integral: %.2f", h5->Integral()));
    stats5->SetTextColor(kBlack);
    stats5->Draw("same");

    TPaveText *stats6 = new TPaveText(0.54, 0.38, 0.64, 0.52, "NDC");
    stats6->SetFillColor(0);
    stats6->SetTextAlign(12);
    stats6->AddText("runNb >=324077 (2018)");
    stats6->AddText(Form("Entries: %.0f", h6->GetEntries()));
    stats6->AddText(Form("Mean: %.2f", h6->GetMean()));
    stats6->AddText(Form("Std Dev: %.2f", h6->GetStdDev()));
    stats6->AddText(Form("Integral: %.2f", h6->Integral()));
    stats6->SetTextColor(kGreen);
    stats6->Draw("same");


    TLegend * legend = new TLegend(0.65,0.3, 0.88, 0.45);
    legend->AddEntry(h1, "RunNb <277792 : 2016");
    legend->AddEntry(h2, "277792 <= RunNb <281613 : 2016");
    legend->AddEntry(h3, "RunNb >=281613 : 2016");
    legend->AddEntry(h4, "2017");
    legend->AddEntry(h5, "RunNb <324077 (2018)");
    legend->AddEntry(h6, "RunNb >=324077 (2018)");
  

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
    pt->AddText("RAW Charge : " + chamber_name);
    pt->SetFillColor(0);
    pt->SetTextColor(1);
    pt->SetTextAlign(22); // Center alignment
    pt->SetTextFont(42); // Helvetica, normal
    pt->Draw("same");
//    st1->Draw("same");
//    st2->Draw("same");

//    c->BuildLegend();
    c->SaveAs("AllYear_RawCharges/"+chamber_name+"_RHSumQ_Distributions_"+trimming+".pdf");
  }


void analyze_root_files(TString directory) {
    // List of chamber names to process
 std::vector<TString> chamberNames = {
        "ME12HV1", "ME12HV2", "ME12HV3", "ME13HV1", "ME13HV2", "ME13HV3",
        "ME21HV1", "ME21HV2", "ME21HV3", "ME22HV1", "ME22HV2", "ME22HV3",
        "ME22HV4", "ME22HV5", "ME31HV1", "ME31HV2", "ME31HV3", "ME32HV1",
        "ME32HV2", "ME32HV3", "ME32HV4", "ME32HV5", "ME41HV1", "ME41HV2",
        "ME41HV3", "ME42HV1", "ME42HV2", "ME42HV3", "ME42HV4", "ME42HV5"
    }; 

	TFile *output_file = new TFile("HV_raw_nonME11.root", "RECREATE");
    for(int i=0; i<chamberNames.size(); i++){
      TString chamber_name = chamberNames[i];
      TString input_file_2016 = directory+"2016_updated/csc_output_2016_"+chamber_name+"_tree_updated.root";
      TString input_file_2017 = directory+"2017_updated/csc_output_2017_"+chamber_name+"_tree_updated.root";
      TString input_file_2018 = directory+"2018_updated/csc_output_2018_"+chamber_name+"_tree_updated.root";
    // Define histograms for the different run number ranges
    TH1F* h1 = new TH1F("h1", "RHSumQ for RunNb < 277792", 300, 0, 3000);
    TH1F* h2 = new TH1F("h2", "RHSumQ for RunNb >= 277792 and RunNb <281613 ", 300, 0, 3000);
    TH1F* h3 = new TH1F("h3", "RHSumQ for RunNb >= 281613 ", 300, 0, 3000);
    TH1F* h4 = new TH1F("h4", "RHSumQ for 2017  ", 300, 0, 3000);
    TH1F* h5 = new TH1F("h5", "RHSumQ for RunNb < 324077 (2018)  ", 300, 0, 3000);
    TH1F* h6 = new TH1F("h6", "RHSumQ for RunNb >= 324077 (2018)  ", 300, 0, 3000);
   
    h1->SetLineColor(kRed);
    h2->SetLineColor(kOrange+2);
    h3->SetLineColor(kBlue);
    h4->SetLineColor(kMagenta);
    h5->SetLineColor(kBlack);
    h6->SetLineColor(kGreen+2);


            TFile *root_file_2016 = TFile::Open(input_file_2016);

            if (!root_file_2016 || root_file_2016->IsZombie()) {
                std::cout << "Failed to open " <<  ". It may be corrupted.\n";
                continue;
            }

            TTree* tree_2016;
            root_file_2016->GetObject("tree", tree_2016); // Adjust tree name

            if (!tree_2016) {
                std::cout << "No TTree found in " <<  ". Skipping this file.\n";
                root_file_2016->Close();
                continue;
            }
            // Set branch addresses
            Double_t rhsumQ_2016, rhsumQ_RAW_2016;
            Double_t rhsumQ_equalised_HV_data_2016;
//            Double_t rhsumQ;
            Long64_t runNb_2016;
            tree_2016->SetBranchAddress("_rhsumQ", &rhsumQ_2016);
            tree_2016->SetBranchAddress("_rhsumQ_RAW", &rhsumQ_RAW_2016);
            tree_2016->SetBranchAddress("_rhsumQ_equalised_HV_data", &rhsumQ_equalised_HV_data_2016);
            tree_2016->SetBranchAddress("_runNb", &runNb_2016);

            Long64_t nentries_2016 = tree_2016->GetEntries();

            // Loop over all entries
            for (Long64_t i = 0; i < nentries_2016; ++i) {
                tree_2016->GetEntry(i);

                if (runNb_2016 < 277792) {
                    //h1->Fill(rhsumQ_equalised_HV_data_2016);
                    h1->Fill(rhsumQ_RAW_2016);
                } 
                else if (277792 <= runNb_2016 && runNb_2016 < 281613) {
                    h2->Fill(rhsumQ_RAW_2016);
                    //h2->Fill(rhsumQ_equalised_HV_data_2016);
                } 

             else if(runNb_2016 >=281613){
                    //h3->Fill(rhsumQ_equalised_HV_data_2016);
                    h3->Fill(rhsumQ_RAW_2016);
                }
            }

            root_file_2016->Close();

            TFile *root_file_2017 = TFile::Open(input_file_2017);
            if (!root_file_2017 || root_file_2017->IsZombie()) {
                std::cout << "Failed to open " <<  ". It may be corrupted.\n";
                continue;
            }
            TTree* tree_2017;
            root_file_2017->GetObject("tree", tree_2017); // Adjust tree name
            if (!tree_2017) {
                std::cout << "No TTree found in " <<  ". Skipping this file.\n";
                root_file_2017->Close();
                continue;
            }
            // Set branch addresses
            Double_t rhsumQ_2017, rhsumQ_RAW_2017;
            Double_t rhsumQ_equalised_HV_data_2017;
//            Double_t rhsumQ;
            Long64_t runNb_2017;
            tree_2017->SetBranchAddress("_rhsumQ", &rhsumQ_2017);
            tree_2017->SetBranchAddress("_rhsumQ_RAW", &rhsumQ_RAW_2017);
            tree_2017->SetBranchAddress("_rhsumQ_equalised_HV_data", &rhsumQ_equalised_HV_data_2017);
            tree_2017->SetBranchAddress("_runNb", &runNb_2017);

            Long64_t nentries_2017 = tree_2017->GetEntries();

            // Loop over all entries
            for (Long64_t i = 0; i < nentries_2017; ++i) {
                tree_2017->GetEntry(i);
                    //h4->Fill(rhsumQ_equalised_HV_data_2017);
                    h4->Fill(rhsumQ_RAW_2017);
            }
            root_file_2017->Close();
 
            TFile *root_file_2018 = TFile::Open(input_file_2018);
            if (!root_file_2018 || root_file_2018->IsZombie()) {
                std::cout << "Failed to open " <<  ". It may be corrupted.\n";
                continue;
            }
            TTree* tree_2018;
            root_file_2018->GetObject("tree", tree_2018); // Adjust tree name
            if (!tree_2018) {
                std::cout << "No TTree found in " <<  ". Skipping this file.\n";
                root_file_2018->Close();
                continue;
            }
            // Set branch addresses
            Double_t rhsumQ_2018, rhsumQ_RAW_2018;
            Double_t rhsumQ_equalised_HV_data_2018;
//            Double_t rhsumQ;
            Long64_t runNb_2018;
            tree_2018->SetBranchAddress("_rhsumQ", &rhsumQ_2018);
            tree_2018->SetBranchAddress("_rhsumQ_RAW", &rhsumQ_RAW_2018);
            tree_2018->SetBranchAddress("_rhsumQ_equalised_HV_data", &rhsumQ_equalised_HV_data_2018);
            tree_2018->SetBranchAddress("_runNb", &runNb_2018);

            Long64_t nentries_2018 = tree_2018->GetEntries();

            // Loop over all entries
            for (Long64_t i = 0; i < nentries_2018; ++i) {
                tree_2018->GetEntry(i);
                if (runNb_2018 < 324077) {
                //h5->Fill(rhsumQ_equalised_HV_data_2018);
                h5->Fill(rhsumQ_RAW_2018);
               }
               else if (runNb_2018 >= 324077) {
                //h5->Fill(rhsumQ_equalised_HV_data_2018);
                h6->Fill(rhsumQ_RAW_2018);
               }

            }
            root_file_2018->Close();
           
            h1->Scale(1./h1->Integral());
            h2->Scale(1./h2->Integral());
            h3->Scale(1./h3->Integral());
            h4->Scale(1./h4->Integral());
            h5->Scale(1./h5->Integral());
            h6->Scale(1./h6->Integral());


            output_file->cd();
	    h1->SetName("h1_"+chamber_name);
	    h2->SetName("h2_"+chamber_name);
	    h3->SetName("h3_"+chamber_name);
	    h4->SetName("h4_"+chamber_name);
	    h5->SetName("h5_"+chamber_name);
	    h6->SetName("h6_"+chamber_name);
	    h1->Write();
	    h2->Write();
	    h3->Write();
	    h4->Write();
	    h5->Write();
	    h6->Write();

            drawing_overlapped_distributions(h1, h2, h3, h4, h5, h6, chamber_name, "before_trimming");
    // Draw all histograms on the same canvas
            TH1F *trimmed_h1 = trimmed_mean(h1);
            TH1F *trimmed_h2 = trimmed_mean(h2);
            TH1F *trimmed_h3 = trimmed_mean(h3);
            TH1F *trimmed_h4 = trimmed_mean(h4);
            TH1F *trimmed_h5 = trimmed_mean(h5);
            TH1F *trimmed_h6 = trimmed_mean(h6);
            drawing_overlapped_distributions(trimmed_h1, trimmed_h2, trimmed_h3, trimmed_h4, trimmed_h5, trimmed_h6, chamber_name,  "after_trimming");

//            output_file->cd();
//    	trimmed_h1->SetName("trimmed_h1_"+chamber_name);
//    	trimmed_h2->SetName("trimmed_h2_"+chamber_name);
//    	trimmed_h3->SetName("trimmed_h3_"+chamber_name);
//    	trimmed_h4->SetName("trimmed_h4_"+chamber_name);
//    	trimmed_h5->SetName("trimmed_h5_"+chamber_name);
//    	trimmed_h6->SetName("trimmed_h6_"+chamber_name);
//    	trimmed_h1->Write();
//    	trimmed_h2->Write();
//    	trimmed_h3->Write();
//    	trimmed_h4->Write();
//    	trimmed_h5->Write();
//    	trimmed_h6->Write();
    delete h1;
    delete h2;
    delete h3;
    delete h4;
    delete h5;
    delete h6;
    
    }
        output_file->Close();

}

void reading_charge_allyears_raw() {
    //TString directory_path = "/afs/cern.ch/user/n/nrawal/eos/CSCAgeing/Run2_Reprocessed/";
    TString directory_path = "/afs/cern.ch/user/n/nrawal/eos/CSCAgeing/Run2_Ntuples/";
//   directory_path = argv[1]; // Optionally use directory path from command line
    analyze_root_files(directory_path);
}

