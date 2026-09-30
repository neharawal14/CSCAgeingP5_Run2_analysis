#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TH1D.h>
#include "TFile.h"
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
#include "badChannel.h"
struct RunRegion{
    double RunMin;
    double RunMax;
    double medianHV;
};


std::vector<RunRegion> GetRunRegions(TString year, TString) ;
std::map<int, std::map<TString, std::vector<RunRegion>>> BuildMedianMap(
    TTree *tree,
    TString year, TString chamber);

double ComputeMedian(std::vector<double> values) ;
std::vector<RunRegion> GetRunRegions(TString year, TString chamber) {
    if (year == "2016") {
      if(chamber=="ME11a" || chamber=="ME11b"){
        return {
            {272007, 281613, NAN},
            {281613, 286520, NAN}
        };
      }
     else{ 
       return{
            {272007, 277792 , NAN},
            {277792 , 281613,  NAN}, 
            {281613, 286520, NAN}
       }; 
     }
    } // end of 2016
    else if (year == "2018") {
     if(chamber.Contains("ME11") || chamber.Contains("ME22")  || chamber.Contains("ME32")   || chamber.Contains("ME42")  || chamber.Contains("ME12")   || chamber.Contains("ME13")){
        return { 
            {314472, 324077, NAN},
            {324077, 325175, NAN}
        };
      }
    else{
	  return { 
 	    {314472, 325175, NAN}, 
          };
      }
   } // end of 2018
    else if (year == "2017") {
        return { 
            {294927, 307082, NAN},
        };
   }

   else if (year == "run2") {

     if(chamber.Contains("ME11")){
        return { 
            {272007, 281613, NAN}, 
            {281613, 286520, NAN}, 
            {294927, 307082, NAN}, 
            {314472, 324077, NAN}, 
            {324077, 325175, NAN} 
        };
   }
    
     else if(chamber.Contains("ME11") || chamber.Contains("ME22")  || chamber.Contains("ME32")   || chamber.Contains("ME42")  || chamber.Contains("ME12")   || chamber.Contains("ME13")){
       return{
            {272007, 277792 , NAN},
            {277792 , 281613,  NAN},
            {281613, 286520, NAN},
            {294927, 307082, NAN},
            {314472, 324077, NAN},
            {324077, 325175, NAN}
       };
     }
    else { return {
            {272007, 277792 , NAN},
            {277792 , 281613,  NAN},
            {281613, 286520, NAN},
            {294927, 307082, NAN},
 	    {314472, 325175, NAN}
     }; 
    }
   } // end of Run2
  else {
     return {    {272007, 325175, NAN} } ;
  }
}
double ComputeMedian(std::vector<double> values) {
    if (values.empty()) return NAN;

    std::sort(values.begin(), values.end());

    int n = values.size();

    if (n % 2 == 1) {
        return values[n / 2];
    } else {
        return 0.5 * (values[n / 2 - 1] + values[n / 2]);
    }
}
std::map<int, std::map<TString, std::vector<RunRegion>>> BuildMedianMap(
    TChain *tree,// Long64_t & _rhid_local , Long64_t &_runNb_local, double &_HV_local, 
    TString year, TString chamber
)
{
    std::map<int, std::map<TString, std::vector<std::vector<double>>>> hvValuesMap;
    Long64_t _rhid_local;
    double _HV_local;
    Long64_t _runNb_local;
    tree->SetBranchAddress("_rhid", &_rhid_local);
    tree->SetBranchAddress("_HV", &_HV_local);
    tree->SetBranchAddress("_runNb", &_runNb_local);
    for (Long64_t i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        std::vector<RunRegion> regions =
            GetRunRegions(year, chamber);

        if (regions.empty()) continue;

        for (int r = 0; r < (int)regions.size(); r++) {
            if (_runNb_local >= regions[r].RunMin &&
                _runNb_local <  regions[r].RunMax)
            {
                hvValuesMap[_rhid_local][year].resize(regions.size());
                hvValuesMap[_rhid_local][year][r].push_back(_HV_local);
                break;
            }
        }
    }

    std::map<int, std::map<TString, std::vector<RunRegion>>> hvMedianMap;
    for (auto &rhidEntry : hvValuesMap) {
        int rhid = rhidEntry.first;
        for (auto &yearEntry : rhidEntry.second) {
            TString year = yearEntry.first;
            std::vector<RunRegion> regions =
                GetRunRegions(year, chamber);
            for (int r = 0; r < (int)regions.size(); r++) {
                double medianHV =
                    ComputeMedian(yearEntry.second[r]);
                regions[r].medianHV = medianHV;
            }
            hvMedianMap[rhid][year] = regions;
        }
    }
    tree->ResetBranchAddresses();
    return hvMedianMap;

}
// Declaring all the functions 
struct ChannelInfo {
    int endcap;              // 1 or 2
    int chamber;
    int layer;
    int rhid_reduced;
    std::string channel_name;
    bool valid;
};

ChannelInfo decode_rhid(int rhid) {

    ChannelInfo info;

    info.valid = false;
    info.endcap = -1;
    info.chamber = -1;
    info.layer = -1;
    info.rhid_reduced = -1;
    info.channel_name = "undefined";

    int rhid_reduced = static_cast<int>(std::floor(rhid / 10)) % 1000;

    if (rhid > 2000000) {
        rhid_reduced += 400;
    }

    info.rhid_reduced = rhid_reduced;

    if (rhid_reduced == 0 || rhid_reduced == 771) {
        return info;
    }

    if (rhid_reduced <= 400) {
        info.endcap = 1;
        info.chamber = static_cast<int>(std::floor(rhid_reduced / 10));
    } else {
        info.endcap = 2;
        info.chamber = static_cast<int>(std::floor((rhid_reduced - 400) / 10));
    }

    info.layer = rhid_reduced % 10;

    std::string endcap_string =
        (info.endcap == 1) ? "_Endcap1" : "_Endcap2";

    info.channel_name =
        "chamber" + std::to_string(info.chamber) +
        "_layer" + std::to_string(info.layer) +
        endcap_string;

    info.valid = true;

    return info;
}


double GetMedianHV(
    int rhid,
    TString year,
    int runNb,
    const std::map<int, std::map<TString, std::vector<RunRegion>>> &hvMedianMap
) {
    auto itRhid = hvMedianMap.find(rhid);
    if (itRhid == hvMedianMap.end()) return NAN;
    auto itYear = itRhid->second.find(year);
    if (itYear == itRhid->second.end()) return NAN;
    const std::vector<RunRegion> &regions = itYear->second;
    for (const auto &region : regions) {
        if (runNb >= region.RunMin && runNb < region.RunMax) {
            return region.medianHV;
        }
    }

    return NAN;
}


class GasDependence {

 public : 
    // variables
    TString area_name;
    TString year;
    TString chamber_name;
    bool debug = false;

    double _rhsumQ_equalised_HV_data, _rhsumQ_RAW;
    double _rhsumQ;
    double _pressure;
    Int_t _n_PV;
    Long64_t _rhid;
    double _integratelumi, _instlumi;
    double _instlumi_dcsjson;
    double _intlumi_delivered_dcsjson;
    double _intlumi_delivered_goldenjson;
    Long64_t _lumiBlock;
    double  _HV_nominal;
    double  _HV;
    Long64_t _runNb; 
    Long64_t _eventNb; 
    Long64_t _timesecond;
    Long64_t _nearestStrip;
    TChain *tree;
    std::map<int, std::map<TString, std::vector<RunRegion>>> hvMedianMap;


    TH1D *  trimmed_mean(TH1D * h);
    void initialise(TString chamber_name_string , TString year_value ,TString area_name_string, TChain*, std::map<int, std::map<TString, std::vector<RunRegion>>> ); 
    double analysing_dependence(const std::map<std::pair<int, int>, std::tuple<TH1D*, double, int> > histograms, TString var, TFile *, bool);
    std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> reading_tree_making_maps(TString variable, bool, double, double, bool, std::vector<double>) ;
    std::string get_channel_name(int rhid);
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

};

void GasDependence :: initialise(TString chamber_name_string , TString year_value ,TString area_name_string, TChain *chain_here, std::map<int, std::map<TString, std::vector<RunRegion>>> HV_Map){

  area_name = area_name_string;
  chamber_name = chamber_name_string;
  year = year_value;
  tree  = chain_here;
  hvMedianMap = HV_Map;
  tree->SetBranchAddress("_rhsumQ_equalised_HV_data", &_rhsumQ_equalised_HV_data);
  tree->SetBranchAddress("_n_PV", &_n_PV);
  tree->SetBranchAddress("_rhsumQ", &_rhsumQ);
  tree->SetBranchAddress("_rhsumQ_RAW", &_rhsumQ_RAW);
  tree->SetBranchAddress("_pressure", &_pressure);
  tree->SetBranchAddress("_integratelumi", &_integratelumi);
  tree->SetBranchAddress("_intlumi_delivered_dcsjson", &_intlumi_delivered_dcsjson);
  tree->SetBranchAddress("_intlumi_delivered_goldenjson", &_intlumi_delivered_goldenjson);
  tree->SetBranchAddress("_timesecond", &_timesecond);
  tree->SetBranchAddress("_nearestStrip", &_nearestStrip);
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


std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> GasDependence :: reading_tree_making_maps(TString variable, 
bool correction, double slope_pressure, double slope_instlumi, bool normalisation , std::vector<double> ME13HV3_charge) {
	
     gStyle->SetTimeOffset(0.);
     // Map to store histograms and mean values
    std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> result;
    // Map to track cumulative sums and counts for calculating mean values
    std::map<std::pair<int, int>, std::pair<double, int>> binStats;

    double min_var, max_var;
    int nBins;
    TString year_value;
    if (variable == "pressure") {
        min_var = 940;
        max_var = 990;
        nBins = 50;
    } 
    else if (variable == "instlumi") {
        min_var = 0;
        max_var = 2.5;
        nBins = 50;

    }
    else if (variable == "PV") {
        min_var = 0;
        max_var = 100;
        nBins =100;
    }

    else if (variable == "intlumi") {
        min_var = 0;
        max_var = 160;
        nBins = 160;
    }
    else if (variable == "time") {
      int bin_width = 86400;
      if(year=="2016") { 
        min_var  = 1462838400 ; // 10 May 2016 : 00 : 00 : 00
        max_var= 1477871999; // 30 Oct 2016 : 23 : 59 : 59
       }
      else if(year=="2017"){
        min_var  = 1497484800 ; // 15 June 2017 : 00 : 00 : 00 
        max_var= 1510790399; // 15 Nov 2017 : 23 : 59 : 59
      }
      else if(year=="2018"){
        min_var  = 1527206400 ; // 25 April 2018 : 00 : 00 : 00
        max_var =  1540511999; // 25 Oct 2018 : 23 : 59 : 59
      }
      else if(year=="run2")
      {
        min_var  = 1462838400 ; // 10 May 2016 : 00 : 00 : 00
        max_var =  1540511999; // 25 Oct 2018 : 23 : 59 : 59
      }
      nBins = ((max_var - min_var + 1) / bin_width);
    }

    double binWidth = (max_var - min_var) / nBins;
    int nEntries = tree->GetEntries();

    std::cout<<" building Median Map "<<std::endl;
     double tolerance = 5.0;

    for (ULong64_t i = 0; i < nEntries; i++) {
    //for (ULong64_t i = 0; i < 10000; i++) {
        tree->GetEntry(i);
       // Removing even Strips
       //if(_lumiBlock%2==0) continue;
       if(_nearestStrip%2==0) continue;

        // removing the recuperated and wrong gas composition period 
         double medianHV = GetMedianHV(
             _rhid,
             year,
             _runNb,
             hvMedianMap
         );
     if (std::isnan(medianHV)) continue;
     if (std::abs(_HV - medianHV) > tolerance) {
        // Not accept point
        continue; 
    }
    if(year=="run2") {
         if(_timesecond >=1451606400  && _timesecond <= 1483142400) year_value = "2016";
         else if(_timesecond >=1483228800 && _timesecond <=1514678400 ) year_value = "2017";
         else if(_timesecond >=1514764800 && _timesecond <=1546214400 ) year_value = "2018";
    }
   else year_value = year;
        bool badchannel = isbadchannel(chamber_name , _rhid , _nearestStrip,  year_value);
       // std::cout<<" accept or not "<<badchannel<<std::endl;
        if(badchannel) continue;

   
        // Skip invalid entries based on conditions
        int Bin;
        if(variable=="pressure") {
          if (_pressure < min_var || _pressure >= max_var) continue;
            Bin = static_cast<int>( ((_pressure - min_var) / binWidth));
         }
        else if(variable=="instlumi")  { 
          if (_instlumi_dcsjson/10000.0 < min_var || _instlumi_dcsjson/10000.0 >= max_var) continue;
           Bin = static_cast<int>( (( (_instlumi_dcsjson/10000.0) - min_var) / binWidth)); 
           //std::cout<<" bin "<<Bin<<std::endl;
         }
        else if(variable=="PV")  {
          if (_n_PV < min_var || _n_PV >= max_var) continue;
           Bin = static_cast<int>( ( (_n_PV - min_var) / binWidth)); //std::cout<<" bin "<<Bin<<std::endl;
        }
        else if(variable=="intlumi")  {
          if (_intlumi_delivered_dcsjson < min_var || _intlumi_delivered_dcsjson >= max_var) continue;
            Bin = static_cast<int>( ((_intlumi_delivered_dcsjson - min_var) / binWidth));
        }
        else if(variable=="time") {
          if (_timesecond < min_var || _timesecond >= max_var) continue;
          Bin = static_cast<int>( ((_timesecond - min_var) / binWidth));
        }
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
                  Form("h_rhid_%d_%s_second_%d", _rhid, variable.Data(), Bin),
                  Form("Histogram for RHID %d, %s Bin %d  : after correction", _rhid, variable.Data(), Bin),
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

	if(this->chamber_name=="ME13HV3")
        equalised_charge = _rhsumQ_equalised_HV_data * ApplyCorrection(_pressure ,"pressure", slope_pressure) ;
	else 
        equalised_charge = _rhsumQ_equalised_HV_data * ApplyCorrection(_pressure ,"pressure", slope_pressure) * 
        ApplyCorrection(_instlumi_dcsjson/10000.0, "instlumi", slope_instlumi);

        // Normalise charges for deriving instlumi dependence with charges in ME13HV3
        if(normalisation==true && (variable=="instlumi" || variable =="PV" || variable=="instlumi_second")) {

          if(Bin <0 || Bin >= ME13HV3_charge.size()) continue;
          //if(ME13HV3_charge[Bin]<=0) continue;
          //if(ME13HV3_charge[Bin]!=0)
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
          double ratio = (_instlumi_dcsjson/10000.0);
          binStats[reducedKey].first += (_instlumi_dcsjson/10000.0); // Cumulative sum of pressure values binStats[reducedKey].second += 1;        // Count of pressure values
          binStats[reducedKey].second += 1;        // Count of pressure value
        }
        else if(variable=="PV"){        
        binStats[reducedKey].first += _n_PV; // Cumulative sum of pressure values
        binStats[reducedKey].second += 1;        // Count of pressure value
        }
        else if(variable=="intlumi"){        
        binStats[reducedKey].first += _intlumi_delivered_dcsjson; // Cumulative sum of pressure values
        binStats[reducedKey].second += 1;        // Count of pressure values
        }
        else if(variable=="time"){        
        binStats[reducedKey].first += _timesecond; // Cumulative sum of pressure values
        binStats[reducedKey].second += 1;        // Count of pressure values
        }
        // Ensure the histogram exists for the reduced key

    } // end of going through each tree entry
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
    // This result is basically  for each rhid and bin - we have a histogram, the average value, and the total count 
    // Hence the format  std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> 
    //  Map of [rhid, bin_nb] ->  [TH1D*, mean value, count]
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
                  last_bin =it;
                }
              }
             double new_integral =0;
            int entries_last_bin = 0;
            for(int it=1; it<last_bin; it++){
              new_integral += h_trim->GetBinContent(it);
            }
 
            entries_last_bin = (int) (trimmean*normal - new_integral);
            for(int it=1; it<last_bin ; it++){
              h_trim_new->SetBinContent(it,h_trim->GetBinContent(it));
              h_trim_new->SetBinError(it,h_trim->GetBinError(it));
            }
            h_trim_new->SetBinContent(last_bin, entries_last_bin);
            if(entries_last_bin!=0) {
            h_trim_new->SetBinError(last_bin, h_trim->GetBinError(last_bin) * (entries_last_bin / h_trim->GetBinContent(last_bin)));
            }
            for(int it=last_bin+1; it<=h_trim->GetNbinsX() ; it++){
              h_trim_new->SetBinContent(it,0);
              h_trim_new->SetBinError(it,0);
            }
            float final_integral = new_integral + entries_last_bin;
            float check_integral = normal * trimmean;
           // std::cout<<" final integral "<<final_integral<<" normal "<<check_integral<<std::endl;
           std::pair<float, float> trimmed_mean_value;
           trimmed_mean_value.first = h_trim_new->GetMean();
           trimmed_mean_value.second = h_trim_new->GetMeanError();
           return h_trim_new;
 }

 
int main(int argc, char *argv[]) {

    TString chamber_name = TString::Format("%s", argv[1]) ;
    TString year = TString::Format("%s", argv[2]) ;
    TString area_name = TString::Format("%s", argv[3]) ;
    // Open the ROOT file
    // Create a TChain with the name of the TTree (e.g., "Events")
    TChain *chain = new TChain("tree");

    // Add ROOT files to the chain
    TString filename1 ="/eos/home-n/nrawal/CSCAgeing/Run2_Ntuples/2016_updated_new/csc_output_2016_"+chamber_name+"_tree_updated.root" ;
    TString filename2 ="/eos/home-n/nrawal/CSCAgeing/Run2_Ntuples/2017_updated_new/csc_output_2017_"+chamber_name+"_tree_updated.root" ;
    TString filename3 ="/eos/home-n/nrawal/CSCAgeing/Run2_Ntuples/2018_updated_new/csc_output_2018_"+chamber_name+"_tree_updated.root" ;

   if(year=="2016") chain->Add(filename1);
   else if(year=="2017") chain->Add(filename2);
   else if(year=="2018") chain->Add(filename3);

   else if(year=="run2") {
   chain->Add(filename1);
   chain->Add(filename2);
   chain->Add(filename3);
   }
   if (chain->GetNtrees() == 0) {
    std::cerr << "Error: No trees were loaded into the chain!" << std::endl;
    return 0;
    }
    TChain *chain_ME13HV3 = new TChain("tree");

    // Add ROOT files to the chain
    TString filename1_ME13HV3 ="/eos/home-n/nrawal/CSCAgeing/Run2_Ntuples/2016_updated_new/csc_output_2016_ME13HV3_tree_updated.root" ;
    TString filename2_ME13HV3 ="/eos/home-n/nrawal/CSCAgeing/Run2_Ntuples/2017_updated_new/csc_output_2017_ME13HV3_tree_updated.root" ;
    TString filename3_ME13HV3 ="/eos/home-n/nrawal/CSCAgeing/Run2_Ntuples/2018_updated_new/csc_output_2018_ME13HV3_tree_updated.root" ;
   if(year=="2016") chain_ME13HV3->Add(filename1_ME13HV3);
   else if(year=="2017") chain_ME13HV3->Add(filename2_ME13HV3);
   else if(year=="2018") chain_ME13HV3->Add(filename3_ME13HV3);

   else if(year=="run2") {
   chain_ME13HV3->Add(filename1_ME13HV3);
   chain_ME13HV3->Add(filename2_ME13HV3);
   chain_ME13HV3->Add(filename3_ME13HV3);
   }
   if (chain_ME13HV3->GetNtrees() == 0) {
    std::cerr << "Error: No trees were loaded into the chain!" << std::endl;
    return 0;
    }

    TString output_file = "./../cumulative_plots/"+area_name + "/"+"dataset_output_"+chamber_name+"_"+year+".root";
    TFile*  outputFile = new TFile(output_file,"RECREATE");
    TString output_file_ME13HV3 = "./../cumulative_plots/"+area_name + "/"+"dataset_output_"+chamber_name+"_"+year+"_ME13HV3.root";
    TFile*  outputFile_ME13HV3 = new TFile(output_file_ME13HV3,"RECREATE");

    std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> histograms_intlumi_initial;
    std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> histograms_time_initial;
    std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> histograms_intlumi_final;
    std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> histograms_time_final;
    std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> histograms_pressure;
    std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> histograms_pressure_second;
    std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> histograms_instlumi;
    std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> histograms_instlumi_second;
    std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> histograms_intlumi_final_all;
    std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> histograms_time_final_all;
    std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> histograms_PV;


    std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> histograms_pressure_ME13HV3;
    std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> histograms_PV_ME13HV3;
    std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> histograms_instlumi_ME13HV3;
    std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> histograms_instlumi_ME13HV3_second;
    double slope_pressure = 0, slope_instlumi = 0;
    double slope_pressure_ME13HV3 = 0, slope_instlumi_ME13HV3 = 0;
    double slope_PV_ME13HV3 = 0;
    double slope_pressure_second = 0, slope_instlumi_second = 0;
    double slope_intlumi_initial = 0 , slope_intlumi_final = 0;
    double  slope_intlumi_final_all = 0;
    double  slope_time_final_all = 0;
    double slope_time_initial = 0 , slope_time_final = 0;
    double slope_PV = 0;

  std::cout<<"Starting now "<<std::endl;
  std::vector<double> ME13HV3_charge(50,0.0);
  std::vector<double> ME13HV3_charge_second(50,0.0);
  std::vector<double> ME13HV3_PV_charge(100,0.0);

  // HV map which to reject the entries
  std::map<int, std::map<TString, std::vector<RunRegion>>> hvMedianMap;
  std::map<int, std::map<TString, std::vector<RunRegion>>> hvMedianMap_ME13HV3;
  hvMedianMap =
        BuildMedianMap(
            chain,
            year, chamber_name
        );

  hvMedianMap_ME13HV3 =
        BuildMedianMap(
            chain_ME13HV3,
            year, "ME13HV3"
        );


    // Map to store cumulative histograms for each pressure bin
///   GasDependence *obj_intlumi_initial = new GasDependence();
///   obj_intlumi_initial->initialise(chamber_name, year, area_name, chain, hvMedianMap);
///   histograms_intlumi_initial = obj_intlumi_initial->reading_tree_making_maps("intlumi", false, 0, 0 , false, ME13HV3_charge);
///   slope_intlumi_initial = obj_intlumi_initial->analysing_dependence(histograms_intlumi_initial, "intlumi_initial", outputFile, false);
///   histograms_intlumi_initial.clear();
///   delete obj_intlumi_initial;

//    GasDependence *obj_time_initial = new GasDependence();
//    obj_time_initial->initialise(chamber_name, year, area_name, chain, hvMedianMap);
//    histograms_time_initial = obj_time_initial->reading_tree_making_maps("time", false, 0, 0, false, ME13HV3_charge);
//    slope_time_initial = obj_time_initial->analysing_dependence(histograms_time_initial, "timesecond_initial", outputFile, false);
//    delete obj_time_initial;

 // Pressure corrections now
   GasDependence *obj_pressure = new GasDependence();
   obj_pressure->initialise(chamber_name, year, area_name, chain, hvMedianMap);
   histograms_pressure = obj_pressure->reading_tree_making_maps("pressure", false, slope_pressure, slope_instlumi, false, ME13HV3_charge);
   slope_pressure = obj_pressure->analysing_dependence(histograms_pressure, "pressure", outputFile, false);
   histograms_pressure.clear();
   delete obj_pressure;

//   GasDependence *obj_pressure_second = new GasDependence();
//   obj_pressure_second->initialise(chamber_name, year, area_name, chain, hvMedianMap);
//   histograms_pressure_second = obj_pressure_second->reading_tree_making_maps("pressure", true, slope_pressure, slope_instlumi, false, ME13HV3_charge);
//   slope_pressure_second = obj_pressure_second->analysing_dependence(histograms_pressure_second, "pressure_second", outputFile, false);
//   histograms_pressure_second.clear();
//   delete obj_pressure_second;

   //ME13HV3 pressure corrections to remove the things
   GasDependence *obj_pressure_ME13HV3 = new GasDependence();
   obj_pressure_ME13HV3->initialise("ME13HV3", year, area_name, chain_ME13HV3, hvMedianMap_ME13HV3);
   histograms_pressure_ME13HV3 = obj_pressure_ME13HV3->reading_tree_making_maps("pressure", false, slope_pressure_ME13HV3, slope_instlumi_ME13HV3, false,ME13HV3_charge);
   slope_pressure_ME13HV3 = obj_pressure_ME13HV3->analysing_dependence(histograms_pressure_ME13HV3, "pressure", outputFile_ME13HV3, false);
   histograms_pressure_ME13HV3.clear();
   delete obj_pressure_ME13HV3;

    // instlumi dependence 
    GasDependence *obj_instlumi_ME13HV3 = new GasDependence();
    obj_instlumi_ME13HV3->initialise("ME13HV3", year, area_name, chain_ME13HV3, hvMedianMap_ME13HV3);
    histograms_instlumi_ME13HV3 = obj_instlumi_ME13HV3->reading_tree_making_maps("instlumi", false, 
      slope_pressure_ME13HV3, slope_instlumi_ME13HV3, false, ME13HV3_charge);
    ME13HV3_charge = obj_instlumi_ME13HV3->reading_charge_ME13HV3(histograms_instlumi_ME13HV3, "instlumi");
    histograms_instlumi_ME13HV3.clear();
    delete obj_instlumi_ME13HV3;

//    GasDependence *obj_instlumi_ME13HV3_second = new GasDependence();
//    obj_instlumi_ME13HV3_second->initialise("ME13HV3", year, area_name, chain_ME13HV3, hvMedianMap_ME13HV3);
//    histograms_instlumi_ME13HV3_second = obj_instlumi_ME13HV3_second->reading_tree_making_maps("instlumi", true, 
//      slope_pressure_ME13HV3, slope_instlumi_ME13HV3, false, ME13HV3_charge_second);
//    ME13HV3_charge_second = obj_instlumi_ME13HV3_second->reading_charge_ME13HV3(histograms_instlumi_ME13HV3_second, "instlumi");
//    histograms_instlumi_ME13HV3_second.clear();
//    delete obj_instlumi_ME13HV3_second;

///    // instlumi dependence 
    GasDependence *obj_instlumi = new GasDependence();
    obj_instlumi->initialise(chamber_name, year, area_name, chain, hvMedianMap);
    histograms_instlumi = obj_instlumi->reading_tree_making_maps("instlumi", false, slope_pressure, slope_instlumi, true, ME13HV3_charge);
    slope_instlumi = obj_instlumi->analysing_dependence(histograms_instlumi, "instlumi", outputFile, true);
    //histograms_instlumi = obj_instlumi->reading_tree_making_maps("instlumi", false, slope_pressure, slope_instlumi, false, ME13HV3_charge);
    //slope_instlumi = obj_instlumi->analysing_dependence(histograms_instlumi, "instlumi", outputFile, false);
    histograms_instlumi.clear();
    delete obj_instlumi;

//    GasDependence *obj_instlumi_second = new GasDependence();
//    obj_instlumi_second->initialise(chamber_name, year, area_name, chain, hvMedianMap);
//    histograms_instlumi_second = obj_instlumi_second->reading_tree_making_maps("instlumi", true, slope_pressure, slope_instlumi, true, ME13HV3_charge_second);
//    slope_instlumi_second = obj_instlumi_second->analysing_dependence(histograms_instlumi_second, "instlumi_second", outputFile, true);
//    histograms_instlumi_second.clear();
//    delete obj_instlumi_second;

//   GasDependence *obj_PV_ME13HV3 = new GasDependence();
//    obj_PV_ME13HV3->initialise("ME13HV3", year, area_name, chain_ME13HV3, hvMedianMap_ME13HV3);
//    histograms_PV_ME13HV3 = obj_PV_ME13HV3->reading_tree_making_maps("PV", false, 
//      slope_pressure_ME13HV3, slope_instlumi_ME13HV3, false, ME13HV3_PV_charge);
//    ME13HV3_PV_charge = obj_PV_ME13HV3->reading_charge_ME13HV3(histograms_PV_ME13HV3, "PV");
//    std::cout<<" finished reading charge for ME13HV3 PV dependence "<<std::endl;


//   GasDependence *obj_PV = new GasDependence();
//    obj_PV->initialise(chamber_name, year, area_name, chain, hvMedianMap);
//    histograms_PV = obj_PV->reading_tree_making_maps("PV", false, slope_pressure, slope_instlumi, true, ME13HV3_PV_charge);
//    slope_PV = obj_PV->analysing_dependence(histograms_PV, "PV", outputFile, true);
//    histograms_PV.clear();
//    delete obj_PV;
    
    GasDependence *obj_intlumi_final = new GasDependence();
    obj_intlumi_final->initialise(chamber_name, year, area_name, chain, hvMedianMap);
    histograms_intlumi_final = obj_intlumi_final->reading_tree_making_maps("intlumi", true, slope_pressure, slope_instlumi, false, ME13HV3_PV_charge);
    slope_intlumi_final = obj_intlumi_final->analysing_dependence(histograms_intlumi_final, "intlumi_final", outputFile, false);
    histograms_intlumi_final.clear();
    delete obj_intlumi_final;

//    GasDependence *obj_time_final = new GasDependence();
//    obj_time_final->initialise(chamber_name, year, area_name, chain, hvMedianMap);
//    histograms_time_final = obj_time_final->reading_tree_making_maps("time", true, slope_pressure, slope_instlumi, false, ME13HV3_charge_new);
//    slope_time_final = obj_time_final->analysing_dependence(histograms_time_final, "timesecond_final", outputFile, false);
//    delete obj_time_final;

   outputFile->Close();
   outputFile_ME13HV3->Close();
    return 0;
}
// Program to find ME13HV3 charges for each instlumi bins
std::vector<double> GasDependence :: reading_charge_ME13HV3(const std::map<std::pair<int, int>, std::tuple<TH1D*, double, int> > histograms, TString var){
 std::vector<double> charge_ME13HV3;
 if(var=="instlumi") charge_ME13HV3.assign(50,0.0);
 else if(var=="PV") charge_ME13HV3.assign(100,0.0);
 // Read the histogram in each bin of instlumi and add all the charges together, and find the mean in each bin 
 std::map<std::string, std::map<int, TH1D*>> cumulativeHistograms;
 double min_var, max_var;
 int nBins;
 if(var=="pressure" || var=="pressure_second") {
    min_var = 940; 
    max_var = 990;
    nBins = 50; 
  }
 else if(var=="instlumi" || var=="instlumi_second") {
    min_var = 0; 
    max_var = 2.5;
    nBins = 50; 
  }
 else if(var=="PV") {
    min_var = 0; 
    max_var = 100;
    nBins = 100; 
  }

 else if(var=="intlumi_initial" || var=="intlumi_final") {
    min_var = 0; 
    max_var = 160;
    nBins = 160; 
  }
 else if (var == "timesecond_initial" || var=="timesecond_final") {
   int bin_width = 86400;
   if(year=="2016") { 
     min_var  = 1462838400 ; // 10 May 2016 : 00 : 00 : 00
     max_var= 1477871999; // 30 Oct 2016 : 23 : 59 : 59
    }
   else if(year=="2017"){
     min_var  = 1497484800 ; // 15 June 2017 : 00 : 00 : 00 
     max_var= 1510790399; // 15 Nov 2017 : 23 : 59 : 59
   }
   else if(year=="2018"){
     min_var  = 1527206400 ; // 25 April 2018 : 00 : 00 : 00
     max_var =  1540511999; // 25 Oct 2018 : 23 : 59 : 59
   }
   nBins = ((max_var - min_var + 1) / bin_width);
 }
 double binWidth = (max_var - min_var) / nBins;
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
      //int bin_value = (key.second-1) *binWidth + min_var;
      //double bin_value = (key.second-1) *binWidth + min_var;
      double bin_value = (key.second) *binWidth + min_var;
      TString Bin_string = TString::Format("%d",Bin);
     // Define condition categories dynamically
     std::vector<std::string> conditions = {"all"}; // Always include "all"
     for (const auto& cond : conditions) {
         createCumulativeHistogram(cumulativeHistograms[cond], Bin, var, cond, bin_value, binWidth, chamber_name, year, false);
         if(h->GetEntries()!=0)
         cumulativeHistograms[cond][Bin]->Add(h);
     }
 } // end of reading histograms into cumulative ones

     std::cout<<" Going to make cumulative ME13HV3 "<<var<<std::endl;
    // Read mean of the cumulative for each bin of instlumi
    for (const auto& [cond, histMap] : cumulativeHistograms) {
        for (const auto& [key, value] : histMap){
        int pressureBin = key;
        //std::cout<<" instlumi bin :"<<pressureBin<<std::endl;
        if(value==NULL || value->GetEntries()==0) continue;
        if(value->Integral()<100) continue;
        TH1D* cumulativeHistogram = value;
        TH1D* trimmed_cumulativeHistogram = trimmed_mean(cumulativeHistogram);
        // Calculate the mean of the cumulative histogram
        double mean = trimmed_cumulativeHistogram->GetMean();
        double mean_error = trimmed_cumulativeHistogram->GetMeanError();

        charge_ME13HV3[pressureBin] = mean;
//        TCanvas *c = new TCanvas();
//        c->cd();
//        cumulativeHistogram->Draw();
//        TString saving_name = "../cumulative_plots/"+area_name+"/all_channels/ME13HV3/"+var+TString::Format("/cumulative_charge_%d_%s_",pressureBin, var.Data())+"_instlumi_ME13HV3.pdf";
//        //c->SaveAs(saving_name);
//        TCanvas *c1 = new TCanvas();
//        c1->cd();
//        trimmed_cumulativeHistogram->Draw();
//        TString saving_name_trimmed = "../cumulative_plots/"+area_name+"/all_channels/ME13HV3/"+var+TString::Format("/cumulative_charge_%d_%s_",pressureBin, var.Data())+"_instlumi_ME13HV3_trimmed.pdf";
 //       c1->SaveAs(saving_name_trimmed);

        //delete cumulativeHistogram;
        //delete trimmed_cumulativeHistogram;
        //charge_ME13HV3_error[pressureBin] = mean_error; 
         }
     }     // Read all bins
     // Print return charge
 return charge_ME13HV3; 
}
// end of finding cumulative charge for ME13HV3
double GasDependence :: analysing_dependence(const std::map<std::pair<int, int>, std::tuple<TH1D*, double, int> > histograms,
   TString var, TFile * output_file, bool normalisation){
	// The idea is that for each rhid and each bin of pressure, instlumi, there is a histogram. Save this information in the histogram map corresponding to the rhid. Also later, I want to make a cumulativeHistogram depending on plus endcap , minus endcap, and CFEBs. So another map defined with this string and the corresponding histograms
    gStyle->SetTimeOffset(0.);
    std::map<std::string, std::map<int, TH1D*>> cumulativeHistograms;
    
     TString dir_name = var;
     TDirectoryFile *dir_var =  (TDirectoryFile*) output_file->mkdir(dir_name);
     dir_var->cd();
     double min_var, max_var;
     int nBins;
     if(var=="pressure" || var=="pressure_second") {
       min_var = 940; 
       max_var = 990;
       nBins = 50; 
    
     }
    else if(var=="instlumi" || var=="instlumi_second") {
       min_var = 0; 
       max_var = 2.5;
       nBins = 50; 
     }
    else if(var=="PV") {
       min_var = 0; 
       max_var = 100;
       nBins = 100; 
     }
    else if(var=="intlumi_initial" || var=="intlumi_final") {
       min_var = 0; 
       max_var = 160;
       nBins = 160; 
     }
    else if (var == "timesecond_initial" || var=="timesecond_final") {
      int bin_width = 86400;
      if(year=="2016") { 
        min_var  = 1462838400 ; // 10 May 2016 : 00 : 00 : 00
        max_var= 1477871999; // 30 Oct 2016 : 23 : 59 : 59
       }
      else if(year=="2017"){
        min_var  = 1497484800 ; // 15 June 2017 : 00 : 00 : 00 
        max_var= 1510790399; // 15 Nov 2017 : 23 : 59 : 59
      }
      else if(year=="2018"){
        min_var  = 1527206400 ; // 25 April 2018 : 00 : 00 : 00
        max_var =  1540511999; // 25 Oct 2018 : 23 : 59 : 59
      }
      nBins = ((max_var - min_var + 1) / bin_width);
    }
    double binWidth = (max_var - min_var) / nBins;
    // Individual trimmed mean TGraphs for individual channel 
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
            hist->GetYaxis()->SetTitle("Trimmed Mean Charge (ADC count)");
            if(var=="pressure" || var=="pressure_second")  hist->GetXaxis()->SetTitle("Pressure (hPa)");
            if(var=="instlumi" || var=="instlumi_second")  hist->GetXaxis()->SetTitle("Instlumi (*10^{34} cm^{2} s^{-1})");
            if(var=="PV")  hist->GetXaxis()->SetTitle("Number of reconstructed PV");
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
     }
     std::map<int, int> n_points;
     double x_var_value;
     double y_charge_value;
     double y_charge_err;
     std::map<TString, int> map_chamber = 
     { {"ME11a", 36},  {"ME11b", 36}, {"ME12HV1", 36}, {"ME12HV2", 36}, {"ME12HV3", 36},
       {"ME13HV1", 36}, {"ME13HV2", 36}, {"ME13HV3", 36},
       {"ME21HV1", 18}, {"ME21HV2", 18}, {"ME21HV3", 18},
       {"ME31HV1", 18}, {"ME31HV2", 18}, {"ME31HV3", 18},
       {"ME41HV1", 18}, {"ME41HV2", 18}, {"ME41HV3", 18},
       {"ME22HV1", 36}, {"ME22HV2", 36}, {"ME22HV3", 36},{"ME22HV4", 36},{"ME22HV5", 36},
       {"ME32HV1", 36}, {"ME32HV2", 36}, {"ME32HV3", 36},{"ME32HV4", 36},{"ME32HV5", 36},
       {"ME42HV1", 36}, {"ME42HV2", 36}, {"ME42HV3", 36},{"ME42HV4", 36},{"ME42HV5", 36},
     };
    int upper_nb = map_chamber[chamber_name] ;  
    int lower_nb = upper_nb/2;
    // Trim histograms and make cumulative distribution for each bin of the variable
     for (const auto& [key, value] : histograms){
         // key.first will be chamber name; key.second will be BinNb
         TH1D* h = std::get<0>(value);
         double var_value = std::get<1>(value);
         int entries = std::get<2>(value);
         int rhid = key.first;
         int Bin = key.second;
         //int bin_value = (key.second-1) *binWidth + min_var;
         //double bin_value = (key.second-1) *binWidth + min_var;
         double bin_value = (key.second) *binWidth + min_var;
         TString Bin_string = TString::Format("%d",Bin);
         // finding the endcap 
         int rhid_reduced = static_cast<int>(std::floor(rhid / 10)) % 1000;
          if (rhid > 2000000) {
           rhid_reduced += 400;
          }
          TString endcap = (rhid_reduced <= 400) ? "positive" : "negative";
       if (!h || h->GetEntries() <= 0) {
        continue;
        // histogram is empty
       }
 
         TH1D *h_trimmed = trimmed_mean(h);
         if(debug) std::cout << "Trimming histogram for RHID: " << key.first
         << ", "<<var<<" Bin: " << key.second << "bin value "<<bin_value<< " hist entries "<<h->Integral()<< " after trim "<<h_trimmed->Integral()<<" value  "<<var_value<<" entries "<<entries<<std::endl;
         // _dataset_pressure_corrected_trimmean_chamber35_layer4_Endcap2vs_pressure
         // Converting rhid into string //2143411
         std::string channel_name_string =  get_channel_name(rhid);
         int chamber_nb;
         int layer_nb;
         if (rhid_reduced <= 400){
          chamber_nb = static_cast<int>(std::floor(rhid_reduced / 10));
         } else {
          chamber_nb = static_cast<int>(std::floor((rhid_reduced - 400) / 10));
         }
         layer_nb = rhid_reduced % 10;
         TString channel_string(channel_name_string);
         
	 h_trimmed->SetName("dataset_trimmed_"+channel_string+"_bin_"+Bin_string+"_vs_"+var);
         if(h!=NULL || h->GetEntries()>0) {
           if(n_points.find(rhid)==n_points.end()){
             n_points[rhid]= 0;
          }
            // TO avoid filling zero values to graph
           if(h->GetEntries()<=0) { 
            continue; 
           }
           // To test if I use center of the bin as my gravity 
           // x_var_value = (Bin-1) * binWidth+ min_var+ 0.5 * binWidth;
           // double low_x_err = 0.5 * binWidth;
           // double up_x_err = 0.5 * binWidth;
           //  h_summary[rhid]->SetPoint(n_points[rhid], x_var_value, y_charge_value);
           //double low_x_err = x_var_value - bin_value ;
           //double up_x_err = bin_value+binWidth - x_var_value;

           x_var_value = var_value ;
           y_charge_value = h_trimmed->GetMean();
           y_charge_err = h_trimmed->GetMeanError();
           h_summary[rhid]->SetPoint(n_points[rhid], x_var_value, y_charge_value);
           h_summary[rhid]->SetPointError(n_points[rhid], 0,0 , y_charge_err, y_charge_err);
           n_points[rhid] = n_points[rhid]+1;
         } // filled when histogram is not emtpy
        // Define condition categories dynamically
        std::vector<std::string> conditions = {"all"}; // Always include "all"
        conditions.push_back("odd_strips");
//        if (endcap == "positive") conditions.push_back("plus");
//        if (endcap == "negative") conditions.push_back("minus");
//        if (chamber_nb % 2 == 0) conditions.push_back("even_chambers"); // Example: Add more conditions
//        if (chamber_nb % 2 != 0) conditions.push_back("odd_chambers"); // Example: Add more conditions
//        if (layer_nb % 2 != 0) conditions.push_back("odd_layers"); // Example: Add more conditions
//        if (layer_nb % 2 == 0) conditions.push_back("even_layers"); // Example: Add more conditions
//   
//        if(chamber_nb >=lower_nb+1 && chamber_nb <=upper_nb && endcap=="positive") conditions.push_back("upper_plus");  
//        if(chamber_nb >=lower_nb+1 && chamber_nb <=upper_nb && endcap=="negative") conditions.push_back("upper_minus");  
//        if(chamber_nb >=1 && chamber_nb <=lower_nb && endcap =="positive") conditions.push_back("lower_plus");  
//        if(chamber_nb >=1 && chamber_nb <=lower_nb && endcap =="negative") conditions.push_back("lower_minus");  
        for (const auto& cond : conditions) {
             // Normalisation is true for charge distribution which are normalised wrt ME13HV3, this is since our normalised charge distribution is in range (0,5) not (0,3000)
            if(normalisation==true)
            createCumulativeHistogram(cumulativeHistograms[cond], Bin, var, cond, bin_value, binWidth, chamber_name, year, true);
            else 
            createCumulativeHistogram(cumulativeHistograms[cond], Bin, var, cond, bin_value, binWidth, chamber_name, year, false);

	if (!h || h->GetEntries() == 0) continue;
            cumulativeHistograms[cond][Bin]->Add(h);
        }
       //  delete h;
       //  delete h_trimmed;
     }
    // end of making cumulative Histograms
    // Drawing individual histogram and write them to root file
    for(const auto &[rhid, hist] : h_summary){
       if(hist){
         TF1 *expFit2 = nullptr;
         if(var=="pressure" || var=="pressure_second" || var=="instlumi" || var=="instlumi_second" || var =="PV"){
          // Founding first and last point of the edges         
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
           
           std::cout << "Fit range: [" << fitlowedge << ", " << fithighedge << "]" << std::endl;
               if(var=="pressure" || var=="pressure_second"){
               expFit2 = new TF1("expFit2", "exp([0]) * exp([1]*(x-967))", fitlowedge, fithighedge);
               expFit2->SetParameters(5, -0.005); // Initial guesses
               } 
               if(var=="pressure_second"){
               expFit2 = new TF1("expFit2", "exp([0]) * exp([1]*(x-967))", fitlowedge, fithighedge);
               } 
               else if(var=="PV"){
                expFit2 = new TF1("expFit2", "exp([0]) * exp([1]*(x-50))", fitlowedge, fithighedge);
               } 

               else if(var=="instlumi" || var=="instlumi_second"){
               expFit2 = new TF1("expFit2", "exp([0]) * exp([1]*(x-1))", fitlowedge, fithighedge);
               }
               hist->Fit(expFit2, "R");
          }
         delete expFit2;
         hist->Write();
       }
     }
    std::map<std::string, double> slope_values;
    for (const auto& [cond, histMap] : cumulativeHistograms) {
    slope_values[cond] = this->Draw_cumulative_summary_histogram(histograms, histMap, cond, var, binWidth);
    }
    double slope_value = slope_values["all"];
    return slope_value;
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
double GasDependence :: Draw_cumulative_summary_histogram(const std::map<std::pair<int, int>, std::tuple<TH1D*, double, int>> histograms , std::map<int, TH1D*> cumulativeHistograms, TString type_cumulative, TString var, double binWidth) {
  double slope_value = 0;
    TGraphAsymmErrors* summaryHistogram = new TGraphAsymmErrors();
    TString summary_title;
    if(type_cumulative=="all") { 
     summary_title = "Cumulative : "+chamber_name+" : "+year+ " : "+var;
    }
    else if(type_cumulative=="plus"){
     summary_title = "Cumulative : "+chamber_name+" : "+year+ " : "+var+" : +endcap";
    }
    else if(type_cumulative=="minus"){
     summary_title = "Cumulative : "+chamber_name+" : "+year+ " : "+var+" : -endcap";
    }
    else if(type_cumulative=="odd_chambers"){
     summary_title = "Cumulative : "+chamber_name+" : "+year+ " : "+var+" : odd chambers";
    }
    else if(type_cumulative=="even_chambers"){
     summary_title = "Cumulative : "+chamber_name+" : "+year+ " : "+var+" : even chambers";
    }
    else if(type_cumulative=="lower_plus"){
     summary_title = "Cumulative : "+chamber_name+" : "+year+ " : "+var+" : lower +endcap";
    }
    else if(type_cumulative=="lower_minus"){
     summary_title = "Cumulative : "+chamber_name+" : "+year+ " : "+var+" : lower -endcap";
    }
    else if(type_cumulative=="upper_plus"){
     summary_title = "Cumulative : "+chamber_name+" : "+year+ " : "+var+" : upper +endcap";
    }
    else if(type_cumulative=="upper_minus"){
     summary_title = "Cumulative : "+chamber_name+" : "+year+ " : "+var+" : upper -endcap";
    }
    else if(type_cumulative=="odd_layers"){
     summary_title = "Cumulative : "+chamber_name+" : "+year+ " : "+var+" : odd layers";
    }
    else if(type_cumulative=="even_layers"){
     summary_title = "Cumulative : "+chamber_name+" : "+year+ " : "+var+" : even layers";
    }
    else if(type_cumulative=="odd_lumiBlock"){
     summary_title = "Cumulative : "+chamber_name+" : "+year+ " : "+var+" : odd lumi sections";
    }
    else if(type_cumulative=="even_lumiBlock"){
     summary_title = "Cumulative : "+chamber_name+" : "+year+ " : "+var+" : even lumi sections";
    }
    else if(type_cumulative=="odd_strips"){
     summary_title = "Cumulative : "+chamber_name+" : "+year+ " : "+var+" : odd numbered strip";
    }
    else if(type_cumulative=="even_strips"){
     summary_title = "Cumulative : "+chamber_name+" : "+year+ " : "+var+" : even numbered strip";
    }

   summaryHistogram->SetTitle(summary_title);
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
        if(cumulativeHistogram->Integral() <100) continue;
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
     if(var=="instlumi" || var=="instlumi_second")
     summaryHistogram->GetYaxis()->SetRangeUser(0,1.5);
     if(var=="PV")
     summaryHistogram->GetYaxis()->SetRangeUser(0,1.5);

     summaryHistogram->GetYaxis()->SetTitle("Trimmed Mean Charge (ADC count)");
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

     TCanvas *c = new TCanvas();
     gStyle->SetOptStat(111112211);
     gStyle->SetOptFit(1111);
    
     if(debug) std::cout<<" before plotting and fitting summary"<<std::endl; 
    
     if(var=="pressure" || var=="pressure_second" || var=="instlumi" || var=="instlumi_second" || var=="PV"){
      double fitlowedge = 0, fithighedge = 0; // Initialize edges
      int nPoints = summaryHistogram->GetN(); // Get the number of points in the graph
      bool foundFirst = false;
      
      // Loop through all points to find the first and last valid points
      for (int i = 0; i < nPoints; i++) {
          double x, y;
          summaryHistogram->GetPoint(i, x, y);
      
          if (y > 0) { // Replace with your condition (e.g., y > threshold)
              if (!foundFirst) {
                  fitlowedge = x-binWidth; // First valid x-coordinate
                  foundFirst = true;
              }
              fithighedge = x+binWidth; // Continuously update with the last valid x-coordinate
          }
      }
     if(debug) std::cout << "Fit range: [" << fitlowedge << ", " << fithighedge << "]" << std::endl;
     TF1 *expFit = nullptr;
     if(var=="pressure"){
     expFit = new TF1("expFit", "exp([0]) * exp([1]*(x-967))", fitlowedge, fithighedge);
     expFit->SetParameters(5, -0.005); // Initial guesses
     summaryHistogram->Fit(expFit, "R");
     slope_value = expFit->GetParameter(1);
     } 
     else if(var=="pressure_second"){
     expFit = new TF1("expFit", "exp([0]) * exp([1]*(x-967))", fitlowedge, fithighedge);
     summaryHistogram->Fit(expFit, "R");
     slope_value = expFit->GetParameter(1);
     } 
     else if(var=="instlumi" || var=="instlumi_second"){
     expFit = new TF1("expFit", "exp([0]) * exp([1]*(x-1))", fitlowedge, fithighedge);
     summaryHistogram->Fit(expFit, "R");
     slope_value = expFit->GetParameter(1);
     }
     else if(var=="PV"){
     expFit = new TF1("expFit", "exp([0]) * exp([1]*(x-50))", fitlowedge, fithighedge);
     summaryHistogram->Fit(expFit, "R");
     slope_value = expFit->GetParameter(1);
     }

    }
     c->cd();
     summaryHistogram->Draw("AP");
     gPad->Update();
     c->Update();
     c->Update();
 
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
  }
 double thecorr =exp(slope*(refvalue-X));
 return thecorr;
  }
