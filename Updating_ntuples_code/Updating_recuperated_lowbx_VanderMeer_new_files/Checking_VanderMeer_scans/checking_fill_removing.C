#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

void checking_fill_removing() {
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
    UInt_t global_time_min = std::numeric_limits<UInt_t>::max();
    UInt_t global_time_max = std::numeric_limits<UInt_t>::min();
    double global_instlumi_min = std::numeric_limits<double>::max();
    double global_instlumi_max = std::numeric_limits<double>::min();
    // Loop over tree and store data in corresponding run vector
    for (int i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        if (_runNb >= 275809 && _runNb <= 275848) {
            instlumi_map[_runNb].push_back({_timesecond, _instlumi});
           // Update global time and instlumi ranges
            global_time_min = std::min(global_time_min, _timesecond);
            global_time_max = std::max(global_time_max, _timesecond);
            global_instlumi_min = std::min(global_instlumi_min, _instlumi);
            global_instlumi_max = std::max(global_instlumi_max, _instlumi);

        }
    }

    std::cout<<" global time min "<<global_time_min<<" max "<<global_time_max<<std::endl;
    // Create TCanvas
    TCanvas *c = new TCanvas("c", "Instlumi Profile", 1200, 700);
    c->cd();
     TGraph *firstGraph = nullptr;  // To store the first graph for setting axis properties

    // Color array for different runs
    int colors[] = {kBlue, kRed, kGreen, kMagenta, kCyan, kOrange, kBlack, kPink, kYellow, kViolet};
    int color_idx = 0;

    // Iterate over each run in the map
    int total_entries=0, accepted_entries = 0 , rejected_entries=0;
    for (auto &entry : instlumi_map) {
        ULong64_t run = entry.first;
        std::vector<std::pair<UInt_t, double>> &instlumi_data = entry.second;

        // Sort by time
        std::sort(instlumi_data.begin(), instlumi_data.end());

        total_entries = instlumi_data.size()+total_entries;
        // Filter out sharp instlumi jumps
        std::vector<std::pair<UInt_t, double>> filtered_data;
        if (!instlumi_data.empty()) {
            filtered_data.push_back(instlumi_data[0]);  // Keep first point

            for (size_t i = 1; i < instlumi_data.size(); i++) {
                double instlumi_prev = filtered_data.back().second;
                double instlumi_curr = instlumi_data[i].second;

                if (std::abs(instlumi_curr - instlumi_prev) <= 100) {
                    filtered_data.push_back(instlumi_data[i]);  // Accept point
                    accepted_entries = accepted_entries+1;
                }
                else{
                  rejected_entries++;
                }
            }
        }

        // Skip if no valid data after filtering
        if (filtered_data.empty()) continue;

        // Create graph for this run
        TGraph *graph = new TGraph(filtered_data.size());
        for (size_t i = 0; i < filtered_data.size(); i++) {
            graph->SetPoint(i, filtered_data[i].first, filtered_data[i].second);
        }

        // Graph styling
        graph->SetMarkerColor(colors[color_idx % 10]);  // Assign different color to each run
        //graph->SetLineColor(colors[color_idx % 10]);
        if (firstGraph == nullptr) {
            firstGraph = graph;  // Store the first graph for axis properties
            firstGraph->SetTitle("Instlumi Fill 5094 (Filtered)");
            firstGraph->GetXaxis()->SetTitle("Time (Day-Hour:Minute)");
            firstGraph->GetXaxis()->SetRangeUser(global_time_min - 3600, global_time_max + 3600);
            firstGraph->SetMinimum(global_instlumi_min - 10);  // Small padding
            firstGraph->SetMaximum(global_instlumi_max + 10);

            firstGraph->GetYaxis()->SetTitle("Instlumi (*10^33) cm^{-2}s^{-1}");
            firstGraph->GetXaxis()->SetTimeFormat("%d-%H:%M");
            firstGraph->GetXaxis()->SetTimeDisplay(1);
            firstGraph->GetXaxis()->SetTimeOffset(0, "gmt");

            firstGraph->Draw("AP");
        }
        else {
            graph->Draw("P SAME");  // Overlay subsequent graphs
        }


        std::cout<<" run "<<run<<" color "<<color_idx<<std::endl;
        color_idx++;
    }
    std::cout<<" total "<<total_entries<<" accepted "<<accepted_entries<<" rejected entries "<<rejected_entries<<std::endl;
    // Set correct range AFTER plotting all graphs

    // Save output
    c->SaveAs("filtered_instlumi_fill_5094_new_run.pdf");
}

