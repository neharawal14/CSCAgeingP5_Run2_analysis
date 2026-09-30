#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <iostream>
#include <vector>
#include <algorithm>

void reading_fill_removing() {
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

    // Store values in a vector
    std::vector<std::pair<UInt_t, double>> instlumi_data;

    // Loop over entries and select required runs
    for (int i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        if (_runNb >= 275809 && _runNb <= 275848) {
            instlumi_data.push_back({_timesecond, _instlumi});
        }
    }

    // Sort data by time (_timesecond)
    std::sort(instlumi_data.begin(), instlumi_data.end());

    // Filter entries: remove sharp jumps in instlumi (> 100 difference)
    std::vector<std::pair<UInt_t, double>> filtered_data;
    if (!instlumi_data.empty()) {
        filtered_data.push_back(instlumi_data[0]);  // Keep the first point

        for (size_t i = 1; i < instlumi_data.size(); i++) {
            double instlumi_prev = filtered_data.back().second;
            double instlumi_curr = instlumi_data[i].second;

            if (std::abs(instlumi_curr - instlumi_prev) <= 100) {
                filtered_data.push_back(instlumi_data[i]);  // Accept point
            }
        }
    }

    // Check if data is available
    if (filtered_data.empty()) {
        std::cerr << "No valid points after filtering!" << std::endl;
        return;
    }

    // Find time range for axis limits
    UInt_t time_old = filtered_data.front().first;
    UInt_t time_end = filtered_data.back().first;

    // Create TGraph for the filtered data
    TGraph *graph = new TGraph(filtered_data.size());
    for (size_t i = 0; i < filtered_data.size(); i++) {
        graph->SetPoint(i, filtered_data[i].first, filtered_data[i].second);
    }

    // Configure graph
    graph->SetTitle("Instlumi Fill 5094 (Filtered)");
    graph->GetXaxis()->SetTitle("Time (Day-Hour:Minute)");
    graph->GetYaxis()->SetTitle("Instlumi (*10^33) cm^{-2}s^{-1}");
    graph->GetXaxis()->SetTimeFormat("%d-%H:%M");
    graph->GetXaxis()->SetTimeDisplay(1);
    graph->GetXaxis()->SetRangeUser(time_old - 3600, time_end + 3600);
    graph->GetXaxis()->SetTimeOffset(0, "gmt");
    graph->SetMarkerColor(kBlue);
    graph->SetMarkerStyle(20);

    // Draw graph
    TCanvas *c = new TCanvas("c", "Instlumi Profile", 1200, 700);
    graph->Draw("AP");

    // Save output
    c->SaveAs("filtered_instlumi_fill_5094.pdf");
}

