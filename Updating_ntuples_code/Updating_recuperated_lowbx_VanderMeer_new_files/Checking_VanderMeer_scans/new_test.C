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
//std::map<ULong64_t, std::vector<std::pair<ULong64_t, double>>> accepted_instlumi_data;
std::map<Long64_t, std::vector<std::pair<Long64_t, double>>> accepted_instlumi_data;
    auto FixTimeAxis = [](TCanvas* c, TAxis* ax) {
//      c->SetBottomMargin(0.18);         // more room for rotated labels
    
      ax->SetTimeDisplay(1);
      ax->SetTimeOffset(0, "gmt");
      ax->SetTimeFormat("%d-%H:%M");    // keep your format
    
//      ax->SetNdivisions(506, kTRUE);    // fewer labels (major divisions)
ax->SetNdivisions(406, kTRUE);   // fewer labels

      ax->SetLabelSize(0.035);          // slightly smaller labels
//      ax->SetLabelAngle(45);            // rotate labels to avoid overlap
//      ax->SetLabelOffset(0.01);
    };
 
//bool is_good_entry(ULong64_t runNb, ULong64_t eventNb, double instlumi) {
bool is_good_entry(Long64_t runNb, Long64_t eventNb, double instlumi) {
 
   //std::cout << "Entered function for run: " << runNb << " time: " << timesecond << " instlumi: " << instlumi << std::endl;
 
     auto it = accepted_instlumi_data.find(runNb);
     if (it != accepted_instlumi_data.end()) {
         const auto &data = it->second;
 
         for (const auto &entry : data) {
             UInt_t accepted_event = entry.first;
             double accepted_instlumi = entry.second;
 
             // Check if instlumi matches and time is within 10 minutes (600 seconds)
             if (accepted_instlumi == instlumi && accepted_event == eventNb) {
                 //std::cout << "Accepting entry with time: " << accepted_event << " instlumi: " << accepted_instlumi << std::endl;
                 return true;
             }
         }
     }
     //std::cout << "Rejecting entry for run: " << runNb << " time: " << eventNb << " instlumi: " << instlumi << std::endl;
 
     return false;
 } 


void new_test() {
    // Open ROOT file and get tree
    //TFile *file = TFile::Open("/eos/home-n/nrawal/CSCAgeing/Run2_Ntuples/2016_updated/csc_output_2016_ME12HV1_tree_updated.root", "READ");
    TFile *file = TFile::Open("/eos/home-n/nrawal/CSCAgeing/Run2_Ntuples/2018_updated/csc_output_2018_ME12HV1_tree_updated.root", "READ");
    TTree *tree = (TTree*)file->Get("tree");

    // Define variables
    double _instlumi;
    double _instlumi_dcsjson;
    Long64_t _runNb;
    Long64_t _eventNb;
    Long64_t _timesecond;
    double _HV, _HV_nominal;

    // Set tree branches
    tree->SetBranchAddress("_instlumi_dcsjson", &_instlumi_dcsjson);
    tree->SetBranchAddress("_runNb", &_runNb);
    tree->SetBranchAddress("_eventNb", &_eventNb);
    tree->SetBranchAddress("_timesecond", &_timesecond);
    tree->SetBranchAddress("_HV", &_HV);
    tree->SetBranchAddress("_HV_nominal", &_HV_nominal);

    // Store data in a map (key: run number, value: vector of time-instlumi pairs)
    //std::map<ULong64_t, std::vector<std::pair<UInt_t, double>>> instlumi_map;
    // Variables for global min/max time range
    Long64_t global_time_min = std::numeric_limits<Long64_t>::max();
    Long64_t global_time_max = std::numeric_limits<Long64_t>::min();
    double global_instlumi_min = std::numeric_limits<double>::max();
    double global_instlumi_max = std::numeric_limits<double>::min();

    int total_entries = 0, accepted_entries = 0, rejected_entries = 0 ;
    //std::map<ULong64_t, std::map<ULong64_t, double>> instlumi_map; // Ordered automatically
    std::map<Long64_t, std::map<Long64_t, double>> instlumi_map; // Ordered automatically
 
     for (int i = 0; i < tree->GetEntries(); i++) {
         tree->GetEntry(i);
        // if(_runNb>=283392 && _runNb <=283416){ 
        if(_runNb>=324973 && _runNb <=324980){
        // std::cout<<" entry "<<i<<" run "<<_runNb<<" instlumi "<<_instlumi_dcsjson<<" event "<<_eventNb<<std::endl;
	// if(!(_runNb>=305597 && _runNb <=305638))continue; 
         //if(_runNb>=281648 && _runNb<= 281693){
        // if(_runNb>=283392 && _runNb<= 283404){
         instlumi_map[_runNb][_eventNb] = _instlumi_dcsjson;  // Automatically sorted
         }
     }
 
     int new_total_e = 0, new_initial_e = 0;
     for (auto &[run, instlumi_data] : instlumi_map) {
 
       // Convert map to sorted vector
       std::vector<std::pair<Long64_t, double>> sorted_data(instlumi_data.begin(), instlumi_data.end());
       // Sort by time
       // Filter out spikes
       new_initial_e += sorted_data.size();
       //std::cout<<" sorted data size "<<sorted_data.size()<<std::endl;
       std::vector<std::pair<Long64_t, double>> filtered_data;
       std::vector<std::pair<Long64_t, double>> rejected_data;
       if (!sorted_data.empty()) {
             filtered_data.push_back(sorted_data[0]);  // Keep first point
             new_total_e++;
             for (size_t i = 1; i < sorted_data.size(); i++) {
                 double instlumi_prev = sorted_data[i-1].second;
                 double instlumi_curr = sorted_data[i].second;
                  
                 if (instlumi_curr == instlumi_prev && 
                     std::find(rejected_data.begin(), rejected_data.end(), sorted_data[i-1]) != rejected_data.end()){
                     std::cout<<" rejecting point since same instlumi as previous "<<instlumi_curr<<" i "<<i<<" run "<<run<<" time "<<sorted_data[i].first<<std::endl;
                     rejected_data.push_back(sorted_data[i]);  // Accept point
                     continue;
                 } 
                 if (std::abs((instlumi_curr - instlumi_prev) / instlumi_prev) <= 0.01) {
                     std::cout<<" accepting point "<<instlumi_curr<<" i "<<i<<" run "<<run<<" time "<<sorted_data[i].first<<std::endl;

                     filtered_data.push_back(sorted_data[i]);  // Accept point
                     new_total_e++;
                 }

                 else{
                     std::cout<<" rejecting point since far from old point "<<instlumi_curr<<" i "<<i<<" instlumi prev "<<instlumi_prev<<" run "<<run<<" time "<<sorted_data[i].first<<std::endl;
                     rejected_data.push_back(sorted_data[i]);  // Accept point
                     continue;
                 }
             }
         }
         //std::cout<<" filtered data size "<<filtered_data.size()<<std::endl;
         // Store filtered data globally
         accepted_instlumi_data[run] = std::move(filtered_data);
    }
 
     int total_e = 0;
     for (const auto &entry : accepted_instlumi_data) {
           total_e += entry.second.size(); // entry.second is the vector of accepted (timestamp, instlumi) pairs
     }
     //std::cout << "Size of accepted data: " << accepted_instlumi_data.size() << ", Total accepted: "<<total_e<<std::endl;
     //std::cout << "Size of accepted data: " <<accepted_instlumi_data.size()<<" initial "<<new_initial_e<<" Total accepted: "<<new_total_e<<std::endl;


    // Loop over tree and store data in corresponding run vector
    for (int i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
         //if(_runNb>=283392 && _runNb <=283416){ 
         if(_runNb>=324973 && _runNb <=324980){
         //std::cout<<" runNb "<<_runNb<<std::endl;
         
	//if(_runNb>=305597 && _runNb <=305638){ 
         //if(_runNb>=281648 && _runNb<= 281693){
        //if(_runNb==283408){
            // Update global min/max for time and instlumi
            global_time_min = std::min(global_time_min, _timesecond);
            global_time_max = std::max(global_time_max, _timesecond);
            global_instlumi_min = std::min(global_instlumi_min, _instlumi_dcsjson);
            global_instlumi_max = std::max(global_instlumi_max, _instlumi_dcsjson);
    }
  }

    // Create TCanvas
    // Create TMultiGraph to combine all runs
    TMultiGraph *mg_initial = new TMultiGraph();

    TMultiGraph *mg = new TMultiGraph();
    TMultiGraph *mg_rej = new TMultiGraph();
 
      
	        TGraph *graph = new TGraph();
        TGraph *graph_rej = new TGraph();
	TGraph *graph_initial = new TGraph();

  int nEntries = tree->GetEntries();
  int total_inst_entries = 0;
  int accepted_inst_entries = 0; 
  int rejected_inst_entries = 0; 

  for (Long64_t i = 0; i < nEntries; i++) {
         tree->GetEntry(i);
 
         // removing the recuperated and wrong gas composition period 
         //if (abs(_HV - _HV_nominal) > 10) continue;
         // This was for testing purposes :
         //if(_rhid!=2121921) continue;
         // Determine the bin for this entry
         //if(!(_runNb>=283392 && _runNb <=283416)) continue;
         if(!(_runNb>=324973 && _runNb <=324980)) continue; 
	 
 	//if (!(_runNb >= 275809 && _runNb <= 275848)) continue;
         // if(!(_runNb>=305597 && _runNb <=305638))continue;
         //if(!(_runNb>=281648 && _runNb<= 281693)) continue; 
         graph_initial->SetPoint(i, _timesecond, _instlumi_dcsjson);
           total_inst_entries++;
         if (!is_good_entry(_runNb, _eventNb, _instlumi_dcsjson)) {
             rejected_inst_entries++;
         		 graph_rej->SetPoint(i, _timesecond, _instlumi_dcsjson);
             //std::cout << "Rejected Entry: Run " << _runNb << ", Time " << _timesecond << ", Instlumi " << _instlumi << std::endl;
         } else {
         		graph->SetPoint(i, _timesecond, _instlumi_dcsjson);
             //std::cout << "Using Entry: Run " << _runNb << ", Time " << _timesecond << ", Instlumi " << _instlumi << std::endl;
             accepted_inst_entries++;
         }
      }// end of tree entires

    // Color array for different runs
    int colors[] = {kViolet, kBlue, kRed, kGreen, kMagenta, kCyan, kOrange, kBlack, kPink, kYellow, kViolet};
    int color_idx = 0;


       graph_initial->SetMarkerColor(kBlue);  // Assign different color to each run
       graph_initial->SetLineColor(kBlue);

        mg_initial->Add(graph_initial);  // Add graph to TMultiGraph
        // Skip if no valid data after filtering


       // Create TGraph for this run

        mg->Add(graph);  // Add graph to TMultiGraph
        graph_rej->SetMarkerColor(kRed);  // Assign different color to each run
        graph_rej->SetLineColor(kRed);
        mg_rej->Add(graph_rej);  // Add graph to TMultiGraph

    double percentage = (rejected_entries *1./total_entries) *100;
    std::cout<<" total :"<<total_inst_entries<<" accepted "<<accepted_inst_entries<<" rejected "<<rejected_inst_entries<<" per(%) "<<percentage<<std::endl;
    TCanvas *c = new TCanvas("c", "Instlumi Profile", 900, 700);
    c->cd();
    c->SetLeftMargin(0.14);


    // Draw the TMultiGraph
    mg->SetTitle("Fill 7321 (Filtered);Time (Day-Hour:Minute);Instlumi (*10^{30}) cm^{-2}s^{-1}");
    //mg->SetTitle("Fill 6325 (Filtered);Time (Day-Hour:Minute);Instlumi (*10^{30}) cm^{-2}s^{-1}");
    //mg->SetTitle("Fill 5423 (Filtered);Time (Day-Hour:Minute);Instlumi (*10^{30}) cm^{-2}s^{-1}");
    //mg->SetTitle("Fill 5339 (Filtered);Time (Day-Hour:Minute);Instlumi (*10^{30}) cm^{-2}s^{-1}");
    mg->Draw("AP");
    FixTimeAxis(c, mg->GetXaxis());
   // Set correct range AFTER all graphs are added
    mg->GetXaxis()->SetTimeFormat("%d-%H:%M");
    mg->GetXaxis()->SetTimeDisplay(1);
    mg->GetXaxis()->SetTimeOffset(0, "gmt");
    mg->GetXaxis()->SetRangeUser(global_time_min - 3600, global_time_max + 3600);
    mg->GetYaxis()->SetRangeUser(global_instlumi_min - 1000, global_instlumi_max + 1000);  // Small padding

    // Save output
    c->SaveAs("plots_1per_consecutive_cuts/filtered_instlumi_fill_7321_correct_per_1per.pdf");
    //c->SaveAs("plots_1per_consecutive_cuts/filtered_instlumi_fill_5339_correct_per_1per.pdf");
    // c->SaveAs("plots_1per_consecutive_cuts/filtered_instlumi_fill_6325_correct_per_1per.pdf");
    //c->SaveAs("plots_1per_consecutive_cuts/filtered_instlumi_fill_5423_correct_per_1per_complete.pdf");

    TCanvas *c_new = new TCanvas("c_new","c_new",900,700);
    c_new->cd();
    c_new->SetLeftMargin(0.14);
    //mg_rej->SetTitle("Fill 5339 (rejected);Time (Day-Hour:Minute);Instlumi (*10^{30}) cm^{-2}s^{-1}");
    mg_rej->SetTitle("Fill 7321 (rejected);Time (Day-Hour:Minute);Instlumi (*10^{30}) cm^{-2}s^{-1}");
    //mg_rej->SetTitle("Fill 6325 (rejected);Time (Day-Hour:Minute);Instlumi (*10^{30}) cm^{-2}s^{-1}");
    //mg_rej->SetTitle("Fill 5423 (rejected);Time (Day-Hour:Minute);Instlumi (*10^{30}) cm^{-2}s^{-1}");
    mg_rej->Draw("AP");
    FixTimeAxis(c_new, mg_rej->GetXaxis());

    // Set correct range AFTER all graphs are added
    mg_rej->GetXaxis()->SetTimeFormat("%d-%H:%M");
    mg_rej->GetXaxis()->SetTimeDisplay(1);
    mg_rej->GetXaxis()->SetTimeOffset(0, "gmt");
    mg_rej->GetXaxis()->SetRangeUser(global_time_min - 3600, global_time_max + 3600);
    mg_rej->GetYaxis()->SetRangeUser(global_instlumi_min - 1000, global_instlumi_max + 1000);  // Small padding
   // c_new->SaveAs("plots_1per_consecutive_cuts/rejected_instlumi_fill_5339_correct_per_1per.pdf");
    c_new->SaveAs("plots_1per_consecutive_cuts/rejected_instlumi_fill_7321_correct_per_1per.pdf");
   //c_new->SaveAs("plots_1per_consecutive_cuts/rejected_instlumi_fill_6325_correct_per_1per.pdf");
   // c_new->SaveAs("plots_1per_consecutive_cuts/rejected_instlumi_fill_5423_correct_per_1per_complete.pdf");

    TCanvas *c1_new = new TCanvas("c1_new","c1_new", 900,700);
    c1_new->cd();
    c1_new->SetLeftMargin(0.14);
    //mg_initial->SetTitle("Fill 5339 (all);Time (Day-Hour:Minute);Instlumi (*10^{30}) cm^{-2}s^{-1}");
    mg_initial->SetTitle("Fill 7321 (all);Time (Day-Hour:Minute);Instlumi (*10^{30}) cm^{-2}s^{-1}");
   // mg_initial->SetTitle("Fill 6325 (all);Time (Day-Hour:Minute);Instlumi (*10^{30}) cm^{-2}s^{-1}");
   // mg_initial->SetTitle("Fill 5423 (all);Time (Day-Hour:Minute);Instlumi (*10^{30}) cm^{-2}s^{-1}");
    mg_initial->Draw("AP");
    FixTimeAxis(c1_new, mg_initial->GetXaxis());

    // Set correct range AFTER all graphs are added
    mg_initial->GetXaxis()->SetTimeFormat("%d-%H:%M");
    mg_initial->GetXaxis()->SetTimeDisplay(1);
    mg_initial->GetXaxis()->SetTimeOffset(0, "gmt");
    mg_initial->GetXaxis()->SetRangeUser(global_time_min - 3600, global_time_max + 3600);
    mg_initial->GetYaxis()->SetRangeUser(global_instlumi_min - 1000, global_instlumi_max + 1000);  // Small padding
    //c1_new->SaveAs("plots_1per_consecutive_cuts/Complete_fill_5339_correct_per_1per.pdf");
    c1_new->SaveAs("plots_1per_consecutive_cuts/Complete_fill_7321_correct_per_1per.pdf");
    //c1_new->SaveAs("plots_1per_consecutive_cuts/Complete_fill_6325_correct_per_1per.pdf");
    //c1_new->SaveAs("plots_1per_consecutive_cuts/Complete_fill_5423_correct_per_1per_complete.pdf");

}

