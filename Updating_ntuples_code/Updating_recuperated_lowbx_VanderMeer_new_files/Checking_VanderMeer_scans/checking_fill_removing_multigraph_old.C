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

void checking_fill_removing_multigraph() {
    // Open ROOT file and get tree
    TFile *file = TFile::Open("/eos/home-n/nrawal/CSCAgeing/Run2_combine_new_selections/2018_updated/csc_output_2018_ME12HV1_tree_HVupdated.root", "READ");
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
        //if(_runNb >=317634 && _runNb<=317641) {
        if(_runNb ==317435) {
            instlumi_map[_runNb].push_back({_timesecond, _instlumi});
            
            // Update global min/max for time and instlumi
            global_time_min = std::min(global_time_min, _timesecond);
            global_time_max = std::max(global_time_max, _timesecond);
            global_instlumi_min = std::min(global_instlumi_min, _instlumi);
            global_instlumi_max = std::max(global_instlumi_max, _instlumi);
        }
    }

    // Create TCanvas
    TCanvas *c = new TCanvas("c", "Instlumi Profile", 1200, 700);
    c->cd();

    // Create TMultiGraph to combine all runs
    TMultiGraph *mg_initial = new TMultiGraph();
    TMultiGraph *mg = new TMultiGraph();
    TMultiGraph *mg_rej = new TMultiGraph();

    // Color array for different runs
    int colors[] = {kViolet, kBlue, kRed, kGreen, kMagenta, kCyan, kOrange, kBlack, kPink, kYellow, kViolet};
    int color_idx = 0;

    // Iterate over each run in the map
    for (auto &entry : instlumi_map) {
        color_idx++;
        int total_entries_run = 0, accepted_entries_run = 0, rejected_entries_run = 0 ;
        ULong64_t run = entry.first;
        std::vector<std::pair<UInt_t, double>> &instlumi_data = entry.second;

        // Sort by time
        std::sort(instlumi_data.begin(), instlumi_data.end());
        total_entries +=instlumi_data.size();
        total_entries_run =instlumi_data.size();

        TGraph *graph_initial = new TGraph(instlumi_data.size());
        for (size_t i = 0; i < instlumi_data.size(); i++) {
            graph_initial->SetPoint(i, instlumi_data[i].first, instlumi_data[i].second);
        }
        if(instlumi_data.empty()) continue;
        graph_initial->SetMarkerColor(colors[color_idx % 10]);  // Assign different color to each run
        graph_initial->SetLineColor(colors[color_idx % 10]);

        mg_initial->Add(graph_initial);  // Add graph to TMultiGraph

        // Filter out sharp instlumi jumps (> 100 difference)
        std::vector<std::pair<UInt_t, double>> filtered_data;
        std::vector<std::pair<UInt_t, double>> rejected_data;
        if (!instlumi_data.empty()) {
            filtered_data.push_back(instlumi_data[0]);  // Keep first point

            for (size_t i = 1; i < instlumi_data.size(); i++) {
                double instlumi_prev;
                 instlumi_prev = filtered_data.back().second;
                //instlumi_prev = instlumi_data[i-1].second;
                double instlumi_curr = instlumi_data[i].second;

                if (std::abs( (instlumi_curr - instlumi_prev)/instlumi_prev) <= 0.05) {
                //if (std::abs(instlumi_curr - instlumi_prev) <= 50) {
                    filtered_data.push_back(instlumi_data[i]);  // Accept point
                    accepted_entries++;
                    accepted_entries_run++;
                }
                else{
                  std::cout<<" rejecting this point "<<instlumi_data[i].first<<" value "<<instlumi_data[i].second<<" entry "<<i<<std::endl;
                   rejected_data.push_back(instlumi_data[i]);  // Accept point
                   rejected_entries++;
                   rejected_entries_run++;
                }
            }
        }

        // Skip if no valid data after filtering
        if (filtered_data.empty()) continue;

        int size = filtered_data.size()-1;
        std::pair<UInt_t , double>  start_run = filtered_data[0];
        std::pair<UInt_t , double>  end_run = filtered_data[size];

        double percentage_run = (rejected_entries_run * 1./total_entries_run) *100;
//        std::cout<<" run "<<run<<" up instlumi "<<start_run.second<<" low instlumi "<<end_run.second<<std::endl;
        std::cout<<" run "<<run<<" total "<<total_entries_run<<" accepted "<<accepted_entries_run<<" rejected "<<rejected_entries_run<<" per(%) "<<percentage_run<<std::endl;
        // Create TGraph for this run
        TGraph *graph = new TGraph(filtered_data.size());
        TGraph *graph_rej = new TGraph(rejected_data.size());
        for (size_t i = 0; i < filtered_data.size(); i++) {
            graph->SetPoint(i, filtered_data[i].first, filtered_data[i].second);
        }
        for (size_t i = 0; i < rejected_data.size(); i++) {
            graph_rej->SetPoint(i, rejected_data[i].first, rejected_data[i].second);
        }

        // Graph styling
        graph->SetMarkerColor(colors[color_idx % 10]);  // Assign different color to each run
        graph->SetLineColor(colors[color_idx % 10]);

        mg->Add(graph);  // Add graph to TMultiGraph
        if(rejected_data.empty()) continue;

        graph_rej->SetMarkerColor(colors[color_idx % 10]);  // Assign different color to each run
        graph_rej->SetLineColor(colors[color_idx % 10]);
        mg_rej->Add(graph_rej);  // Add graph to TMultiGraph

    }
    double percentage = (rejected_entries *1./total_entries) *100;
    std::cout<<" total :"<<total_entries<<" accepted "<<accepted_entries<<" rejected "<<rejected_entries<<" per(%) "<<percentage<<std::endl;
    // Draw the TMultiGraph
    mg->SetTitle("Fill 6759 (Filtered);Time (Day-Hour:Minute);Instlumi (*10^33) cm^{-2}s^{-1}");
    mg->Draw("AP");

    // Set correct range AFTER all graphs are added
    mg->GetXaxis()->SetTimeFormat("%d-%H:%M");
    mg->GetXaxis()->SetTimeDisplay(1);
    mg->GetXaxis()->SetTimeOffset(0, "gmt");
    mg->GetXaxis()->SetRangeUser(global_time_min - 3600, global_time_max + 3600);
    mg->GetYaxis()->SetRangeUser(global_instlumi_min - 1000, global_instlumi_max + 1000);  // Small padding

    // Save output
    c->SaveAs("filtered_instlumi_fill_6759_correct_per.pdf");

    TCanvas *c_new = new TCanvas();
    c_new->cd();
    mg_rej->SetTitle("Fill 6759 (rejected);Time (Day-Hour:Minute);Instlumi (*10^33) cm^{-2}s^{-1}");
    mg_rej->Draw("AP");

    // Set correct range AFTER all graphs are added
    mg_rej->GetXaxis()->SetTimeFormat("%d-%H:%M");
    mg_rej->GetXaxis()->SetTimeDisplay(1);
    mg_rej->GetXaxis()->SetTimeOffset(0, "gmt");
    mg_rej->GetXaxis()->SetRangeUser(global_time_min - 3600, global_time_max + 3600);
    mg_rej->GetYaxis()->SetRangeUser(global_instlumi_min - 1000, global_instlumi_max + 1000);  // Small padding
    c_new->SaveAs("rejected_instlumi_fill_6759_correct_per.pdf");

    TCanvas *c1_new = new TCanvas();
    c1_new->cd();
    mg_initial->SetTitle("Fill 6759 (all);Time (Day-Hour:Minute);Instlumi (*10^33) cm^{-2}s^{-1}");
    mg_initial->Draw("AP");

    // Set correct range AFTER all graphs are added
    mg_initial->GetXaxis()->SetTimeFormat("%d-%H:%M");
    mg_initial->GetXaxis()->SetTimeDisplay(1);
    mg_initial->GetXaxis()->SetTimeOffset(0, "gmt");
    mg_initial->GetXaxis()->SetRangeUser(global_time_min - 3600, global_time_max + 3600);
    mg_initial->GetYaxis()->SetRangeUser(global_instlumi_min - 1000, global_instlumi_max + 1000);  // Small padding
    c1_new->SaveAs("Complete+fill_6759_correct_per.pdf");

}

