#include "tree_class.h"
#include <iostream>
#include <sstream>
#include "TString.h"
#include <fstream>
#include <functional>
#include <stdio.h>
#include <stdlib.h>
//#include "ChargeORIGandInstL.h"
using namespace std;
class RemovingEntries{
  private : 
  bool isRun2Full(Long64_t _timesecond, Long64_t _runNb){
//    return (! (  (_runNb >=274954 && _runNb <=276543) || 
//	(_runNb >=318828 && _runNb <=319993) ||
//        (_runNb >= 323414 && _runNb <= 325172) ||
//        (_runNb >=300574 && _runNb <=301627) )
//        );
//
    return (! (  
	   (_timesecond >=1502064000 && _timesecond <1503360000) || 
           (_timesecond >=1503014400 && _timesecond <1504310400) ||
	   (_timesecond >=1502064000 && _timesecond <1503360000) || 
	   (_timesecond >=1503014400 && _timesecond <1504310400) ||
	   (_timesecond >=1528848000 && _timesecond <1530144000) ||
           (_timesecond >= 1530748800 && _timesecond <= 1532044800) ||
           (_timesecond >= 1536710400 && _timesecond <= 1538006400) ||
           (_timesecond >= 1539129600 && _timesecond <= 1540425600) 
	 ));

    }

    bool is2016Full(Long64_t _timesecond, Long64_t _runNb) {
        // Recuperated gas inserted - 7June; +15 days = 22 June 
        // Fresh gas inserted 24 June; +15 days = 9 July
        // return (! ( (_timesecond >=1465257600 && _timesecond <1466553600) || 
        //  (_timesecond >=1466726400 && _timesecond <1468022400) ) );
        // At last removed : 7June - 9July : same thing as above
         return (! ( (_timesecond >=1465257600 && _timesecond <1468022400) ) );
    }

    bool is2017Full(Long64_t _timesecond, Long64_t _runNb) {
        //Gas change happened on 7Aug and then at 18Aug again fresh gas, so periods removed: 7Aug - 2Sep: 
        return (!  ( (_timesecond >=1502064000 && _timesecond <1503360000) || 
		  (_timesecond >= 1503014400 && _timesecond <1504310400) ) );
        // At last removed : 7Aug - 2Sep : same thing as above
        return (!  ( (_timesecond >=1502064000 && _timesecond <1504310400) ));
        ///return (!(_integratelumi >= 50 && _integratelumi < 53.7));
        //return (!  (_runNb >=300574 && _runNb <=302019));
    }

    bool is2018Full(Long64_t _timesecond, Long64_t _runNb) {
//            13June <=time <28 June ; 5 July <=time < 20 July
//            12 Sep <=time <27 Sep ; 10 Oct <=time < 25 Oct
// I have not removed 28June to 5 July data, it correspond to small data in 110 fb-1, but it seems to not affect final results, logically it should have been removed. But it is too much to now do that, so it is just kept. No one will know anyway, its a really small fraction of data, and it is not affecting any result.
        return (! ( (_timesecond >=1528848000 && _timesecond <1530144000) ||
            (_timesecond >= 1530748800 && _timesecond < 1532044800)  ||
            (_timesecond >= 1536710400 && _timesecond < 1540425600)  
	     ) ) ;
//        return (! ( (_runNb >=318828 && _runNb <=319993) ||
//            (_runNb >= 323414 && _runNb <= 325172) )
//            );
//        return (! ( (_timesecond >=319337 && _runNb <=319950) ||
//            (_runNb >= 323414 && _runNb <= 325172) )
//            ); 

    }
   std::map<TString, std::function<bool(Long64_t, Long64_t)>> conditionMap = {
    {"2016", [this](Long64_t _timesecond, Long64_t _runNb) { return this->is2016Full(_timesecond, _runNb); }},
    {"2017", [this](Long64_t _timesecond, Long64_t _runNb) { return this->is2017Full(_timesecond, _runNb); }},
    {"2018", [this](Long64_t _timesecond, Long64_t _runNb) { return this->is2018Full(_timesecond, _runNb); }},
    {"run2", [this](Long64_t _timesecond, Long64_t _runNb) { return this->isRun2Full(_timesecond, _runNb); }}
   };

  public : 
  
  TString year_name; 
  TreeStructure *tree ;
  void initialiseTreeStructure(TString file_path, TString output_file_path, TString);
  void FillingTree();

  void read_run_with_low_bx();
  void removing_VanderMeer_scans();
  bool is_good_entry(Long64_t runNb, Long64_t _eventNb, double instlumi) ;

  TString area_name;
  std::vector<std::pair<int, int>> run_ranges;
  std::vector<std::pair<int, int>> time_ranges;
  bool debug = false;
 
  double _rhsumQ_equalised_HV_data, _pressure, _rhsumQ_RAW;
  double _rhsumQ;
  Long64_t _rhid;
  double _integratelumi, _instlumi;
  double _intlumi_delivered_dcsjson, _intlumi_delivered_goldenjson;
  double _intlumi_recorded_dcsjson, _intlumi_recorded_goldenjson;
  double _HV;
  double _HV_nominal;
  Long64_t _runNb;
  Long64_t _eventNb;
  Long64_t _timesecond;
 
  std::map<Long64_t, std::vector<std::pair<Long64_t, double>>> accepted_instlumi_data;
  std::map<Long64_t, std::vector<std::pair<Long64_t, double>>> rejected_instlumi_data;
};

// To check  if the entry satisfies VanderMeer Sca
bool RemovingEntries::is_good_entry(Long64_t runNb, Long64_t eventNb, double instlumi) {
 
      auto it = accepted_instlumi_data.find(runNb);
      if (it != accepted_instlumi_data.end()) {
          const auto &data = it->second;
 
          for (const auto &entry : data) {
              UInt_t accepted_event = entry.first;
              double accepted_instlumi = entry.second;
 
              if (accepted_instlumi == instlumi && accepted_event == eventNb) {
                  //std::cout << "Accepting entry with time: " << accepted_event << " instlumi: " << accepted_instlumi << std::endl;
                  return true;
              }
          }
      }
      //std::cout << "Rejecting entry for run: " << runNb << " time: " << eventNb << " instlumi: " << instlumi << std::endl;
      return false;
} 
// end of checking if the entry satisfies VanderMeer Scan
// To check what are VanderMeer Entries
void RemovingEntries::removing_VanderMeer_scans() {
      std::map<Long64_t, std::map<Long64_t, double>> instlumi_map; // Ordered automatically
      int count_entry=0;
      //std::cout<<" inside function to check for VanderMeer Scan"<<std::endl;
      for (int i = 0; i < tree->tree->GetEntries(); i++) {
          tree->tree->GetEntry(i);
          if (conditionMap.find(year_name) != conditionMap.end())
         // if (!conditionMap[year_name](tree->old_intlumi_delivered_goldenjson,tree->old_runNb)) continue;
          if (!conditionMap[year_name](tree->old_timesecond,tree->old_runNb)) continue;
          // Skip invalid entries based on conditions
          //if (abs(tree->old_HV - tree->old_HV_nominal) > 10) continue;
           instlumi_map[tree->old_runNb][tree->old_eventNb] = tree->old_instlumi_dcsjson;  // Automatically sorted
           count_entry++;
      }

      //std::cout<<" read tree to check for VanderMeer Scan"<<std::endl;
 
      // Step 2: Check threshold condition for acceptance or rejection
      int accepted_instlumi_complete = 0;
      int rejected_instlumi_complete = 0;
 
      int instlumi_entries = 0 ;
      // Process and filter each run
      // auto [run, entry] :
 
      for (const auto& [run, entry] : instlumi_map) {
          std::vector<std::pair<Long64_t, double>> instlumi_data(entry.begin(), entry.end());
          instlumi_entries += instlumi_data.size();
 
          std::vector<std::pair<Long64_t, double>> filtered_data;
          std::vector<std::pair<Long64_t, double>> rejection_data;
          if (!instlumi_data.empty()) {
              filtered_data.push_back(instlumi_data[0]);  // Keep first point
              accepted_instlumi_complete++;
 
              for (size_t i = 1; i < instlumi_data.size(); i++) {
                  double instlumi_prev = instlumi_data[i - 1].second;
                  double instlumi_curr = instlumi_data[i].second;
                  if (instlumi_curr == instlumi_prev &&
                     std::find(rejection_data.begin(), rejection_data.end(), instlumi_data[i-1]) != rejection_data.end()){
                     rejection_data.push_back(instlumi_data[i]);  // reject point
                     continue;
                   }
 
                  if (std::abs((instlumi_curr - instlumi_prev) / instlumi_prev) <= 0.01) {
                      filtered_data.push_back(instlumi_data[i]);  // Accept point
                      accepted_instlumi_complete++;
                  }
                  else{
                      rejection_data.push_back(instlumi_data[i]);  // Accept point
                      rejected_instlumi_complete++;
                  }
              }
          }// end of instlumi data check

          // Store filtered data globally
           accepted_instlumi_data[run] = filtered_data;
           rejected_instlumi_data[run] = rejection_data;
           
           std::cout<<" run "<<run<<" size of accepted instlumi "<<filtered_data.size()<<" size of rejected "<<rejection_data.size()<<std::endl;
       }
 }
 // end of remvoing entries
 // To check if the run has low bunch crossing
 void RemovingEntries :: read_run_with_low_bx(){
     // Open CSV file
    //std::ifstream file("/eos/home-n/nrawal/CSCAgeing/VanderMeer_removal/bunchcrossing_luminosity/filtered_runs_"+year_name+".csv");
    std::ifstream file("./BunchCrossing_per_fill/filtered_runs_"+year_name+".csv");
      if (!file.is_open()) {
          std::cerr << "Error: Cannot open CSV file!" << std::endl;
          return;
      }
      // Store run ranges in a vector of pairs
      std::string line;
      std::getline(file, line);
      // Read the CSV file line by line
      while (std::getline(file, line)) {
          std::stringstream ss(line);
          std::string run_start_str,run_end_str;
          int run_start, run_end;
          // Read the first three fields (ignoring whitespace issues)
 
          std::getline(ss, run_start_str, ',');
          std::getline(ss, run_end_str, ',');
 
          try {
                  int run_start = std::stoi(run_start_str);
                  int run_end = std::stoi(run_end_str);
                  run_ranges.push_back(std::make_pair(run_start, run_end));
              } catch (const std::invalid_argument &e) {
                  std::cerr << "Error: Unable to convert values in line: " << line << std::endl;
              }
           } // end of while loop
         file.close();
         std::cout << "Parsed Run Ranges:\n";
            for (const auto &range : run_ranges) {
                std::cout << "Start: " << range.first << ", End: " << range.second << std::endl;
                }
  } // end of reading the csv file for low bx
int main(int argc,char *argv[]) {
    for (int i = 0; i < argc; i++) {
      std::cout<<argv[i]<<"\t";
     }
   // Convert the second argument to TString
   TString year = TString::Format("%s", argv[1]);
   TString chamber_name = TString::Format("%s", argv[2]);
   std::cout<<" the year "<<year<<std::endl;
   std::cout<<" chamber "<<chamber_name<<std::endl;
 
  TString input_path = "/afs/cern.ch/user/n/nrawal/eos/CSCAgeing/Run2_Ntuples/"+year+"_updated/";
  //TString input_path = "/afs/cern.ch/user/n/nrawal/eos/CSCAgeing/Run2_Ntuples_complete_relaxed/"+year+"_updated/";
 // TString input_path = "/afs/cern.ch/user/n/nrawal/eos/CSCAgeing/Run2_Ntuples_with_relaxed_hit_per_layer/"+year+"_updated/";
  TString filename = "csc_output_"+year+"_"+chamber_name+"_tree_updated.root";
  TString file_path = input_path +filename;
  
  //TString output_path = "/afs/cern.ch/user/n/nrawal/eos/CSCAgeing/Run2_Ntuples/"+year+"_updated_recuperated/";
  TString output_path = "/afs/cern.ch/user/n/nrawal/eos/CSCAgeing/Run2_Ntuples/"+year+"_updated_new/";
  //TString output_path = "/afs/cern.ch/user/n/nrawal/eos/CSCAgeing/Run2_Ntuples_complete_relaxed/"+year+"_updated_new/";
 // TString output_path = "/afs/cern.ch/user/n/nrawal/eos/CSCAgeing/Run2_Ntuples_with_relaxed_hit_per_layer/"+year+"_updated_new/";
  TString output_filename = "csc_output_"+year+"_"+chamber_name+"_tree_updated.root";
  TString output_file_path = output_path+output_filename;

  RemovingEntries *obj = new RemovingEntries();
  obj->initialiseTreeStructure(file_path, output_file_path, year);
  obj->FillingTree();

  return 0;
}

void RemovingEntries :: initialiseTreeStructure(TString file_path, TString output_file_path, TString year){
  tree = new TreeStructure();
  // initilaising the other class
  tree->initialise(file_path, output_file_path);
  tree->Setup_new_tree();

  year_name = year;
  this->read_run_with_low_bx();
  this->removing_VanderMeer_scans();
}


void RemovingEntries :: FillingTree(){
  
  std::cout<<"tree entries "<<tree->tree->GetEntries()<<std::endl;
  int entries = tree->tree->GetEntries();
  //for(int i = 0 ;i<entries ; i++){
  std::cout<<"tree entries "<<tree->tree->GetEntries()<<std::endl;
  int j =0 ;
  bool debug =false;

  int total_inst_entries = 0, skipped_entries =0 ;
  int accepted_inst_entries = 0, rejected_inst_entries = 0;

  int entries_all = 0; 
  int entries_acc_first = 0,entries_acc_second = 0, entries_acc_third = 0; 
  for(int i = 0 ;i<entries ; i++){
  //for(int i = 0 ;i<2000 ; i++){
    tree->tree->GetEntry(i);
    // std::cout<<" run Nb "<<tree->old_runNb<<std::endl;
    if(debug){  j = j+1; if(j>=2000) break; }
    if(debug) std::cout<<" tree HV "<<tree->old_HV<<std::endl;
    if(debug) std::cout<<" tree HV nominal "<<tree->old_HV_nominal<<std::endl;
    if(debug) std::cout<<" tree charge "<<tree->old_rhsumQ_RAW<<std::endl;
   
    entries_all++; 
    // Check if the period belongs to recuperated period , then remove the event 
    if (conditionMap.find(year_name) != conditionMap.end())
    if (!conditionMap[year_name](tree->old_timesecond,tree->old_runNb)) continue;

//    if (conditionMap.find(year_name) != conditionMap.end())
//    //if (!conditionMap[year_name](tree->old_integratelumi, tree->old_runNb)) continue;
//    if (!conditionMap[year_name](tree->old_integratelumi, tree->old_runNb)) continue;

    entries_acc_first++; 
    if(debug) std::cout<<" here after the first condition is passed"<<std::endl;
    // To check if low bunch crossing
    bool skip = false;
    for (const auto &range : run_ranges) {
         if (tree->old_runNb >= range.first && tree->old_runNb <= range.second) {
             skip = true;
             skipped_entries++;
             break;  // No need to check further if already matched
         }
      }
     if (skip) continue;  // Skip this entry if in unwanted range

    entries_acc_second++; 
    if(debug) std::cout<<" here after the first condition is passed"<<std::endl;
     // To check if low VanderMeer Removal
     total_inst_entries++;
     //std::cout<<" run Nb "<<tree->old_runNb<<" event "<<tree->old_eventNb<<" instlumi "<<tree->old_instlumi_dcsjson<<std::endl;
     if (!is_good_entry(tree->old_runNb, tree->old_eventNb, tree->old_instlumi_dcsjson)) {
      rejected_inst_entries++;
      if(debug) std::cout << "Rejected Entry: Run " << tree->old_runNb << ", Time " <<tree->old_timesecond << ", Instlumi " << tree->old_instlumi_dcsjson << std::endl;
      continue;
    } else {
      if(debug) std::cout << "Using Entry: Run " << tree->old_runNb << ", Time " << tree->old_timesecond << ", Instlumi " << tree->old_instlumi_dcsjson << std::endl;
      accepted_inst_entries++;
    }
    entries_acc_third++; 
    if(debug) std::cout<<" here after the first condition is passed"<<std::endl;
//    tree->new_passZmumusel = tree->old_passZmumusel;
//    tree->new_passisomuondzdxy = tree->old_passisomuondzdxy;
    tree->new_eventNb = tree->old_eventNb; 
    tree->new_runNb  = tree->old_runNb;
    tree->new_lumiBlock = tree->old_lumiBlock;
    tree->new_rhid    = tree->old_rhid;
    tree->new_stationring   = tree->old_stationring;
//    tree->new_current = 0;
    tree->new_pressure = tree->old_pressure;
//    tree->new_temperature = tree->old_temperature;
    tree->new_timesecond = tree->old_timesecond;
    tree->new_n_PV = tree->old_n_PV;
    tree->new_bunchcrossing = tree->old_bunchcrossing;
    tree->new_etamuon = tree->old_etamuon;
    tree->new_phimuon = tree->old_phimuon;
    tree->new_ptmuon = tree->old_ptmuon;
//    tree->new_z_pt = tree->old_z_pt;
//    tree->new_z_eta = tree->old_z_eta;
//    tree->new_z_phi = tree->old_z_phi;
//    tree->new_z_mass = tree->old_z_mass;
    tree->new_HV = tree->old_HV; 
    tree->new_HV_nominal = tree->old_HV_nominal; 
    tree->new_rhsumQ = tree->old_rhsumQ;
    tree->new_rhsumQ_RAW =tree->old_rhsumQ_RAW;
    tree->new_rhsumQ_equalised_HV_data = tree->old_rhsumQ_equalised_HV_data;
    tree->new_integratelumi = tree->old_integratelumi;
    tree->new_instlumi = tree->old_instlumi;
    tree->new_nearestStrip= tree->old_nearestStrip;
    tree->new_instlumi_goldenjson= tree->old_instlumi_goldenjson;
    tree->new_instlumi_dcsjson= tree->old_instlumi_dcsjson;
    tree->new_intlumi_delivered_goldenjson= tree->old_intlumi_delivered_goldenjson;
    tree->new_intlumi_recorded_goldenjson= tree->old_intlumi_recorded_goldenjson;
    tree->new_intlumi_delivered_dcsjson= tree->old_intlumi_delivered_dcsjson;
    tree->new_intlumi_recorded_dcsjson= tree->old_intlumi_recorded_dcsjson;
///    tree->new_iso_PF_first = tree->old_iso_PF_first;
///    tree->new_iso_PF_second = tree->old_iso_PF_second;
///    tree->new_isolation1 = tree->old_isolation1;
///    tree->new_isolation2 = tree->old_isolation2;
  
    tree->tree_new->Fill();
  } // end of going through tree

  std::cout<<" total entries "<<total_inst_entries<<" accepted "<<accepted_inst_entries<<" rejected "<<rejected_inst_entries<<std::endl;
  std::cout<<" total  "<<entries_all<<" accepted after first "<<entries_acc_first<<" after second "<<entries_acc_second<<" after third "<<entries_acc_third<<std::endl;
                          
tree->WriteNew();
}
