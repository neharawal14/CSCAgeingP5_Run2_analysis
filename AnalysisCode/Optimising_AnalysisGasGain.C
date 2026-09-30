#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TH1D.h>
#include "TStyle.h"
#include <TCanvas.h>
#include <TF1.h>
#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include "TPaveStats.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"

// Declaring all the functions 

class GasDependence {
		public : 
    // variables
    // Map year strings to condition functions
    TString area_name;
    TString year = "2016";
    TString chamber_name = "ME12HV1";
    TString type="all_channels";
    std::vector<std::pair<int, int>> run_ranges;
    std::vector<std::pair<int, int>> time_ranges;
    bool debug = false;

    double _rhsumQ_equalised_HV_data, _pressure, _rhsumQ_RAW;
    double _rhsumQ;
    Long64_t _rhid;
    double _integratelumi, _instlumi;
    double _instlumi_dcsjson;
    double _intlumi_delivered_dcsjson;
    double _intlumi_delivered_goldenjson;
    //ULong64_t _lumiBlock;
    Long64_t _lumiBlock;
    double  _HV_nominal;
    double  _HV;
    Long64_t _runNb; 
    Long64_t _eventNb; 
    Long64_t _timesecond;
    //ULong64_t _runNb; 
    //ULong64_t _eventNb; 
    //ULong64_t _timesecond;

    //ULong64_t _nearestStrip;
    Long64_t _nearestStrip;
    TChain *tree;

    void initialise(TString chamber_name_string , TString year_value ,TString area_name_string, TChain* ); 
    double analysing_dependence(const std::map<std::pair<int, int>, std::tuple<TH1D*, double, int> > histograms, TString var, TFile *, bool);
    std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> reading_tree_making_maps(TString variable, bool, double, double, bool, std::vector<double>) ;
    std::string get_channel_name(int rhid);

    TH1D *  trimmed_mean(TH1D * h);
    double ApplyCorrection(double X ,TString correctiontype, double slope );
    double Draw_cumulative_summary_histogram(const std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> histograms ,std::map<int, TH1D*> cumulativeHistogram, TString type, TString var, double binWidth);


    std::vector<double> reading_charge_ME13HV3(const std::map<std::pair<int, int>, std::tuple<TH1D*, double, int> > histograms, TString var);
    void createCumulativeHistogram(std::map<int, TH1D*>& histMap, 
                               int bin, 
                               const TString var, 
                               const TString condition, 
                               double bin_value, 
                               double binWidth, 
                               const TString chamber_name, 
                               const TString year, 
                               bool ) ;
     void FillCumulativeGraphs(const std::map<std::pair<int, int>, std::tuple<TH1D*, double, int> >& ,
      std::map<int, TGraphAsymmErrors*>& ,
      std::map<std::string, std::map<int, TH1D*>>& histograms,
      const TString& chamber_name,
      const TString& var,
      bool normalisation);

      std::map<int, TGraphAsymmErrors*> CreateSummaryGraphs(
        const std::map<std::pair<int, int>, std::tuple<TH1D*, double, int> > histograms, const TString& var);

      void GetVariableBinning(const TString& var, double& min_var, double& max_var, int& nBins, double& binWidth);
      void finding_endcap_chambers(int rhid, TString& endcap, int& chamber_nb, int& layer_nb);
      std::map<std::string, std::map<int, TH1D*>> CreateCumulativeGraphs(
       const std::map<std::pair<int, int>, std::tuple<TH1D*, double, int> > histograms, 
         const TString& var, bool normalisation);

      std::map<std::string, double> FitCumulativeGraphs(const std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> 
          histograms , 
            std::map<int, TH1D*> cumulativeHistograms, TString chamber_name,   TString var);
      void  FitIndieGraphs(TGraphAsymmErrors* h_summary, TString var);
};

void GasDependence :: initialise(TString chamber_name_string , TString year_value ,TString area_name_string, TChain *chain_here){

  area_name = area_name_string;
  chamber_name = chamber_name_string;
  year = year_value;
  tree  = chain_here;
  // Set tree branches
  tree->SetBranchAddress("_rhsumQ_equalised_HV_data", &_rhsumQ_equalised_HV_data);
  tree->SetBranchAddress("_rhsumQ", &_rhsumQ);
  tree->SetBranchAddress("_rhsumQ_RAW", &_rhsumQ_RAW);
  tree->SetBranchAddress("_pressure", &_pressure);
  tree->SetBranchAddress("_integratelumi", &_integratelumi);
  tree->SetBranchAddress("_intlumi_delivered_dcsjson", &_intlumi_delivered_dcsjson);
  tree->SetBranchAddress("_intlumi_delivered_goldenjson", &_intlumi_delivered_goldenjson);
  tree->SetBranchAddress("_timesecond", &_timesecond);
  //tree->SetBranchAddress("_instlumi", &_instlumi);
  tree->SetBranchAddress("_instlumi_dcsjson", &_instlumi_dcsjson);
  tree->SetBranchAddress("_rhid", &_rhid);
  tree->SetBranchAddress("_HV", &_HV);
  tree->SetBranchAddress("_HV_nominal", &_HV_nominal);
  tree->SetBranchAddress("_runNb", &_runNb);
  tree->SetBranchAddress("_eventNb", &_eventNb);
  tree->SetBranchAddress("_lumiBlock", &_lumiBlock);
}


std::string GasDependence :: get_channel_name(int rhid){
     int rhid_reduced = static_cast<int>(std::floor(rhid / 10)) % 1000;
     if (rhid > 2000000) {
         rhid_reduced += 400;
     }
     std::string endcap = (rhid_reduced <= 400) ? "_Endcap1" : "_Endcap2";
     int chamber_nb;
     if (rhid_reduced <= 400) {
         chamber_nb = static_cast<int>(std::floor(rhid_reduced / 10));
     } else {
         chamber_nb = static_cast<int>(std::floor((rhid_reduced - 400) / 10));
     }
     std::string channel_name;
     if (rhid_reduced != 0 && rhid_reduced != 771) {
         channel_name = "chamber" + std::to_string(chamber_nb) +
                        "_layer" + std::to_string(rhid_reduced % 10) +
                        endcap;
     } else {
         channel_name = "undefined";
     }
     return channel_name;
 }
 
// Binning for the variable of interest
 void GasDependence::GetVariableBinning(
  const TString& var,
  double& min_var,
  double& max_var,
  int& nBins,
  double& binWidth
) {
  if (var == "pressure" || var == "pressure_second") {
      min_var = 940;
      max_var = 990;
      nBins = 50;
  }
  else if (var == "instlumi" || var == "instlumi_second") {
    // bin of instlumi is 500 range
      min_var = 0;
      max_var = 2.5;
      nBins = 50;
  }
  else if (var == "n_PV") {
      min_var = 0;
      max_var = 100;
      nBins = 100;
  }

  else if (var == "intlumi_initial" || var == "intlumi_final") {
      min_var = 0;
      max_var = 160;
      nBins = 160;
  }
  else if (var == "timesecond_initial" || var == "timesecond_final") {
       // bin of time is 1 day (86400 seconds)
      int bin_width = 86400;
      if (year == "2016") {
          min_var = 1462838400;
          max_var = 1477871999;
      }
      else if (year == "2017") {
          min_var = 1497484800;
          max_var = 1510790399;
      }
      else if (year == "2018") {
          min_var = 1527206400;
          max_var = 1540511999;
      }

      nBins = ((max_var - min_var + 1) / bin_width);
  }

  binWidth = (max_var - min_var) / nBins;
}


std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> GasDependence :: reading_tree_making_maps(TString variable, bool correction, double slope_pressure, double slope_instlumi
, bool normalisation , std::vector<double> ME13HV3_charge) {
	
     gStyle->SetTimeOffset(0.);
     // Map to store histograms and mean values
    std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> result;

    // Map to track cumulative sums and counts for calculating mean values
    std::map<std::pair<int, int>, std::pair<double, int>> binStats;

    double min_var = 0, max_var = 0;
    int nBins = 0;
    double binWidth = 0;
    this->GetVariableBinning(variable, min_var, max_var, nBins, binWidth);

    int nEntries = tree->GetEntries();

    int skipped_entries = 0;
    int skipped_entries_time = 0;
    int total_inst_entries = 0;
    int accepted_inst_entries = 0;
    int rejected_inst_entries = 0;

    //std::cout<<" Number of entries : " <<nEntries << std::endl;
    for (ULong64_t i = 0; i < nEntries; i++) {
    //for (ULong64_t i = 0; i < 200000; i++) {
        tree->GetEntry(i);
        // removing the recuperated and wrong gas composition period 
        // Skip invalid entries based on conditions
        if (abs(_HV - _HV_nominal) > 5) continue;
        int Bin;
        if(variable=="pressure")  Bin = static_cast<int>( ((_pressure - min_var) / binWidth)+1);
        else if(variable=="instlumi")  { Bin = static_cast<int>( (( (_instlumi_dcsjson/10000.0) - min_var) / binWidth) +1); //std::cout<<" bin "<<Bin<<std::endl;
         }
        else if(variable=="_n_PV")  Bin = static_cast<int>( ((_n_PV - min_var) / binWidth) +1);
        else if(variable=="intlumi")  Bin = static_cast<int>( ((_intlumi_delivered_dcsjson - min_var) / binWidth) +1);
        else if(variable=="time")  Bin = static_cast<int>( ((_timesecond - min_var) / binWidth) +1);
        if (Bin < 0 || Bin >= nBins) continue; // Skip out-of-range pressures

        // Create a reduced key for this bin
        std::pair<int, int> reducedKey = std::make_pair(_rhid, Bin);
        // Ensure the histogram exists for the reduced key
        if (result.find(reducedKey) == result.end()) {
            // Create a new histogram for this bin ; name of the histogram depends on whether before or after correction
            // This is to potentially not have same name histogram and craeting memory leak
            TH1D *hist = nullptr;
            TH1D *hist_new = nullptr;
            if(correction==false){
            hist = new TH1D(
                Form("h_rhid_%d_%s_%d", _rhid, variable.Data(), Bin),
                Form("Histogram for RHID %d, %s Bin %d", _rhid, variable.Data(), Bin),
                3000, 0, 3000
            );
            hist->AddDirectory(kFALSE);
           if(normalisation==true){
            hist_new = new TH1D(
                Form("h_rhid_%d_%s_%d", _rhid, variable.Data(), Bin),
                Form("Histogram for RHID %d, %s Bin %d", _rhid, variable.Data(), Bin),
                50, 0, 5
            );
            hist_new->AddDirectory(kFALSE);}
            }
            else if(correction==true){
            hist = new TH1D(
                Form("h_rhid_%d_%s_second_%d", _rhid, variable.Data(), Bin),
                Form("Histogram for RHID %d, %s Bin %d : after correction", _rhid, variable.Data(), Bin),
                3000, 0, 3000
            );
            hist->AddDirectory(kFALSE);

           if(normalisation==true){
            hist_new = new TH1D(
                Form("h_rhid_%d_%s_%d", _rhid, variable.Data(), Bin),
                Form("Histogram for RHID %d, %s Bin %d", _rhid, variable.Data(), Bin),
                50, 0, 5
            );
            hist_new->AddDirectory(kFALSE);}
            }
            if(normalisation==true)
            result[reducedKey] = std::make_tuple(hist_new, 0.0, 0); // Initialize the histogram and mean
            else 
            result[reducedKey] = std::make_tuple(hist, 0.0, 0); // Initialize the histogram and mean
        }

        double equalised_charge =0;
        equalised_charge = _rhsumQ_equalised_HV_data * ApplyCorrection(_pressure ,"pressure", slope_pressure) * 
        ApplyCorrection(_instlumi_dcsjson, "instlumi", slope_instlumi);

        // Normalise charges for deriving instlumi dependence with charges in ME13HV3
        if(normalisation==true) {
          if(ME13HV3_charge[Bin]!=0)
           //std::cout<<" old equalised charge :"<<equalised_charge<<" bin :"<<Bin<<" ME13HV3 charge :"<<ME13HV3_charge[Bin]<<std::endl;
          equalised_charge = equalised_charge/ME13HV3_charge[Bin];
        }

        // Fill the histogram with charge data
        std::get<0>(result[reducedKey])->Fill(equalised_charge);

        // Update cumulative sum and count for calculating the mean pressure
        if(variable=="pressure"){        
        binStats[reducedKey].first += _pressure; // Cumulative sum of pressure values
        binStats[reducedKey].second += 1;        // Count of pressure value
        }

        else if(variable=="instlumi"){        
        binStats[reducedKey].first += (_instlumi_dcsjson/10000.0); // Cumulative sum of pressure values binStats[reducedKey].second += 1;        // Count of pressure values
        binStats[reducedKey].second += 1;        // Count of pressure value
// std::cout<<" bin stats : reduced key "<<reducedKey.first<<" instlumi "<<_instlumi_dcsjson<<" thousand "<<ratio<<std::endl;
        }
        else if(variable=="_n_PV"){        
        binStats[reducedKey].first += _n_PV; // Cumulative sum of pressure values binStats[reducedKey].second += 1;        // Count of pressure values
        binStats[reducedKey].second += 1;        // Count of pressure value
        }

        else if(variable=="intlumi"){        
        //binStats[reducedKey].first += _integratelumi; // Cumulative sum of pressure values
        binStats[reducedKey].first += _intlumi_delivered_dcsjson; // Cumulative sum of pressure values
        binStats[reducedKey].second += 1;        // Count of pressure values
        }
        else if(variable=="time"){        
        binStats[reducedKey].first += _timesecond; // Cumulative sum of pressure values
        binStats[reducedKey].second += 1;        // Count of pressure values
        }
        // Ensure the histogram exists for the reduced key

        //std::cout<<" rhid "<<_rhid<<" : run : "<<_runNb<<" charge "<<_rhsumQ_RAW<<std::endl;
    } // end of going through each tree entry
//    std::cout<<" total entries "<<nEntries<<" skipped entries "<<skipped_entries<<std::endl;
    std::cout<<" total entries inst "<<total_inst_entries<<" skipped entries "<<rejected_inst_entries<<" accepted "<<accepted_inst_entries<<std::endl;

    // Calculate the mean pressure for each bin and store it in the result map
    for (auto& [key, stats] : binStats) {
        double sumPressure = stats.first;
        int count = stats.second;
        double meanPressure = (count > 0) ? (sumPressure / count) : 0.0;

         // Update the mean value in the result map
         std::get<1>(result[key]) = meanPressure;
         std::get<2>(result[key]) = count;
         TH1D* h1 = std::get<0>(result[key]);
         int nentries = h1->Integral();
         double mean = h1->GetMean();
         if(debug) std::cout<<" key "<<key.first<<" bin "<<key.second<< " mean "<<meanPressure<<" histogram value "<<nentries<<" count "<<count<<" mean value "<<mean<<std::endl;
     } // end of reading mean value for each bin

    return result;
}
// Trimming histogram and providing trimmed histogram
TH1D * GasDependence ::  trimmed_mean(TH1D * h){
 
   TH1D *h_trim = (TH1D*) h->Clone();
   TH1D * h_trim_new = (TH1D*) h->Clone();
 
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
 
 using HistogramMap = std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>>;
 TString BuildInputFileName(const TString& year, const TString& chamber_name) {
     return "/eos/home-n/nrawal/CSCAgeing/Run2_Ntuples/" + year +
            "_updated_new/csc_output_" + year + "_" + chamber_name +
            "_tree_updated.root";
 }
 void AddYearFiles(TChain* chain, const TString& requested_year, const TString& chamber_name) {
     const TString filename2016 = BuildInputFileName("2016", chamber_name);
     const TString filename2017 = BuildInputFileName("2017", chamber_name);
     const TString filename2018 = BuildInputFileName("2018", chamber_name);
 
     if (requested_year == "2016") chain->Add(filename2016);
     else if (requested_year == "2017") chain->Add(filename2017);
     else if (requested_year == "2018") chain->Add(filename2018);
     else if (requested_year == "run2") {
         chain->Add(filename2016);
         chain->Add(filename2017);
         chain->Add(filename2018);
     }
 }
 bool EnsureChainLoaded(const TChain* chain, const TString& label) {
     if (chain->GetNtrees() == 0) {
         std::cerr << "Error: No trees were loaded into the " << label << " chain!" << std::endl;
         return false;
     }
     return true;
 }
 TFile* OpenOutputFile(const TString& area_name, const TString& chamber_name,
                       const TString& year, const TString& suffix = "") {
     const TString output_file = "./../cumulative_plots/" + area_name +
                                 "/dataset_output_" + chamber_name + "_" +
                                 year + suffix + ".root";
     return new TFile(output_file, "RECREATE");
 }
 GasDependence* MakeGasDependence(const TString& chamber_name, const TString& year,
                                  const TString& area_name, TChain* chain) {
     GasDependence* analyser = new GasDependence();
     analyser->initialise(chamber_name, year, area_name, chain);
     return analyser;
 }
 double RunDependenceAnalysis(TChain* chain,
                              const TString& chamber_name,
                              const TString& year,
                              const TString& area_name,
                              const TString& map_variable,
                              const TString& analysis_variable,
                              TFile* output_file,
                              bool correction,
                              double slope_pressure,
                              double slope_instlumi,
                              bool normalisation,
                              std::vector<double> me13hv3_charge) {
     GasDependence* analyser = MakeGasDependence(chamber_name, year, area_name, chain);
     HistogramMap histograms = analyser->reading_tree_making_maps(
         map_variable, correction, slope_pressure, slope_instlumi, normalisation, me13hv3_charge
     );
     double slope = analyser->analysing_dependence(
         histograms, analysis_variable, output_file, normalisation
     );
     histograms.clear();
     delete analyser;
     return slope;
 }
 std::vector<double> ReadME13HV3Charge(TChain* chain_ME13HV3,
                                       const TString& year,
                                       const TString& area_name,
                                       double slope_pressure_ME13HV3,
                                       double slope_instlumi_ME13HV3,
                                       std::vector<double> me13hv3_charge) {
     GasDependence* analyser = MakeGasDependence("ME13HV3", year, area_name, chain_ME13HV3);
     HistogramMap histograms = analyser->reading_tree_making_maps(
         "instlumi", false, slope_pressure_ME13HV3, slope_instlumi_ME13HV3, false, me13hv3_charge
     );
     std::vector<double> charge = analyser->reading_charge_ME13HV3(histograms, "instlumi");
     histograms.clear();
     delete analyser;
     return charge;
 }
 
int main(int argc, char *argv[]) {

    if(argc<4){
    std::cerr << "Usage: " << argv[0] << " <chamber_name> <year> <area_name>" << std::endl;
    return 1;
    }
    const TString chamber_name = TString::Format("%s", argv[1]) ;
    const TString year = TString::Format("%s", argv[2]) ;
    const TString area_name = TString::Format("%s", argv[3]) ;
    const bool debug = false;
    // Open the ROOT file
    // Create a TChain and add all the trees 
    // with the name of the TTree (e.g., "Events") to read the root files from each year
    TChain *chain = new TChain("tree");
    AddYearFiles(chain, year, chamber_name);

    TChain *chain_ME13HV3 = new TChain("tree");
    AddYearFiles(chain_ME13HV3, year, "ME13HV3");

    if (!EnsureChainLoaded(chain, "main")) return 0;
    if (!EnsureChainLoaded(chain_ME13HV3, "ME13HV3")) return 0;

    TFile* outputFile = OpenOutputFile(area_name, chamber_name, year);
    TFile* outputFile_ME13HV3 = OpenOutputFile(area_name, chamber_name, year, "_ME13HV3");
    //TFile* outputFile_all = OpenOutputFile(area_name, chamber_name, year, "_all");
    // Map to store histograms for each variable
    double slope_pressure = 0, slope_instlumi = 0;
    double slope_pressure_ME13HV3 = 0, slope_instlumi_ME13HV3 = 0;
    double slope_intlumi_initial = 0 , slope_intlumi_final = 0;
    double slope_time_initial = 0 , slope_time_final = 0;
    double  slope_intlumi_final_all = 0,  slope_time_final_all = 0;
    // If you want to check the pressure and instlumi depends on pressure and instlumi after applying correction once
    //double slope_pressure_second = 0, slope_instlumi_second = 0;

    // Initial integrated-luminosity dependence before corrections.
    // The first boolean is whether to apply the pressure correction or not
    // The second boolean should be true only when you want to derive instlumi dependnece, 
    // which should be normalised wrt chargein ME13HV3 chamber
    // In case of deriving pressure correction, slope_pressure should be 0
    // In case of deriving instlumi correction, slope_instlumi should be 0
    // But when it is non-zero, the corrections would be applied
    std::vector<double> ME13HV3_charge(50,0.0);
    RunDependenceAnalysis(chain, chamber_name, year, area_name,
      "intlumi", "intlumi_initial", outputFile,
      false, 0, 0, false, ME13HV3_charge);

    // Pressure dependence for the requested chamber.
    slope_pressure = RunDependenceAnalysis(chain, chamber_name, year, area_name,
                           "pressure", "pressure", outputFile,
                           false, slope_pressure, slope_instlumi,
                           false, ME13HV3_charge);
    
    // Pressure dependence for the ME13HV3 reference chamber.
    slope_pressure_ME13HV3 = RunDependenceAnalysis(chain_ME13HV3, "ME13HV3", year, area_name,
                                   "pressure", "pressure", outputFile_ME13HV3,
                                   false, slope_pressure_ME13HV3,
                                   slope_instlumi_ME13HV3,
                                   false, ME13HV3_charge);
   
    std::cout<<" Pressure slope for chamber "<<chamber_name<<" is "<<slope_pressure<<std::endl;
    std::cout<<" Pressure slope for chamber ME13HV3 is "<<slope_pressure_ME13HV3<<std::endl;
    // Read ME13HV3 charge per instlumi bin; this is later used for normalisation.
    ME13HV3_charge = ReadME13HV3Charge(chain_ME13HV3, year, area_name,
                       slope_pressure_ME13HV3, slope_instlumi_ME13HV3,
                       ME13HV3_charge);
    
    // Instlumi dependence after ME13HV3-based normalisation.
    slope_instlumi = RunDependenceAnalysis(chain, chamber_name, year, area_name,
                           "instlumi", "instlumi", outputFile,
                           false, slope_pressure, slope_instlumi,
                           true, ME13HV3_charge);
    std::cout<<" Instlumi slope for chamber "<<chamber_name<<" is "<<slope_instlumi<<std::endl;
   
    std::vector<double> empty_ME13HV3_charge(50, 0.0);
    // Final integrated-luminosity dependence after pressure and instlumi corrections.
    RunDependenceAnalysis(chain, chamber_name, year, area_name,
           "intlumi", "intlumi_final", outputFile,
           true, slope_pressure, slope_instlumi,
           false, empty_ME13HV3_charge);
    
     outputFile->Close();
     //outputFile_all->Close();
     //outputFile_ME13HV3->Close();
    return 0;
}
// Program to find ME13HV3 charges for each instlumi bins
std::vector<double> GasDependence :: reading_charge_ME13HV3(const std::map<std::pair<int, int>, std::tuple<TH1D*, double, int> > histograms, TString var){
std::vector<double> charge_ME13HV3(50,0.0);
 // Read the histogram in each bin of instlumi and add all the charges together, and find the mean in each bin 
    //std::map<std::string, std::map<int, TH1D*>> cumulativeHistograms;
    std::map<std::string, std::map<int, TH1D*>> cumulativeHistograms;
    double min_var = 0, max_var = 0;
    int nBins = 0 ;
    double binWidth = 0;
    // Access the bin variables
    this->GetVariableBinning(var, min_var, max_var, nBins, binWidth);
   // Individual trimmed mean TGraphs for individual channel 
     std::map<int, int> n_points;
     double x_var_value;
     double y_charge_value;
     double y_charge_err;
    // Trim histograms and make cumulative distribution for each bin of the variable
     for (const auto& [key, value] : histograms){
         // key.first will be chamber name; key.second will be BinNb
         TH1D* h = std::get<0>(value);
         double var_value = std::get<1>(value);
         int entries = std::get<2>(value);
         int rhid = key.first;
         int Bin = key.second;
         int bin_value = (key.second-1) *binWidth + min_var;
         TString Bin_string = TString::Format("%d",Bin);
        // Define condition categories dynamically
        std::vector<std::string> conditions = {"all"}; // Always include "all"
        for (const auto& cond : conditions) {
            createCumulativeHistogram(cumulativeHistograms[cond], Bin, var, cond, bin_value, binWidth, chamber_name, year, false);
            if(h->GetEntries()!=0)
            cumulativeHistograms[cond][Bin]->Add(h);
        }
    } // end of reading histograms into cumulative ones
    // Read mean of the cumulative for each bin of instlumi
    for (const auto& [cond, histMap] : cumulativeHistograms) {
        for (const auto& [key, value] : histMap){
        int pressureBin = key;
        std::cout<<" instlumi bin :"<<pressureBin<<std::endl;
        if(value==NULL || value->GetEntries()==0) continue;
        TH1D* cumulativeHistogram = value;
        TH1D* trimmed_cumulativeHistogram = trimmed_mean(cumulativeHistogram);
        // Calculate the mean of the cumulative histogram
        double mean = trimmed_cumulativeHistogram->GetMean();
        double mean_error = trimmed_cumulativeHistogram->GetMeanError();
        //std::cout<<" charge Bin :"<<pressureBin<<" charge :"<<mean<<" before trim "<<cumulativeHistogram->GetMean()<<std::endl;
        charge_ME13HV3[pressureBin] = mean;

        TCanvas *c = new TCanvas();
        c->cd();
        cumulativeHistogram->Draw();
        TString saving_name = "../cumulative_plots/"+area_name+"/all_channels/ME13HV3/"+var+TString::Format("/cumulative_charge_%d_%s_",pressureBin, var.Data())+"_instlumi_ME13HV3.pdf";
        //c->SaveAs(saving_name);
        TCanvas *c1 = new TCanvas();
        c1->cd();
        trimmed_cumulativeHistogram->Draw();
        TString saving_name_trimmed = "../cumulative_plots/"+area_name+"/all_channels/ME13HV3/"+var+TString::Format("/cumulative_charge_%d_%s_",pressureBin, var.Data())+"_instlumi_ME13HV3_trimmed.pdf";
 //       c1->SaveAs(saving_name_trimmed);

        //delete cumulativeHistogram;
        //delete trimmed_cumulativeHistogram;
        //charge_ME13HV3_error[pressureBin] = mean_error; 
         }
     }     // Read all bins
 return charge_ME13HV3; 
}

// Create individual histogramm for each channel and then fill them with the input histogram
std::map<int, TGraphAsymmErrors*> GasDependnece :: CreateSummaryGraphs(
  const std::map<std::pair<int, int>, std::tuple<TH1D*, double, int> > histograms, 
  const TString& var){
     std::map<int, TGraphAsymmErrors*> h_summary ; 
     for (const auto& [key, value] : histograms){
          // one new summary Histogram for each rhid
          int rhid = key.first;
          std::string channel_name_string =  get_channel_name(rhid);
          TString channel_string(channel_name_string);
          if(h_summary.find(rhid)==h_summary.end()){
            TGraphAsymmErrors *hist = new TGraphAsymmErrors(); 
            TString graph_name = TString::Format("dataset_trimmed_%s_%s", channel_string.Data(), var.Data());
            TString graph_title = chamber_name+" : "+year+" : "+channel_string+" : "+var;
            hist->SetName(graph_name);
            hist->SetTitle(graph_title);
            hist->GetYaxis()->SetTitle("Trimmed mean charge");
            if(var=="pressure" || var=="pressure_second")  hist->GetXaxis()->SetTitle("Pressure (hPa)");
            if(var=="instlumi" || var=="instlumi_second")  hist->GetXaxis()->SetTitle("Instlumi (*10^{34} cm^{2} s^{-1})");
            if(var=="intlumi_initial" || var=="intlumi_final")  hist->GetXaxis()->SetTitle("Integrated lumi  (fb^{-1})");
            if(var=="timesecond_initial" || var=="timesecond_final"){
                hist->GetXaxis()->SetTimeDisplay(1);
                hist->GetXaxis()->SetLabelSize(0.02);
                hist->GetXaxis()->SetTimeFormat("%Y/%m/%d"); 
                hist->GetXaxis()->SetTitle("time");
            }
            gStyle->SetOptStat(111112211);
            gStyle->SetOptFit(1111);
            h_summary[rhid] = hist;
          }
      } // end of for loop
    }
     // Histogram declared for each rhid, now turn is left to fill it

// Create map for cumulativeGraphs
std::map<std::string, std::map<int, TH1D*>> GasDependnece :: CreateCumulativeGraphs(
const std::map<std::pair<int, int>, std::tuple<TH1D*, double, int> > histograms, 
  const TString& var, TString chamber_name, bool normalisation){
     std::map<std::string, std::map<int, TH1D*>>  cumulativeHistograms;
     for (const auto& [key, value] : histograms){
      int Bin = key.second;
      int bin_value = (key.second-1) *binWidth + min_var;
        std::map<TString, int> map_chamber = {
          {"ME11a", 36},  {"ME11b", 36}, {"ME12HV1", 36}, {"ME12HV2", 36}, {"ME12HV3", 36},
          {"ME13HV1", 36}, {"ME13HV2", 36}, {"ME13HV3", 36},
          {"ME21HV1", 18}, {"ME21HV2", 18}, {"ME21HV3", 18},
          {"ME31HV1", 18}, {"ME31HV2", 18}, {"ME31HV3", 18},
          {"ME41HV1", 18}, {"ME41HV2", 18}, {"ME41HV3", 18},
          {"ME22HV1", 36}, {"ME22HV2", 36}, {"ME22HV3", 36}, {"ME22HV4", 36}, {"ME22HV5", 36},
          {"ME32HV1", 36}, {"ME32HV2", 36}, {"ME32HV3", 36}, {"ME32HV4", 36}, {"ME32HV5", 36},
          {"ME42HV1", 36}, {"ME42HV2", 36}, {"ME42HV3", 36}, {"ME42HV4", 36}, {"ME42HV5", 36},
         };
      int upper_nb = map_chamber[chamber_name];
      int lower_nb = upper_nb / 2;
      TString endcap;
      int chamber_nb;
      int layer_nb;
      finding_endcap_chambers(rhid, endcap, chamber_nb,  layer_nb);
        // Define condition categories dynamically
      std::vector<std::string> conditions = {"all"}; // Always include "all"
      if (endcap == "positive") conditions.push_back("plus");
      if (endcap == "negative") conditions.push_back("minus");
      if (chamber_nb % 2 == 0) conditions.push_back("even_chambers"); // Example: Add more conditions
      if (chamber_nb % 2 != 0) conditions.push_back("odd_chambers"); // Example: Add more conditions
      if (layer_nb % 2 != 0) conditions.push_back("odd_layers"); // Example: Add more conditions
      if (layer_nb % 2 == 0) conditions.push_back("even_layers"); // Example: Add more conditions
      if(chamber_nb >=lower_nb+1 && chamber_nb <=upper_nb && endcap=="positive") conditions.push_back("upper_plus");  
      if(chamber_nb >=lower_nb+1 && chamber_nb <=upper_nb && endcap=="negative") conditions.push_back("upper_minus");  
      if(chamber_nb >=1 && chamber_nb <=lower_nb && endcap =="positive") conditions.push_back("lower_plus");  
      if(chamber_nb >=1 && chamber_nb <=lower_nb && endcap =="negative") conditions.push_back("lower_minus");  
      
      for (const auto& cond : conditions){
          // Normalisation is true for charge distribution which are normalised wrt ME13HV3, 
          //this is since our normalised charge distribution is in range (0,5) not (0,3000)
          createCumulativeHistogram(cumulativeHistograms[cond], Bin, var, 
            cond, bin_value, binWidth, chamber_name, year, normalisation);
        }// all the conditions filled
      }// all the rhid value are filled
      return cumulativeHistograms;
  } 
  // end of create cumulative Graphs
void draw_histogram(TH1D*h, int rhid, TString var , int Bin, TString chamber_name, 
  TString year, bool normalisation, TString trim_status){
     TString channel_string = TString::Format("%s", get_channel_name(rhid).c_str());
     TCanvas *c = new TCanvas();
     gStyle->SetOptStat(111112211);
     c->cd();
     h->Draw();
     TString save_name = "../cumulative_plots/"+area_name+"/all_channels/"+chamber_name+"/"+var+
     TString::Format("/charge_distribution_rhid_%s_bin_%d_", channel_string, Bin)+trim_status+"_"+year+".pdf";
     c->SaveAs(save_name);
} 
  // Filling all the histograms with individual charge distribution and then trim also them to make trimmed mean
  void GasDependence::FillCumulativeGraphs(
      const std::map<std::pair<int, int>, std::tuple<TH1D*, double, int> >& cumulative_histograms,
      std::map<int, TGraphAsymmErrors*>& h_summary,
      std::map<std::string, std::map<int, TH1D*>>& histograms,
       const TString& chamber_name,
      const TString& var,
      bool normalisation
  ){
    double min_var = 0, max_var =0 ;
    int nBins = 0 ;
    double binWidth = 0;
    this->GetVariableBinning(var, min_var, max_var, nBins, binWidth);

      // The histogram for each rhid, will be filled for each bin of the variable (pressure, instlumi, intlumi, time)
     //  with the gas gain for each bin; hence we use "n_points" to fill each bin separately
      std::map<int, int> n_points;
      double x_var_value;
      double y_charge_value;
      double y_charge_err;
      // Trim histograms and fill summary graph for each channel
      for (const auto& [key, value] : histograms){
          TH1D* h = std::get<0>(value);
          double var_value = std::get<1>(value);
          int entries = std::get<2>(value);
  
          int rhid = key.first;
          int Bin = key.second;
          int bin_value = (key.second - 1) * binWidth + min_var;
          TString Bin_string = TString::Format("%d", Bin);
          
          // Finding the endcap ; the function to trim the histogram
          TH1D* h_trimmed = trimmed_mean(h);
          if (debug) {std::cout<< "Trimming histogram for RHID: " << key.first << ", " << var << " Bin: " << key.second << " bin value " << bin_value << " hist entries " << h->Integral()
<< " after trim " << h_trimmed->Integral() << " value " << var_value << " entries " << entries << std::endl;}
          h_trimmed->SetName("dataset_trimmed_" + channel_string + "_bin_" + Bin_string + "_vs_" + var);
  
          if (h == NULL || h->GetEntries() == 0)  continue;
              if (n_points.find(rhid) == n_points.end()) {
                  n_points[rhid] = 0;}
              // Avoid filling zero values to graph
              if (h->GetEntries() <= 0) { continue;}
              // If you want to save the histogram
              if(bool_save_hist){ 
                draw_histogram(h, rhid, var, Bin, chamber_name, year, normalisation, "before_trimming"); 
                draw_histogram(h_trimmed, rhid, var, Bin, chamber_name, year, normalisation, "after_trimming"); 
              }
              x_var_value = var_value;
              // We decide to not assing any error to x bin
              //double low_x_err = x_var_value - bin_value;
              //double up_x_err = bin_value + binWidth - x_var_value;
              y_charge_value = h_trimmed->GetMean();
              y_charge_err = h_trimmed->GetMeanError();
              h_summary[rhid]->SetPoint(
                  n_points[rhid],
                  x_var_value,
                  y_charge_value
              );
              h_summary[rhid]->SetPointError(
                  n_points[rhid],
                  0,
                  0,
                  y_charge_err,
                  y_charge_err
              );
              n_points[rhid] = n_points[rhid] + 1;

            TString endcap; int chamber_nb; int layer_nb;
            finding_endcap_chambers(rhid, endcap, chamber_nb,  layer_nb);
            // Filling the corresponding cumulative Histogram
            std::vector<std::string> conditions = {"all"}; // Always include "all"
            if (endcap == "positive") conditions.push_back("plus");
            if (endcap == "negative") conditions.push_back("minus");
            if (chamber_nb % 2 == 0) conditions.push_back("even_chambers"); // Example: Add more conditions
            if (chamber_nb % 2 != 0) conditions.push_back("odd_chambers"); // Example: Add more conditions
            if (layer_nb % 2 != 0) conditions.push_back("odd_layers"); // Example: Add more conditions
            if (layer_nb % 2 == 0) conditions.push_back("even_layers"); // Example: Add more conditions
            if(chamber_nb >=lower_nb+1 && chamber_nb <=upper_nb && endcap=="positive") conditions.push_back("upper_plus");  
            if(chamber_nb >=lower_nb+1 && chamber_nb <=upper_nb && endcap=="negative") conditions.push_back("upper_minus");  
            if(chamber_nb >=1 && chamber_nb <=lower_nb && endcap =="positive") conditions.push_back("lower_plus");  
            if(chamber_nb >=1 && chamber_nb <=lower_nb && endcap =="negative") conditions.push_back("lower_minus");  
            for (const auto& cond : conditions){
                 if(cumulativeHistograms[cond].find(Bin)!=cumulativeHistograms[cond].end())
                  cumulativeHistograms[cond][Bin]->Add(h);
             } // end of cumulative
       } // Filled all rhid
  }
  // end of filling the summary graph

  void GasDependence::finding_endcap_chambers(int rhid, TString& endcap, int& chamber_nb, int& layer_nb){
         int rhid_reduced = static_cast<int>(std::floor(rhid / 10)) % 1000;
          if (rhid > 2000000) {
              rhid_reduced += 400;
          }
          TString endcap = (rhid_reduced <= 400) ? "positive" : "negative";
          std::string channel_name_string = get_channel_name(rhid);
          TString channel_string(channel_name_string);
          int chamber_nb;
          int layer_nb;
          if (rhid_reduced <= 400) {
              chamber_nb = static_cast<int>(std::floor(rhid_reduced / 10));
          } else {
              chamber_nb = static_cast<int>(std::floor((rhid_reduced - 400) / 10));
          }
          layer_nb = rhid_reduced % 10;
    }
    // Found the name and lyaer and everything
 
    void  GasDependence::FitIndieGraphs( TGraphAsymmErrors* hist, TString var){
    //TGraphAsymmErrors* graph  GasDependence::FitIndieGraphs( TGraphAsymmErrors* hist, TString var){
           if(hist==NULL || hist->GetN()==0) continue;
        
           TF1 *expFit2 = nullptr;
           // Only fit for pressure and instlumi
           if(var=="pressure" || var=="pressure_second" || var=="instlumi" || var=="instlumi_second"){
            // Finding first and last point of the edges         
             double fitlowedge = 0, fithighedge = 0; // Initialize edges
             int nPoints = hist->GetN(); // Get the number of points in the graph
             bool foundFirst = false;
             // Loop through all points to find the first and last valid points
             for (int i = 0; i < nPoints; i++) {
                 double x, y;
                 hist->GetPoint(i, x, y);
                 if (y > 0) { // Replace with your condition (e.g., y > threshold)
                     if (!foundFirst) {
                         fitlowedge = x-binWidth; // First valid x-coordinate
                         foundFirst = true;
                     }
                     fithighedge = x+binWidth; // Continuously update with the last valid x-coordinate
                 }
              }
            if(var=="pressure"){
               expFit2 = new TF1("expFit2", "exp([0]) * exp([1]*(x-967))", fitlowedge, fithighedge);
               //expFit2 = new TF1("expFit2", "exp([0]) * exp([1]*(x-967))", min_var, max_var);
               expFit2->SetParameters(5, -0.005); // Initial guesses
               } 
             if(var=="pressure_second"){
               expFit2 = new TF1("expFit2", "exp([0]) * exp([1]*(x-967))", fitlowedge, fithighedge);
               } 
              else if(var=="instlumi" || var=="instlumi_second"){
               //expFit2 = new TF1("expFit2", "exp([0]) * exp([1]*(x-10000))", min_var, max_var);
               expFit2 = new TF1("expFit2", "exp([0]) * exp([1]*(x-1))", fitlowedge, fithighedge);
               }
               hist->Fit(expFit2, "R");
           }
          delete expFit2;
          hist->Write();
       } // end of the fit Function

 

double GasDependence :: analysing_dependence(const std::map<std::pair<int, int>, std::tuple<TH1D*, double, int> > histograms, 
  TString var, TFile * output_file, bool normalisation){
	// The idea is that for each rhid and each bin of pressure, instlumi, there is a histogram. 
 // Save this information in the histogram map corresponding to the rhid. Also later, 
// I want to make a cumulativeHistogram depending on plus endcap , minus endcap, and CFEBs. 
//So another map defined with this string and the corresponding histograms
    gStyle->SetTimeOffset(0.); // for time display in case of time dependence
    // Fill the individual channels into  the histograms for each channel and then make summary graph for each channel
    std::map<int, TGraphAsymmErrors*> h_summary ; 
    std::map<std::string, std::map<int, TH1D*>>& cumulativeHistograms, 
   // h_summary holds the trimmed mean vs variable for each channel 
    h_summary = CreateSummaryGraphs(histograms, var);
   // cumulative histograms holds the charge value for each variable cumulatively
   // We will trim it later to make a summary graph
    cumulativeHistograms = CreateCumulativeGraphs(histograms, var, chamber,  normalisation);
    // Fill individual summary histograms  both for each channel and for each condition
    FillCumulativeGraphs(cumulativeHistograms, h_summary, histograms, var, chamber_name, normalisation);

    // Once we have cumulativeHistograms I want to fit them to obtain the slope for each condition
    // Open the directory file to write the output
    TString dir_name = var;
    TDirectoryFile *dir_var =  (TDirectoryFile*) output_file->mkdir(dir_name);
    dir_var->cd();

    // Fit individual summary graphs
   for(auto& [rhid, hist] : h_summary){
     FitIndieGraphs(hist, var);
   }
    // Make summary histogram for each of the cumulative plot and fit them
    std::map<std::string, double> slope_values ;
    for (const auto& [cond, histMap] : cumulativeHistograms) {
       slope_values[cond] = FitCumulativeGraphs(histograms, histMap, var, chamber_name);
    }
    double slope_value = slope_values["all"];
    return slope_value;
    // Only slope for all chambers is used to correct pressure, and instlumi dependence
}
// End of analysing_dependence function
void GasDependence ::createCumulativeHistogram(std::map<int, TH1D*>& histMap, 
                               int bin, 
                               const TString var, 
                               const TString condition, 
                               double bin_value, 
                               double binWidth, 
                               const TString chamber_name, 
                               const TString year, bool normalisation) {
    if (histMap.find(bin) == histMap.end()) {
        TString histName = Form("cumulative_%s_%d_%s", var.Data(), bin, condition.Data());
        TString histTitle = Form("cumulative: %s: %s : %.0f <= %s < %.0f : %s", 
                                 chamber_name.Data(), year.Data(), bin_value, var.Data(), bin_value + binWidth, condition.Data());

        if(normalisation==true)
        histMap[bin] = new TH1D(histName, histTitle, 50, 0, 5); // Example: adjust binning as needed
        else 
        histMap[bin] = new TH1D(histName, histTitle, 3000, 0, 3000); // Example: adjust binning as needed
        histMap[bin]->Sumw2();
        histMap[bin]->GetXaxis()->SetTitle("charge (ADC)");
        histMap[bin]->GetYaxis()->SetTitle("Nb. of entries");
        histMap[bin]->SetTitle(histTitle); 
    }

}
// End of CreateCumulativeHistogram

// To draw summary histogram from cumulativeHistogrmas
// Return slope values for each cumulative Histogram
double GasDependence :: FitCumulativeGraphs(const std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> histograms , 
  std::map<int, TH1D*> cumulativeHistograms, TString cond,  TString chamber_name,  
  TString var){
    double slope_value = 0;
    std::map<TString, TString> condition_name_map = {
        {"all", "all chambers"},
        {"plus", "+endcap"},
        {"minus", "-endcap"},
        {"odd_chambers", "Odd chambers"},
        {"even_chambers", "Even chambers"},
        {"lower_plus", "Lower +endcap"},
        {"lower_minus", "Lower -endcap"},
        {"upper_plus", "Upper +endcap"},
        {"upper_minus", "Upper -endcap"},
        {"odd_layers", "Odd layers"},
        {"even_layers", "Even layers"}
    };
  
        TGraphAsymmErrors* summaryHistogram = new TGraphAsymmErrors();
        TString cond_name = get_condition_name(cond);
        summaryHistogram->SetName(TString::Format("summary_cumulative_%s_%s_%s", var, cond, chamber_name));
        summaryTitle = TString::Format("Cumulative : %s : %s : %s : %s", chamber_name, year, var, cond_name_map);
        summaryHistogram->SetTitle(summary_title);
        summaryHistogram->GetXaxis()->SetTitle(var);
        summaryHistogram->GetYaxis()->SetTitle("Trimmed mean charge (ADC)");
   // Finding the mean x value for summary Histogram
   std::map<int, double> sum_var_bin;
   std::map<int, double> mean_var_bin;
   std::map<int, double> entries_var_bin;
   std::map<int, int > entries_total;
   for (const auto& [key, value] : histograms){
         int Bin = key.second;
         double var_value = std::get<1>(value);
         int rhid = key.first;
         int entries_total = std::get<2>(value);
      
         if(mean_var_bin.find(Bin)==mean_var_bin.end()){
           sum_var_bin[Bin] = 0; 
           mean_var_bin[Bin] = 0; 
           entries_var_bin[Bin] = 0; 
          } // end of initialising mean, sum and entries
         sum_var_bin[Bin] = sum_var_bin[Bin]+(var_value * entries_total); 
         entries_var_bin[Bin] = entries_var_bin[Bin]+entries_total; 
     }
     for(auto &[key, value] : sum_var_bin){
         mean_var_bin[key] = value/entries_var_bin[key];
     }
    int n_points_summary = 0;
   
    // Reading cumulative Histogram into a summary plot 
    for (auto& pair : cumulativeHistograms) {
        int pressureBin = pair.first;
        TH1D* cumulativeHistogram = pair.second;
        TH1D* trimmed_cumulativeHistogram = trimmed_mean(cumulativeHistogram);
        // Calculate the mean of the cumulative histogram
        double mean = trimmed_cumulativeHistogram->GetMean();
        double mean_error = trimmed_cumulativeHistogram->GetMeanError();

        summaryHistogram->SetPoint(n_points_summary, mean_var_bin[pressureBin], mean);
        summaryHistogram->SetPointError(n_points_summary, 0, 0, mean_error, mean_error);
        if(debug) std::cout<<" point "<<n_points_summary<<" mean "<<mean_var_bin[pressureBin]<<" bin actual "<<pressureBin<<" mean value "<<mean<<std::endl;
        n_points_summary++;

        // Write the cumulative histogram to the output file
        TCanvas *c1 = new TCanvas();
        c1->cd();
        gStyle->SetOptStat(111112211);
        cumulativeHistogram->Draw();
        TString saving_name = "../cumulative_plots/"+area_name+"/all_channels/"+chamber_name+"/"+var+TString::Format("/cumulative_charge_%d_%s_",pressureBin, var.Data())+type_cumulative+".pdf";
       // c1->SaveAs(saving_name);

        TCanvas *c2 = new TCanvas();
        c2->cd();
        gStyle->SetOptStat(111112211);
        trimmed_cumulativeHistogram->Draw();
        TString saving_name_trimmed = "../cumulative_plots/"+area_name+"/all_channels/"+chamber_name+"/"+var+TString::Format("/cumulative_charge_%d_%s_",pressureBin, var.Data())+type_cumulative+"_trimmed.pdf";
        //c2->SaveAs(saving_name_trimmed);

        //cumulativeHistogram->Write();
        delete c1;
        delete c2;
        delete cumulativeHistogram; // Clean up cumulative histograms
        delete trimmed_cumulativeHistogram; // Clean up cumulative histograms
    }
     summaryHistogram->GetYaxis()->SetRangeUser(250,600);
     if(var=="instlumi")
     summaryHistogram->GetYaxis()->SetRangeUser(0,2.5);
     summaryHistogram->GetYaxis()->SetTitle("Trimmed mean charge");
      if(var=="pressure" || var=="pressure_second")  {
        summaryHistogram->GetXaxis()->SetTitle("Pressure (hPa)");
        summaryHistogram->GetXaxis()->SetRangeUser(940, 990);
      }
      if(var=="instlumi" || var=="instlumi_second"){
        summaryHistogram->GetXaxis()->SetTitle("Instlumi (*10^{34} cm^{2} s^{-1})");
        summaryHistogram->GetXaxis()->SetRangeUser(0,3);
      }
      if(var=="intlumi_initial" || var=="intlumi_final") {
        summaryHistogram->GetXaxis()->SetRangeUser(0,160);
        summaryHistogram->GetXaxis()->SetTitle("Integrated lumi  (fb^{-1})");
      }
      if(var=="timesecond_initial" || var=="timesecond_final"){
          summaryHistogram->GetXaxis()->SetTimeDisplay(1);
          summaryHistogram->GetXaxis()->SetLabelSize(0.02);
          summaryHistogram->GetXaxis()->SetTimeFormat("%Y/%m/%d"); 
          summaryHistogram->GetXaxis()->SetTitle("time");
       }

     if(debug) std::cout<<" number of points "<<summaryHistogram->GetN();
     summaryHistogram->SetMarkerStyle(20);
     summaryHistogram->SetMarkerSize(0.5);

     // Fitting the summaryHistogram in case of pressure and instlumi
     FitIndieGraphs(summaryHistogram, var);
     summaryHistogram->SetName("dataset_trimmed_"+chamber_name+"_allgoodchannelsvs_"+var+"_"+type_cumulative);
     summaryHistogram->Write();
     delete summaryHistogram;
     return slope_value;
}

double GasDependence :: ApplyCorrection(double X ,TString correctiontype,  double slope ){
  double refvalue = 0;
  if(correctiontype=="pressure"){
    refvalue =967 ;
  }
  else if (correctiontype=="instlumi"){
    refvalue = 1 ;
    X  = (X/10000.0);
  }
  else if (correctiontype=="_n_PV"){
    refvalue = 50 ;
  }
 double thecorr =exp(slope*(refvalue-X));
 //double thecorr = p1*(refvalue-X);
 return thecorr;
 }
