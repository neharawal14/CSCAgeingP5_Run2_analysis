#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TMultiGraph.h>
#include <TAxis.h>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <limits>  // Needed for setting min/max values

void scanning_fill() {
    // Open ROOT file and get tree
    TFile *file = TFile::Open("/eos/home-n/nrawal/CSCAgeing/Run2_combine_new_selections/2016_updated/csc_output_2016_ME11a_tree_HVupdated.root", "READ");
    TTree *tree = (TTree*)file->Get("tree");

    // Define variables
    double _instlumi;
    ULong64_t _runNb;
    UInt_t _timesecond;

    // Set tree branches
    tree->SetBranchAddress("_instlumi", &_instlumi);
    tree->SetBranchAddress("_runNb", &_runNb);
    tree->SetBranchAddress("_timesecond", &_timesecond);

    // Store data in a map (key: run number, value: vector of time-instlumi pairs)
    std::map<ULong64_t, std::vector<std::pair<UInt_t, double>>> instlumi_map;

    // Variables for global min/max time range
    UInt_t global_time_min = std::numeric_limits<UInt_t>::max();
    UInt_t global_time_max = std::numeric_limits<UInt_t>::min();
    double global_instlumi_min = std::numeric_limits<double>::max();
    double global_instlumi_max = std::numeric_limits<double>::min();

    int total_entries = 0, accepted_entries = 0, rejected_entries = 0 ;
    // Loop over tree and store data in corresponding run vector
    for (int i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        if (_runNb >= 275809 && _runNb <= 275848) {
            instlumi_map[_runNb].push_back({_timesecond, _instlumi});
            
            // Update global min/max for time and instlumi
            global_time_min = std::min(global_time_min, _timesecond);
            global_time_max = std::max(global_time_max, _timesecond);
            global_instlumi_min = std::min(global_instlumi_min, _instlumi);
            global_instlumi_max = std::max(global_instlumi_max, _instlumi);
        }
    }


    // Create TMultiGraph to combine all runs
    TMultiGraph *mg_diff = new TMultiGraph();
    TMultiGraph *mg_acc_diff = new TMultiGraph();
    TMultiGraph *mg_rej_diff = new TMultiGraph();

    // Color array for different runs
    int colors[] = {kViolet, kBlue, kRed, kGreen, kMagenta, kCyan, kOrange, kBlack, kPink, kYellow, kViolet};
    int color_idx = 0;

    // Iterate over each run in the map
    for (auto &entry : instlumi_map) {
        color_idx++;
        ULong64_t run = entry.first;
        std::vector<std::pair<UInt_t, double>> &instlumi_data = entry.second;
        // Sort by time
        std::sort(instlumi_data.begin(), instlumi_data.end());
        
        TGraph *graph_diff = new TGraph(instlumi_data.size());
       std::vector<std::pair<UInt_t, double>> diff_filtered_data;
       std::vector<std::pair<UInt_t, double>> diff_rejected_data;

       std::vector<std::pair<UInt_t, double>> filtered_data;
       std::vector<std::pair<UInt_t, double>> rejected_data;
       filtered_data.push_back(instlumi_data[0]);  // Keep first point
        if (!instlumi_data.empty()) {
            for (size_t i = 1; i < instlumi_data.size(); i++) {
                double instlumi_prev;
                 //instlumi_prev = filtered_data.back().second;
                 instlumi_prev = instlumi_data[i-1].second;
                 double instlumi_curr = instlumi_data[i].second;

                 double relative_diff = (instlumi_curr - instlumi_prev)/instlumi_prev;
                 graph_diff->SetPoint(i, instlumi_data[i].first, relative_diff);
             }
						std::pair<UInt_t, double> diff;
            
	 
             for (size_t i = 1; i < instlumi_data.size(); i++) {
                 double instlumi_prev;
                  instlumi_prev = filtered_data.back().second;
                 double instlumi_curr = instlumi_data[i].second;
                 double relative_diff = ((instlumi_curr - instlumi_prev)/instlumi_prev);
 
								 diff = std::make_pair(instlumi_data[i].first, relative_diff);
                 if (abs(relative_diff) <= 0.01) {
                     filtered_data.push_back(instlumi_data[i]);  // Accept point
                     diff_filtered_data.push_back(diff);  // Accept point
                 }
                 else{
                    diff_rejected_data.push_back(diff);  // Accept point
                    rejected_data.push_back(instlumi_data[i]);  // Accept point
                 }
             }

        mg_diff->Add(graph_diff);  // Add graph to TMultiGraph
        TGraph *graph_acc_diff = new TGraph(diff_filtered_data.size());
        TGraph *graph_rej_diff = new TGraph(diff_rejected_data.size());
       graph_acc_diff->SetMarkerColor(colors[color_idx % 10]);  // Assign different color to each run
       graph_acc_diff->SetLineColor(colors[color_idx % 10]);
         // Graph styling
        graph_diff->SetMarkerColor(colors[color_idx % 10]);  // Assign different color to each run
        graph_diff->SetLineColor(colors[color_idx % 10]);
   
        if(!diff_filtered_data.empty()){
        for (size_t i = 1; i < diff_filtered_data.size(); i++) {
					graph_acc_diff->SetPoint(i, diff_filtered_data[i].first, diff_filtered_data[i].second);
        }
        mg_acc_diff->Add(graph_acc_diff);  // Add graph to TMultiGraph
        }
        if(!diff_rejected_data.empty()){
        for (size_t i = 1; i < diff_rejected_data.size(); i++) {
					graph_rej_diff->SetPoint(i, diff_rejected_data[i].first, diff_rejected_data[i].second);

        }
        mg_rej_diff->Add(graph_rej_diff);  // Add graph to TMultiGraph
        }



        std::cout<<" checked non empty iniitial point"<<std::endl;
      } // instlumi empty check end

        std::cout<<" checked final "<<std::endl;
    } // end of map
    // Draw the TMultiGraph
    // Create TCanvas
    std::cout<<" drawing canvas "<<std::endl;
    TCanvas *c = new TCanvas("c", "Instlumi Profile", 1200, 700);
    c->cd();

    mg_diff->SetTitle("Fill 5094 (relative diff consecutive points);Time (Day-Hour:Minute);relative diff");
    mg_diff->Draw("AP");
    std::cout<<" drawing canvas "<<std::endl;
    // Set correct range AFTER all graphs are added
    mg_diff->GetXaxis()->SetTimeFormat("%d-%H:%M");
    mg_diff->GetXaxis()->SetTimeDisplay(1);
    mg_diff->GetXaxis()->SetTimeOffset(0, "gmt");
    mg_diff->GetXaxis()->SetRangeUser(global_time_min - 3600, global_time_max + 3600);
    //mg_diff->GetYaxis()->SetRangeUser(-.05, .05);

    // Save output
    c->SaveAs("relative_diff_instlumi_fill_5094_all.pdf");

		TCanvas *c1 = new TCanvas();
c1->cd();
    mg_acc_diff->SetTitle("Fill 5094 (relative diff accepted points);Time (Day-Hour:Minute);relative diff");
    mg_acc_diff->Draw("AP");

    // Set correct range AFTER all graphs are added
    mg_acc_diff->GetXaxis()->SetTimeFormat("%d-%H:%M");
    mg_acc_diff->GetXaxis()->SetTimeDisplay(1);
    mg_acc_diff->GetXaxis()->SetTimeOffset(0, "gmt");
    mg_acc_diff->GetXaxis()->SetRangeUser(global_time_min - 3600, global_time_max + 3600);
    mg_acc_diff->GetYaxis()->SetRangeUser(-.05, .05);

    // Save output
    c1->SaveAs("relative_diff_instlumi_fill_5094_accepted.pdf");

		TCanvas *c2 = new TCanvas();
c2->cd();
    mg_rej_diff->SetTitle("Fill 5094 (relative diff rejected points);Time (Day-Hour:Minute); rel diff ");
    mg_rej_diff->Draw("AP");

    // Set correct range AFTER all graphs are added
    mg_rej_diff->GetXaxis()->SetTimeFormat("%d-%H:%M");
    mg_rej_diff->GetXaxis()->SetTimeDisplay(1);
    mg_rej_diff->GetXaxis()->SetTimeOffset(0, "gmt");
    mg_rej_diff->GetXaxis()->SetRangeUser(global_time_min - 3600, global_time_max + 3600);
//    mg_rej_diff->GetYaxis()->SetRangeUser(-50, 50);

    // Save output
    c2->SaveAs("relative_diff_instlumi_fill_5094_rejected.pdf");

}

