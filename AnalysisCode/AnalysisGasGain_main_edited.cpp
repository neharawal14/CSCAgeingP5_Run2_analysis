#include <cmath>
#include <filesystem>
#include <map>
#include <memory>
#include <stdexcept>
#include <limits>
#include <utility>
#include <TFitResultPtr.h>
#include <TDirectory.h>
#include <TString.h>
#include <TVirtualPad.h>
#include <vector>
#include <string>
#include <TFile.h>
#include <TChain.h>
#include <TH1D.h>
#include "TStyle.h"
#include <TCanvas.h>
#include <TF1.h>
#include <iostream>
#include <tuple>
#include <algorithm>
#include "TGraphAsymmErrors.h"
#include "badChannel.h"
namespace fs = std::filesystem;
using HistogramMap = std::map<std::pair<int, int>,
    std::tuple<std::unique_ptr<TH1D>, double, Long64_t>>;
using CumulativeMap = std::map<int, std::unique_ptr<TH1D>>;

// Branch buffers must outlive all reads, and be disconnected on every exit path.
struct BranchAddressReset {
    TChain& chain;
    ~BranchAddressReset() { chain.ResetBranchAddresses(); }
};

template<class T>
void BindBranch(TChain* chain, const char* name, T* address) {
    if (chain->SetBranchAddress(name, address) < 0)
        throw std::runtime_error(std::string("Missing or incompatible branch: ") + name);
}

struct Binning {
    double min, max;
    int bins;
    double width() const { return (max - min) / bins; }
};

Binning GetBinning(const TString& variable, const TString& year) {
    if (variable == "pressure" || variable == "pressure_second") return {940, 990, 50};
    if (variable == "instlumi" || variable == "instlumi_second") return {0, 2.5, 50};
    if (variable == "PV") return {0, 100, 100};
    if (variable == "intlumi" || variable == "intlumi_initial" || variable == "intlumi_final")
        return {0, 160, 160};
    if (variable == "time" || variable == "timesecond_initial" || variable == "timesecond_final") {
        double low = 0, high = 0;
        if (year == "2016") { low = 1462838400; high = 1477871999; }
        else if (year == "2017") { low = 1497484800; high = 1510790399; }
        else if (year == "2018") { low = 1527206400; high = 1540511999; }
        else if (year == "run2") { low = 1462838400; high = 1540511999; }
        else throw std::invalid_argument("Unsupported year for time binning");
        return {low, high, static_cast<int>((high - low + 1) / 86400)};
    }
    throw std::invalid_argument(std::string("Unsupported variable: ") + variable.Data());
}

struct RunRegion{
    double RunMin;
    double RunMax;
    double medianHV;
};


std::vector<RunRegion> GetRunRegions(TString year, TString) ;
std::map<int, std::map<TString, std::vector<RunRegion>>> BuildMedianMap(
    TChain *tree,
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

    const auto n = values.size();

    if (n % 2 == 1) {
        return values[n / 2];
    } else {
        return 0.5 * (values[n / 2 - 1] + values[n / 2]);
    }
}
std::map<int, std::map<TString, std::vector<RunRegion>>> BuildMedianMap(
    TChain *tree, 
    TString year, TString chamber
)
{
    std::map<int, std::map<TString, std::vector<std::vector<double>>>> hvValuesMap;
    if (!tree) throw std::invalid_argument("Null median-map chain");
    BranchAddressReset reset{*tree};
    Long64_t _rhid_local{};
    double _HV_local = 0;
    Long64_t _runNb_local = 0;
    BindBranch(tree, "_rhid", &_rhid_local);
    BindBranch(tree, "_HV", &_HV_local);
    BindBranch(tree, "_runNb", &_runNb_local);
    const auto regions = GetRunRegions(year, chamber);
    const Long64_t entries = tree->GetEntries();
    for (Long64_t i = 0; i < entries; i++) {
        if (tree->GetEntry(i) <= 0) throw std::runtime_error("Failed to read tree entry");
        if (_rhid_local < 0 || _rhid_local > std::numeric_limits<int>::max())
            throw std::runtime_error("RHID outside supported integer range");

        for (int r = 0; r < (int)regions.size(); r++) {
            if (_runNb_local >= regions[r].RunMin &&
                _runNb_local <  regions[r].RunMax)
            {
                if (!std::isfinite(_HV_local)) break;
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

ChannelInfo DecodeRhid(int rhid) {

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
    Long64_t runNb,
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


int ChamberCount(const TString& chamber) {
    static const std::map<TString, int> map_chamber = 
     { {"ME11a", 36},  {"ME11b", 36}, {"ME12HV1", 36}, {"ME12HV2", 36}, {"ME12HV3", 36},
       {"ME13HV1", 36}, {"ME13HV2", 36}, {"ME13HV3", 36},
       {"ME21HV1", 18}, {"ME21HV2", 18}, {"ME21HV3", 18},
       {"ME31HV1", 18}, {"ME31HV2", 18}, {"ME31HV3", 18},
       {"ME41HV1", 18}, {"ME41HV2", 18}, {"ME41HV3", 18},
       {"ME22HV1", 36}, {"ME22HV2", 36}, {"ME22HV3", 36},{"ME22HV4", 36},{"ME22HV5", 36},
       {"ME32HV1", 36}, {"ME32HV2", 36}, {"ME32HV3", 36},{"ME32HV4", 36},{"ME32HV5", 36},
       {"ME42HV1", 36}, {"ME42HV2", 36}, {"ME42HV3", 36},{"ME42HV4", 36},{"ME42HV5", 36},
     };
    const auto found = map_chamber.find(chamber);
    if (found == map_chamber.end())
        throw std::invalid_argument("Unsupported chamber: " + std::string(chamber.Data()));
    return found->second;
}

std::vector<std::string> ChannelCategories(int rhid, const TString& chamber) {
    const auto info = DecodeRhid(rhid);
    const int count = ChamberCount(chamber);
    // Retain historical category mapping even for the decoder's sentinel IDs.
    const bool positive = info.rhid_reduced <= 400;
    const int chamberNumber = (positive ? info.rhid_reduced : info.rhid_reduced - 400) / 10;
    const int layerNumber = info.rhid_reduced % 10;
    std::vector<std::string> categories = {
        "all", positive ? "plus" : "minus",
        chamberNumber % 2 == 0 ? "even_chambers" : "odd_chambers",
        layerNumber % 2 == 0 ? "even_layers" : "odd_layers"
    };
    if (chamberNumber >= 1 && chamberNumber <= count / 2)
        categories.push_back(positive ? "lower_plus" : "lower_minus");
    else if (chamberNumber > count / 2 && chamberNumber <= count)
        categories.push_back(positive ? "upper_plus" : "upper_minus");
    return categories;
}

std::map<int, double> MeanBinCoordinates(const HistogramMap& histograms,
                                        const TString& category, const TString& chamber) {
    std::map<int, std::pair<double, Long64_t>> totals;
    for (const auto& [key, value] : histograms) {
        const auto categories = ChannelCategories(key.first, chamber);
        if (std::find(categories.begin(), categories.end(), category.Data()) == categories.end()) continue;
        const auto& histogram = std::get<0>(value);
        const auto entries = std::get<2>(value);
        if (!histogram || histogram->GetEntries() <= 0 || entries <= 0) continue;
        auto& total = totals[key.second];
        total.first += std::get<1>(value) * entries;
        total.second += entries;
    }
    std::map<int, double> means;
    for (const auto& [bin, total] : totals) means[bin] = total.first / total.second;
    return means;
}

void ConfigureGraphAxes(TGraphAsymmErrors& graph, const TString& variable) {
    graph.GetYaxis()->SetTitle("Trimmed Mean Charge (ADC count)");
    if (variable == "pressure" || variable == "pressure_second")
        graph.GetXaxis()->SetTitle("Pressure (hPa)");
    else if (variable == "instlumi" || variable == "instlumi_second")
        graph.GetXaxis()->SetTitle("Instlumi (*10^{34} cm^{-2} s^{-1})");
    else if (variable == "PV") graph.GetXaxis()->SetTitle("Number of reconstructed PV");
    else if (variable == "intlumi_initial" || variable == "intlumi_final")
        graph.GetXaxis()->SetTitle("Integrated lumi (fb^{-1})");
    else if (variable == "timesecond_initial" || variable == "timesecond_final") {
        graph.GetXaxis()->SetTimeDisplay(1);
        graph.GetXaxis()->SetLabelSize(0.02);
        graph.GetXaxis()->SetTimeFormat("%Y/%m/%d");
        graph.GetXaxis()->SetTitle("time");
    }
}

double FitDependence(TGraphAsymmErrors& graph, const TString& variable,
                     double binWidth, bool required, const char* fitName = "expFit2") {
    const char* formula = nullptr;
    if (variable == "pressure" || variable == "pressure_second")
        formula = "exp([0]) * exp([1]*(x-967))";
    else if (variable == "instlumi" || variable == "instlumi_second")
        formula = "exp([0]) * exp([1]*(x-1))";
    else if (variable == "PV") formula = "exp([0]) * exp([1]*(x-50))";
    else return 0;
    double low = std::numeric_limits<double>::infinity(), high = -low;
    int positivePoints = 0;
    for (int i = 0; i < graph.GetN(); ++i) {
        double x, y;
        graph.GetPoint(i, x, y);
        if (std::isfinite(x) && std::isfinite(y) && y > 0) {
            low = std::min(low, x - binWidth);
            high = std::max(high, x + binWidth);
            ++positivePoints;
        }
    }
    auto failed = [&]() {
        const std::string message = "Insufficient data or failed fit: " + std::string(variable.Data()) +
            " (" + graph.GetTitle() + ")";
        if (required) throw std::runtime_error(message);
        std::cerr << "Warning: " << message << '\n';
        return std::numeric_limits<double>::quiet_NaN();
    };
    if (positivePoints < 2 || !(low < high)) return failed();
    TF1 fit(fitName, formula, low, high);
    if (variable == "pressure") fit.SetParameters(5, -0.005);
    // ROOT owns the fitted copy stored on the graph, not this local original.
    const int status = graph.Fit(&fit, "R");
    if (status != 0 || !std::isfinite(fit.GetParameter(1))) return failed();
    return fit.GetParameter(1);
}

class GasDependence {

 public : 
    // variables
    TString area_name;
    TString year;
    TString chamber_name;
    bool debug = false;

    double _rhsumQ_equalised_HV_data = 0;
    double _pressure = 0, _instlumi_dcsjson = 0, _intlumi_delivered_dcsjson = 0, _HV = 0;
    Int_t _n_PV = 0;
    Long64_t _rhid = 0, _runNb = 0, _timesecond = 0, _nearestStrip = 0;
    TChain *tree = nullptr; // Borrowed; the caller owns the chain.
    GasDependence() = default;
    GasDependence(const GasDependence&) = delete;
    GasDependence& operator=(const GasDependence&) = delete;
    ~GasDependence() { if (tree) tree->ResetBranchAddresses(); }
    std::map<int, std::map<TString, std::vector<RunRegion>>> hvMedianMap;


    std::unique_ptr<TH1D> TrimChargeHistogram(const TH1D* h);
    void Initialize(TString chamber_name_string , TString year_value ,TString area_name_string, TChain*, std::map<int, std::map<TString, std::vector<RunRegion>>> ); 
    double AnalyzeDependence(const HistogramMap& histograms, TString var, TFile *, bool);
    HistogramMap BuildChargeHistograms(TString variable, bool, double, double, bool, const std::vector<double>&) ;
    std::string ChannelName(int rhid);
    double VariableValue(const TString& variable) const;
    double ApplyCorrection(double X ,TString correctiontype, double slope );
    double WriteCumulativeSummary(const HistogramMap& histograms ,const CumulativeMap& cumulativeHistogram, TString type, TString var, double binWidth);
    std::vector<double> BuildReferenceCharges(const HistogramMap& histograms, TString var);
    void EnsureCumulativeHistogram(CumulativeMap& histMap, 
                               int bin, 
                               const TString var, 
                               const TString condition, 
                               double bin_value, 
                               double binWidth, 
                               const TString chamber_name, 
                               const TString year, 
                               bool ) ;

};

void GasDependence :: Initialize(TString chamber_name_string , TString year_value ,TString area_name_string, TChain *chain_here, std::map<int, std::map<TString, std::vector<RunRegion>>> HV_Map){

  area_name = area_name_string;
  chamber_name = chamber_name_string;
  year = year_value;
  if (!chain_here) throw std::invalid_argument("Null input chain");
  if (tree) tree->ResetBranchAddresses();
  tree = chain_here;
  hvMedianMap = std::move(HV_Map);
  BindBranch(tree, "_rhsumQ_equalised_HV_data", &_rhsumQ_equalised_HV_data);
  BindBranch(tree, "_n_PV", &_n_PV);
  BindBranch(tree, "_pressure", &_pressure);
  BindBranch(tree, "_intlumi_delivered_dcsjson", &_intlumi_delivered_dcsjson);
  BindBranch(tree, "_timesecond", &_timesecond);
  BindBranch(tree, "_nearestStrip", &_nearestStrip);
  BindBranch(tree, "_instlumi_dcsjson", &_instlumi_dcsjson);
  BindBranch(tree, "_rhid", &_rhid);
  BindBranch(tree, "_HV", &_HV);
  BindBranch(tree, "_runNb", &_runNb);
}


std::string GasDependence::ChannelName(int rhid) {
    return DecodeRhid(rhid).channel_name;
}

double GasDependence::VariableValue(const TString& variable) const {
    if (variable == "pressure") return _pressure;
    if (variable == "instlumi") return _instlumi_dcsjson / 10000.0;
    if (variable == "PV") return _n_PV;
    if (variable == "intlumi") return _intlumi_delivered_dcsjson;
    if (variable == "time") return _timesecond;
    throw std::invalid_argument("Unsupported tree variable: " + std::string(variable.Data()));
}

HistogramMap GasDependence :: BuildChargeHistograms(TString variable, 
bool correction, double slope_pressure, double slope_instlumi, bool normalisation , const std::vector<double>& ME13HV3_charge) {
	
     gStyle->SetTimeOffset(0.);
     // Map to store histograms and mean values
    HistogramMap result;
    // Map to track cumulative sums and counts for calculating mean values
    std::map<std::pair<int, int>, std::pair<double, Long64_t>> binStats;

    const auto binning = GetBinning(variable, year);
    const double min_var = binning.min, max_var = binning.max;
    const int nBins = binning.bins;
    const double binWidth = binning.width();
    TString year_value;
    const Long64_t nEntries = tree->GetEntries();

    std::cout << "Reading " << chamber_name << " versus " << variable << std::endl;
     double tolerance = 5.0;

    for (Long64_t i = 0; i < nEntries; i++) {
        if (tree->GetEntry(i) <= 0) throw std::runtime_error("Failed to read tree entry");

        if (_rhid < 0 || _rhid > std::numeric_limits<int>::max())
            throw std::runtime_error("RHID outside supported integer range");
        // removing the recuperated and wrong gas composition period 
         double medianHV = GetMedianHV(
             _rhid,
             year,
             _runNb,
             hvMedianMap
         );
     if (!std::isfinite(medianHV) || !std::isfinite(_HV) ||
         !std::isfinite(_pressure) || !std::isfinite(_instlumi_dcsjson) ||
         !std::isfinite(_rhsumQ_equalised_HV_data)) continue;
     if (std::abs(_HV - medianHV) > tolerance) {
        // Not accept point
        continue; 
    }
    if(year=="run2") {
         if(_timesecond >=1451606400  && _timesecond <= 1483142400) year_value = "2016";
         else if(_timesecond >=1483228800 && _timesecond <=1514678400 ) year_value = "2017";
         else if(_timesecond >=1514764800 && _timesecond <=1546214400 ) year_value = "2018";
         else continue; // Do not reuse the previous event's year in date gaps.
    }
   else year_value = year;
        bool badchannel = isbadchannel(chamber_name , _rhid , _nearestStrip,  year_value);
        if(badchannel) continue;
        const double coordinate = VariableValue(variable);
        if (!std::isfinite(coordinate) || coordinate < min_var || coordinate >= max_var) continue;
        const int Bin = static_cast<int>((coordinate - min_var) / binWidth);
        if (Bin < 0 || Bin >= nBins) continue;

        const auto reducedKey = std::make_pair(static_cast<int>(_rhid), Bin);
        double equalised_charge =0;

	if(this->chamber_name=="ME13HV3")
        equalised_charge = _rhsumQ_equalised_HV_data * ApplyCorrection(_pressure ,"pressure", slope_pressure) ;
	else 
        equalised_charge = _rhsumQ_equalised_HV_data * ApplyCorrection(_pressure ,"pressure", slope_pressure) * 
        ApplyCorrection(_instlumi_dcsjson/10000.0, "instlumi", slope_instlumi);

        // Normalise charges for deriving instlumi dependence with charges in ME13HV3
        if(normalisation==true && (variable=="instlumi" || variable =="PV" || variable=="instlumi_second")) {

          if (static_cast<std::size_t>(Bin) >= ME13HV3_charge.size()) continue;
          if (!std::isfinite(ME13HV3_charge[Bin]) || ME13HV3_charge[Bin] <= 0) continue;
          equalised_charge /= ME13HV3_charge[Bin];
        }

        if (!std::isfinite(equalised_charge)) continue;
        if (result.find(reducedKey) == result.end()) {
            const TString name = Form("h_rhid_%lld_%s%s_%d", _rhid,
                                      variable.Data(), correction ? "_second" : "", Bin);
            const TString title = Form("Histogram for RHID %lld, %s Bin %d%s", _rhid,
                variable.Data(), Bin, correction ? " : after correction" : "");
            auto histogram = std::make_unique<TH1D>(name, title,
                normalisation ? 50 : 3000, 0, normalisation ? 5 : 3000);
            histogram->SetDirectory(nullptr);
            result.emplace(reducedKey, std::make_tuple(std::move(histogram), 0.0, 0));
        }
        // Fill the histogram with charge data
        std::get<0>(result[reducedKey])->Fill(equalised_charge);

        binStats[reducedKey].first += coordinate;
        ++binStats[reducedKey].second;

    } // end of going through each tree entry
    // Calculate the mean pressure for each bin and store it in the result map
    for (auto& [key, stats] : binStats) {
        double sumPressure = stats.first;
        Long64_t count = stats.second;
        double meanPressure = (count > 0) ? (sumPressure / count) : 0.0;

         // Update the mean value in the result map
         std::get<1>(result[key]) = meanPressure;
         std::get<2>(result[key]) = count;
         TH1D* h1 = std::get<0>(result[key]).get();
         double nentries = h1->Integral();
         double mean = h1->GetMean();
         if(debug) std::cout<<" key "<<key.first<<" bin "<<key.second<< " mean "<<meanPressure<<" histogram value "<<nentries<<" count "<<count<<" mean value "<<mean<<std::endl;
     } // end of reading mean value for each bin

    return result;
    // This result is basically  for each rhid and bin - we have a histogram, the average value, and the total count 
    // Hence the format  HistogramMap 
    //  Map of [rhid, bin_nb] ->  [TH1D*, mean value, count]
}

// Trimming histogram and providing trimmed histogram
std::unique_ptr<TH1D> GasDependence::TrimChargeHistogram(const TH1D* h) {
   if (!h) throw std::invalid_argument("Cannot trim a null histogram");
   const TH1D* h_trim = h;
   auto h_trim_new = std::unique_ptr<TH1D>(static_cast<TH1D*>(h->Clone()));
   h_trim_new->SetDirectory(nullptr);
 
   // Retain the original 85% prescription, including overflow in the target
   // and integer truncation of the boundary bin. See the review before changing it.
   const float trimFraction = 0.85;
   const float total = h_trim->Integral() + h_trim->GetBinContent(h_trim->GetNbinsX() + 1);
   h_trim_new->Reset();
   h_trim_new->ResetStats();
   float integral = 0;
   int lastBin = 0;
   for (int bin = 1; bin <= h_trim->GetNbinsX(); ++bin) {
       if (integral < trimFraction * total) {
           integral += h_trim->GetBinContent(bin);
           lastBin = bin;
       }
   }
   double precedingIntegral = 0;
   for (int bin = 1; bin < lastBin; ++bin) {
       precedingIntegral += h_trim->GetBinContent(bin);
       h_trim_new->SetBinContent(bin, h_trim->GetBinContent(bin));
       h_trim_new->SetBinError(bin, h_trim->GetBinError(bin));
   }
   const int boundaryEntries = static_cast<int>(trimFraction * total - precedingIntegral);
   h_trim_new->SetBinContent(lastBin, boundaryEntries);
   if (boundaryEntries != 0 && h_trim->GetBinContent(lastBin) > 0)
       h_trim_new->SetBinError(lastBin, h_trim->GetBinError(lastBin) *
           (boundaryEntries / h_trim->GetBinContent(lastBin)));
   return h_trim_new;
}


struct Configuration {
    TString chamber, year, area;
    fs::path outputRoot = "../Results_Root_files";
    fs::path inputRoot = "/eos/home-n/nrawal/CSCAgeing/Run2_Ntuples";

    fs::path outputFile(bool reference = false) const {
        return outputRoot / area.Data() /
            (std::string("dataset_output_") + chamber.Data() + "_" + year.Data() +
             (reference ? "_ME13HV3.root" : ".root"));
    }
};

Configuration ParseArguments(int argc, char* argv[]) {
    if (argc < 4 || argc > 6)
        throw std::invalid_argument(
            "Usage: AnalysisGasGain CHAMBER YEAR AREA [OUTPUT_ROOT] [INPUT_ROOT]");
    Configuration config;
    config.chamber = argv[1]; config.year = argv[2]; config.area = argv[3];
    if (config.year != "2016" && config.year != "2017" &&
        config.year != "2018" && config.year != "run2")
        throw std::invalid_argument("YEAR must be 2016, 2017, 2018, or run2");
    for (const auto& component : {config.chamber, config.area}) {
        const std::string text = component.Data();
        if (text.empty() || text == "." || text == ".." ||
            text.find_first_of("/\\") != std::string::npos)
            throw std::invalid_argument("CHAMBER and AREA must be single path components");
    }
    ChamberCount(config.chamber);
    if (argc >= 5) config.outputRoot = argv[4];
    if (argc >= 6) config.inputRoot = argv[5];
    if (config.outputRoot.empty() || config.inputRoot.empty())
        throw std::invalid_argument("Input and output roots must not be empty");
    return config;
}

void LoadInputs(TChain& chain, const Configuration& config, const TString& chamber) {
    for (const std::string year : {"2016", "2017", "2018"}) {
        if (config.year != "run2" && config.year != year.c_str()) continue;
        const auto path = config.inputRoot / (year + "_updated_new") /
            ("csc_output_" + year + "_" + chamber.Data() + "_tree_updated.root");
        // nentries=0 forces ROOT to check each file and tree immediately.
        if (chain.Add(path.string().c_str(), 0) != 1)
            throw std::runtime_error("Cannot load tree from " + path.string());
    }
    if (chain.GetEntries() <= 0 || chain.LoadTree(0) < 0)
        throw std::runtime_error("Input chain is empty or unreadable");
}

std::unique_ptr<TFile> OpenOutput(const fs::path& path) {
    fs::create_directories(path.parent_path());
    auto file = std::unique_ptr<TFile>(TFile::Open(path.string().c_str(), "RECREATE"));
    if (!file || file->IsZombie() || !file->IsWritable())
        throw std::runtime_error("Cannot create output file: " + path.string());
    return file;
}

double RunStage(GasDependence& analysis, TFile& output, const TString& variable,
                const TString& label, bool corrected, double pressureSlope = 0,
                double lumiSlope = 0, bool normalized = false,
                const std::vector<double>& reference = {}) {
    auto histograms = analysis.BuildChargeHistograms(variable, corrected,
        pressureSlope, lumiSlope, normalized, reference);
    return analysis.AnalyzeDependence(histograms, label, &output, normalized);
}

void RunAnalysis(const Configuration& config) {
    // Prevent transient histogram registration; all histograms have C++ owners.
    TH1::AddDirectory(false);
    TChain targetChain("tree"), referenceChain("tree");
    LoadInputs(targetChain, config, config.chamber);
    LoadInputs(referenceChain, config, "ME13HV3");
    const auto targetHV = BuildMedianMap(&targetChain, config.year, config.chamber);
    const auto referenceHV = BuildMedianMap(&referenceChain, config.year, "ME13HV3");
    auto output = OpenOutput(config.outputFile());
    auto referenceOutput = OpenOutput(config.outputFile(true));
    GasDependence target, reference;
    target.Initialize(config.chamber, config.year, config.area, &targetChain, targetHV);
    reference.Initialize("ME13HV3", config.year, config.area, &referenceChain, referenceHV);

    RunStage(target, *output, "intlumi", "intlumi_initial", false);
    RunStage(target, *output, "time", "timesecond_initial", false);
    const double pressureSlope = RunStage(target, *output, "pressure", "pressure", false);
    RunStage(target, *output, "pressure", "pressure_second", true, pressureSlope);
    const double referencePressureSlope =
        RunStage(reference, *referenceOutput, "pressure", "pressure", false);
    std::vector<double> referenceCharge;
    {
        auto histograms = reference.BuildChargeHistograms("instlumi", false,
            referencePressureSlope, 0, false, {});
        referenceCharge = reference.BuildReferenceCharges(histograms, "instlumi");
    }
    const double lumiSlope = RunStage(target, *output, "instlumi", "instlumi", false,
        pressureSlope, 0, true, referenceCharge);
    RunStage(target, *output, "intlumi", "intlumi_final", true, pressureSlope, lumiSlope);
    RunStage(target, *output, "time", "timesecond_final", true, pressureSlope, lumiSlope);
    output->Close();
    referenceOutput->Close();
    if (output->TestBit(TFile::kWriteError) || referenceOutput->TestBit(TFile::kWriteError))
        throw std::runtime_error("ROOT reported an output write error");
}

int main(int argc, char* argv[]) {
    try {
        RunAnalysis(ParseArguments(argc, argv));
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}

// Program to find ME13HV3 charges for each instlumi bins
std::vector<double> GasDependence::BuildReferenceCharges(
    const HistogramMap& histograms, TString var) {
    if (var != "instlumi" && var != "PV")
        throw std::invalid_argument("Reference charge supports instlumi and PV only");
    const auto binning = GetBinning(var, year);
    std::vector<double> charges(binning.bins, 0.0);
    CumulativeMap cumulative;
    for (const auto& [key, value] : histograms) {
        const auto& histogram = std::get<0>(value);
        if (!histogram || histogram->GetEntries() == 0) continue;
        EnsureCumulativeHistogram(cumulative, key.second, var, "all",
            binning.min + key.second * binning.width(), binning.width(), chamber_name, year, false);
        cumulative.at(key.second)->Add(histogram.get());
    }
    for (const auto& [bin, histogram] : cumulative) {
        if (histogram->Integral() < 100) continue;
        const auto trimmed = TrimChargeHistogram(histogram.get());
        charges.at(bin) = trimmed->GetMean();
    }
    return charges;
}
// end of finding cumulative charge for ME13HV3
double GasDependence::AnalyzeDependence(const HistogramMap& histograms,
    TString var, TFile* output_file, bool normalisation) {
    gStyle->SetTimeOffset(0.);
    gStyle->SetOptStat(111112211);
    gStyle->SetOptFit(1111);
    if (!output_file || output_file->IsZombie()) throw std::runtime_error("Invalid output file");
    auto* directory = output_file->mkdir(var);
    if (!directory) throw std::runtime_error("Cannot create output directory: " + std::string(var.Data()));
    TDirectory::TContext directoryContext(directory);
    const auto binning = GetBinning(var, year);
    std::map<int, std::unique_ptr<TGraphAsymmErrors>> graphs;
    std::map<std::string, CumulativeMap> cumulative;

    for (const auto& [key, value] : histograms) {
        const auto& histogram = std::get<0>(value);
        if (!histogram || histogram->GetEntries() <= 0) continue;
        const auto [rhid, bin] = key;
        auto& graph = graphs[rhid];
        if (!graph) {
            graph = std::make_unique<TGraphAsymmErrors>();
            const TString channel = ChannelName(rhid);
            graph->SetName("dataset_trimmed_" + channel + "_" + var);
            graph->SetTitle(chamber_name + " : " + year + " : " + channel + " : " + var);
            ConfigureGraphAxes(*graph, var);
        }
        const auto trimmed = TrimChargeHistogram(histogram.get());
        const int point = graph->GetN();
        graph->SetPoint(point, std::get<1>(value), trimmed->GetMean());
        graph->SetPointError(point, 0, 0, trimmed->GetMeanError(), trimmed->GetMeanError());
        for (const auto& category : ChannelCategories(rhid, chamber_name)) {
            EnsureCumulativeHistogram(cumulative[category], bin, var, category.c_str(),
                binning.min + bin * binning.width(), binning.width(), chamber_name, year, normalisation);
            cumulative[category].at(bin)->Add(histogram.get());
        }
    }
    for (const auto& [rhid, graph] : graphs) {
        FitDependence(*graph, var, binning.width(), false);
        if (graph->Write() <= 0) throw std::runtime_error("Failed to write channel graph");
    }
    if (cumulative.empty()) throw std::runtime_error("No accepted events for " + std::string(var.Data()));
    double slope = 0;
    for (const auto& [category, histogramsByBin] : cumulative) {
        const double fitted = WriteCumulativeSummary(histograms, histogramsByBin,
            category.c_str(), var, binning.width());
        if (category == "all") slope = fitted;
    }
    return slope;
}
// End of AnalyzeDependence function
void GasDependence ::EnsureCumulativeHistogram(CumulativeMap& histMap, 
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
        histMap[bin] = std::make_unique<TH1D>(histName, histTitle, 50, 0, 5); // Example: adjust binning as needed
        else 
        histMap[bin] = std::make_unique<TH1D>(histName, histTitle, 3000, 0, 3000); // Example: adjust binning as needed
        histMap[bin]->SetDirectory(nullptr);
        histMap[bin]->Sumw2();
        histMap[bin]->GetXaxis()->SetTitle("charge (ADC)");
        histMap[bin]->GetYaxis()->SetTitle("Nb. of entries");
        histMap[bin]->SetTitle(histTitle); 
    }

}
// End of CreateCumulativeHistogram


// To draw summary histogram from cumulativeHistogrmas
double GasDependence :: WriteCumulativeSummary(const HistogramMap& histograms , const CumulativeMap& cumulativeHistograms, TString type_cumulative, TString var, double binWidth) {
  double slope_value = 0;
    auto summaryHistogram = std::make_unique<TGraphAsymmErrors>();
    static const std::map<std::string, std::string> labels = {
        {"all", ""}, {"plus", " : +endcap"}, {"minus", " : -endcap"},
        {"odd_chambers", " : odd chambers"}, {"even_chambers", " : even chambers"},
        {"lower_plus", " : lower +endcap"}, {"lower_minus", " : lower -endcap"},
        {"upper_plus", " : upper +endcap"}, {"upper_minus", " : upper -endcap"},
        {"odd_layers", " : odd layers"}, {"even_layers", " : even layers"}
    };
    summaryHistogram->SetTitle("Cumulative : " + chamber_name + " : " + year + " : " + var +
        labels.at(type_cumulative.Data()).c_str());
   // Category-specific x coordinates must match the channels contributing to y.
   const auto mean_var_bin = MeanBinCoordinates(histograms, type_cumulative, chamber_name);
    int n_points_summary = 0;
   
    // Reading cumulative Histogram into a summary plot 
    for (auto& pair : cumulativeHistograms) {
        int pressureBin = pair.first;
        TH1D* cumulativeHistogram = pair.second.get();
        if(cumulativeHistogram->Integral() <100 || mean_var_bin.count(pressureBin) == 0) continue;
        auto trimmed_cumulativeHistogram = TrimChargeHistogram(cumulativeHistogram);
        // Calculate the mean of the cumulative histogram
        double mean = trimmed_cumulativeHistogram->GetMean();
        double mean_error = trimmed_cumulativeHistogram->GetMeanError();

        summaryHistogram->SetPoint(n_points_summary, mean_var_bin.at(pressureBin), mean);
        summaryHistogram->SetPointError(n_points_summary, 0, 0, mean_error, mean_error);
        if(debug) std::cout<<" point "<<n_points_summary<<" mean "<<mean_var_bin.at(pressureBin)<<" bin actual "<<pressureBin<<" mean value "<<mean<<std::endl;
        n_points_summary++;

        // Diagnostic PDF export was disabled in the original program.
    }
     summaryHistogram->GetYaxis()->SetRangeUser(250,600);
     if(var=="instlumi" || var=="instlumi_second")
     summaryHistogram->GetYaxis()->SetRangeUser(0,1.5);
     if(var=="PV")
     summaryHistogram->GetYaxis()->SetRangeUser(0,1.5);

     ConfigureGraphAxes(*summaryHistogram, var);
     if (var == "pressure" || var == "pressure_second") summaryHistogram->GetXaxis()->SetRangeUser(940, 990);
     if (var == "instlumi" || var == "instlumi_second") summaryHistogram->GetXaxis()->SetRangeUser(0, 3);
     if (var == "intlumi_initial" || var == "intlumi_final") summaryHistogram->GetXaxis()->SetRangeUser(0, 160);

     if(debug) std::cout<<" number of points "<<summaryHistogram->GetN();
     summaryHistogram->SetMarkerStyle(20);
     summaryHistogram->SetMarkerSize(0.5);

     auto c = std::make_unique<TCanvas>();
     gStyle->SetOptStat(111112211);
     gStyle->SetOptFit(1111);
    
     if(debug) std::cout<<" before plotting and fitting summary"<<std::endl; 
    
     slope_value = FitDependence(*summaryHistogram, var, binWidth, type_cumulative == "all", "expFit");
     c->cd();
     summaryHistogram->Draw("AP");
     gPad->Update();
     c->Update();
 
     summaryHistogram->SetName("dataset_trimmed_"+chamber_name+"_allgoodchannelsvs_"+var+"_"+type_cumulative);

     if (summaryHistogram->Write() <= 0) throw std::runtime_error("Failed to write summary graph");

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
 else throw std::invalid_argument("Unsupported correction type");
 double thecorr = std::exp(slope*(refvalue-X));
 return thecorr;
  }
