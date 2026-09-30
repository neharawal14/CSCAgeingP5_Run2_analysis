// gas_gain_refactored.cpp
#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <cmath>
#include <limits>
#include <algorithm>
#include <TString.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TGraphAsymmErrors.h>
#include <TF1.h>
#include <TAxis.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TStyle.h>
#include <TDirectoryFile.h>
class GasGain {
public:
    TString ratio_type;
    TFile* output_file;
    std::vector<TString> years = {"2016", "2017", "2018"};

 
   double Intlumi2016 ;double Intlumi2017; 
    void initialise_variable(TString);
    void reading_gas_gain_cumulative();
    std::pair<TString, TString> getRatioLabels() const;

    std::pair<double, double> FindingRange(TGraphAsymmErrors*org , TGraphAsymmErrors *ref);
    TGraphAsymmErrors* combiningGraphs(std::vector<TGraphAsymmErrors*>, TString);
    void DrawGraph(TGraphAsymmErrors* graphs ,TString save_path, TString chamber);
    TGraphAsymmErrors* NormalisingCombinedGraphs(TGraphAsymmErrors* graphs) ;
    std::pair<double, double> NormaliseFirstBin(TGraphAsymmErrors* combined, TString);
    void normaliseGraph(TGraphAsymmErrors*, TString, TString, TGraphAsymmErrors*);
    std::pair<TGraphAsymmErrors*, TGraphAsymmErrors*> normaliseAndFit(TGraphAsymmErrors*, TString, TString, TGraphAsymmErrors*);
    std::pair<float, float> fitGraph(TGraphAsymmErrors*, TString, TString);
    void drawMeanPlot(std::vector<float>, std::vector<float>, TString, std::vector<TString>, TString);
};

std::pair<TString, TString> GasGain::getRatioLabels() const {
    static const std::map<TString, std::pair<TString, TString>> ratio_map = {
        {"plus_minus_endcap", {"minus", "plus"}},
        {"odd_even_chambers", {"odd_chambers", "even_chambers"}},
        {"odd_even_layers", {"odd_layers", "even_layers"}},
        {"lower_plus_minus_endcap", {"lower_minus", "lower_plus"}},
        {"upper_plus_minus_endcap", {"upper_minus", "upper_plus"}}
    };
    return ratio_map.at(ratio_type);
}

void styleGraph(TGraphAsymmErrors* graph, Color_t color, Style_t style = 20, Size_t size = 0.5) {
    graph->SetMarkerColor(color);
    graph->SetMarkerStyle(style);
    graph->SetMarkerSize(size);
}

std::pair<double, double> calculateYRange(TGraphAsymmErrors* graph) {
    double minY = std::numeric_limits<double>::max();
    double maxY = -std::numeric_limits<double>::max();
    for (int i = 0; i < graph->GetN(); ++i) {
        double x, y;
        graph->GetPoint(i, x, y);
        if (y > 0) {
            minY = std::min(minY, y);
            maxY = std::max(maxY, y);
        }
    }
    if (minY == std::numeric_limits<double>::max()) minY = 1e-6;
    return {minY, maxY};
}
void GasGain :: DrawGraph(TGraphAsymmErrors* graphs ,TString save_path, TString chamber){
    // Create vertical lines at x = 39.3 and x = 83.3
    double minY = std::numeric_limits<double>::max();
    double maxY = -std::numeric_limits<double>::max();

    for (int i = 0; i < graphs->GetN(); ++i) {
        double x, y;
        graphs->GetPoint(i, x, y);
    
        if (y < minY) minY = y;
        if (y > maxY) maxY = y;
    }
    if(minY < 50){
      minY = 0.95;
      maxY = 1.05;
    }
    TLine *line1 = new TLine(Intlumi2016, minY, Intlumi2016, maxY);
    TLine *line2 = new TLine(Intlumi2017, minY, Intlumi2017, maxY);
    // Set line style and color
    line1->SetLineColor(kOrange);
    line2->SetLineColor(kOrange);
    line1->SetLineStyle(2);  // Dashed
    line2->SetLineStyle(2);  // Dashed
    
    // Then draw the lines on top
  gStyle->SetTimeOffset(0.);
  graphs->GetXaxis()->SetTimeDisplay(1);
  graphs->GetXaxis()->SetLabelSize(0.02);
  graphs->GetXaxis()->SetTimeFormat("%m/%d");

  TCanvas *c = new TCanvas();
  c->cd();
  graphs->Draw("AP*");
    line1->Draw("same");
    line2->Draw("same");

  graphs->GetXaxis()->SetTitle("Integrated luminosity (fb^{-1})");
  graphs->SetMarkerSize(0.5);
  graphs->SetMarkerStyle(20);

  
  c->SaveAs("AllResults/results_gas_gain_" + ratio_type + "_time/intlumi/"+save_path+ chamber + "_" + ratio_type + "_before_ratio.pdf");
}
TGraphAsymmErrors* GasGain::combiningGraphs(std::vector<TGraphAsymmErrors*> graphs, TString chamber) {
    int totalPoints = 0;
    for (auto g : graphs) totalPoints += g->GetN();

    //Adding all year together before normalising
    std::vector<double> x(totalPoints), y(totalPoints), exl(totalPoints), exh(totalPoints), eyl(totalPoints), eyh(totalPoints);
    int idx = 0;
    for (auto g : graphs) {
        for (int i = 0; i < g->GetN(); ++i) {
            double xi, yi;
            g->GetPoint(i, xi, yi);
            if(yi <=0 ) continue;
                x[idx] = xi;
                y[idx] = yi;
                exl[idx] = g->GetErrorXlow(i);
                exh[idx] = g->GetErrorXhigh(i);
                eyl[idx] = g->GetErrorYlow(i);
                eyh[idx] = g->GetErrorYlow(i);
                ++idx;
            }
        }

    return new TGraphAsymmErrors(idx, x.data(), y.data(), exl.data(), exh.data(), eyl.data(), eyh.data());
    }


TGraphAsymmErrors* GasGain:: NormalisingCombinedGraphs(TGraphAsymmErrors* graphs) {
    int totalPoints = 0;
     totalPoints += graphs->GetN();

    //Adding all year together before normalising
    std::vector<double> x(totalPoints), y(totalPoints), exl(totalPoints), exh(totalPoints), eyl(totalPoints), eyh(totalPoints);

    double refY = 1.0, refErr = 0.0;
    for (int i = 0; i < graphs->GetN(); ++i) {
        double x_, y_;
        graphs->GetPoint(i, x_, y_);
        if (y_ > 0) {
            refY = y_;
            refErr = graphs->GetErrorYhigh(i);
            break;
        }
    }

    int idx = 0;
      for (int i = 0; i < graphs->GetN(); ++i) {
            double xi, yi;
            graphs->GetPoint(i, xi, yi);
            if (yi > 0) {
                double ratio = yi / refY;
                double err = ratio * std::sqrt(std::pow(graphs->GetErrorYhigh(i) / yi, 2) + std::pow(refErr / refY, 2));
                x[idx] = xi;
                y[idx] = ratio;
                exl[idx] = graphs->GetErrorXlow(i);
                exh[idx] = graphs->GetErrorXhigh(i);
                eyl[idx] = err;
                eyh[idx] = err;
                ++idx;
            }
    }


    return new TGraphAsymmErrors(idx, x.data(), y.data(), exl.data(), exh.data(), eyl.data(), eyh.data());
}
std::pair<double, double> GasGain::FindingRange(TGraphAsymmErrors*org , TGraphAsymmErrors *ref){
    // Loop over all points in the graph
    double minY = std::numeric_limits<double>::max();
    double maxY = -std::numeric_limits<double>::max();
    for (int i = 0; i < org->GetN(); ++i) {
        double x, y;
        org->GetPoint(i, x, y);
        if (y > 0) { // Ignore points with zero or negative Y
            if (y < minY) minY = y;
            if (y > maxY) maxY = y;
        }
    }
    
    if (minY == std::numeric_limits<double>::max()) {
        minY = 1e-6; // Default small value if all points are zero or skipped
    }
    
    // Optionally, add a margin to Y-axis range
    double marginFactor = 0.02; // 10% margin
    double rangeMin = minY - marginFactor * fabs(minY);
    double rangeMax = maxY + marginFactor * fabs(maxY);
 
    double minY1 = std::numeric_limits<double>::max();
    double maxY1 = -std::numeric_limits<double>::max();
    
    // Loop over all points in the graph
    for (int i = 0; i < ref->GetN(); ++i) {
        double x, y;
        ref->GetPoint(i, x, y);
        if (y > 0) { // Ignore points with zero or negative Y
            if (y < minY1) minY1 = y;
            if (y > maxY1) maxY1 = y;
        }
    }
    
    if (minY1 == std::numeric_limits<double>::max()) {
        minY1 = 1e-6; // Default small value if all points are zero or skipped
    }
    
    // Optionally, add a margin to Y-axis range
    double marginFactor1 = 0.05; // 10% margin
    double rangeMin1 = minY1 - marginFactor * fabs(minY1);
    double rangeMax1 = maxY1 + marginFactor * fabs(maxY1);
   
    double rangeMinall , rangeMaxall;
    if(rangeMin <rangeMin1) rangeMinall  = rangeMin;
    else rangeMinall = rangeMin1; 
    if(rangeMax >rangeMax1) rangeMaxall =  rangeMax;
    else rangeMaxall = rangeMax1; 

    return std::make_pair(rangeMinall, rangeMaxall);
 
}
void GasGain::normaliseGraph(TGraphAsymmErrors* graph, TString chamber, TString year, TGraphAsymmErrors* ref) {
  gStyle->SetTimeOffset(0.);
  graph->GetXaxis()->SetTimeDisplay(1);
  graph->GetXaxis()->SetLabelSize(0.02);
  graph->GetXaxis()->SetTimeFormat("%m/%d");
  ref->GetXaxis()->SetTimeDisplay(1);
  ref->GetXaxis()->SetLabelSize(0.02);
  ref->GetXaxis()->SetTimeFormat("%m/%d");
 
 
  TGraphAsymmErrors* graph_clone =  (TGraphAsymmErrors*)graph->Clone();
  TGraphAsymmErrors* ref_clone =  (TGraphAsymmErrors*)ref->Clone();

  std::pair<double, double> range; 
//  range =  FindingRange(graph_clone, ref_clone); 
//  graph_clone->GetYaxis()->SetRangeUser(range.first,range.second);
//  ref_clone->GetYaxis()->SetRangeUser(range.first,range.second);

  graph_clone->GetYaxis()->SetRangeUser(280,500);
  ref_clone->GetYaxis()->SetRangeUser(280,500);

  graph_clone->GetXaxis()->SetRangeUser(280,500);
  ref_clone->GetXaxis()->SetRangeUser(280,500);

 
    styleGraph(graph_clone, kBlue);
    styleGraph(ref_clone, kRed);

    TLine *line1; 
    TLine *line2;
//  gStyle->SetTimeOffset(0.);
//  graph_clone->GetXaxis()->SetTimeDisplay(1);
//  graph_clone->GetXaxis()->SetLabelSize(0.02);
//  graph_clone->GetXaxis()->SetTimeFormat("%Y/%m/%d");
//  ref_clone->GetXaxis()->SetTimeDisplay(1);
//  ref_clone->GetXaxis()->SetLabelSize(0.02);
//  ref_clone->GetXaxis()->SetTimeFormat("%Y/%m/%d");


    graph_clone->GetYaxis()->SetTitle("Trimmed Mean Charge");
    graph_clone->GetXaxis()->SetTitle("Integrated luminosity (fb^{-1})");
    ref_clone->GetYaxis()->SetTitle("Trimmed Mean Charge");
    ref_clone->GetXaxis()->SetTitle("Integrated luminosity (fb^{-1})");
    graph_clone->SetTitle("Cumulative : "+chamber+" : "+year+" : Gas gain after pressure/instlumi corr.");
    TCanvas* c = new TCanvas();
    graph_clone->Draw("AP");
    ref_clone->Draw("P SAME");

    if(year=="run2") {
    line1 = new TLine(Intlumi2016, 280,  Intlumi2016, 500);
    line2 = new TLine(Intlumi2017, 280, Intlumi2017, 500);
    // Set line style and color
    line1->SetLineColor(kOrange);
    line2->SetLineColor(kOrange);
    line1->SetLineStyle(2);  // Dashed
    line2->SetLineStyle(2);  // Dashed
    line1->Draw("same");
    line2->Draw("same");
    }
    auto labels = getRatioLabels();
    TLegend* leg = new TLegend(0.1, 0.8, 0.3, 0.9);
    leg->AddEntry(graph_clone, chamber + " " + labels.first, "lep");
    leg->AddEntry(ref_clone, chamber + " " + labels.second, "lep");
    leg->Draw("same");

//    c->SaveAs("AllResults/results_gas_gain_" + ratio_type + "_time/intlumi/" + year + "/plots_all/" + chamber + "_" + ratio_type + "_before_ratio_" + year + ".pdf");
    c->SaveAs("AllResults/results_gas_gain_" + ratio_type + "_time/intlumi/" + year + "/plots_all/" + chamber + "_" + ratio_type + "_before_ratio_" + year + "_initial.pdf");
}

std::pair<TGraphAsymmErrors*, TGraphAsymmErrors*> GasGain::normaliseAndFit(TGraphAsymmErrors* gOrig, TString chamber, TString year, TGraphAsymmErrors* gRef) {
    int n = gOrig->GetN();
    std::vector<double> x(n), y(n), exl(n), exh(n), eyl(n), eyh(n);

    for (int i = 0; i < n; ++i) {
        double xo, yo, xr, yr;
        gOrig->GetPoint(i, xo, yo);
        gRef->GetPoint(i, xr, yr);

        x[i] = xo;
        exl[i] = gOrig->GetErrorXlow(i);
        exh[i] = gOrig->GetErrorXhigh(i);

        if (yr != 0) {
            y[i] = yo / yr;
            double errTerm1 = gOrig->GetErrorYhigh(i) / yo;
            double errTerm2 = gRef->GetErrorYhigh(i) / yr;
            double totalErr = y[i] * std::sqrt(errTerm1 * errTerm1 + errTerm2 * errTerm2);
            eyl[i] = eyh[i] = totalErr;
        } else {
            y[i] = eyl[i] = eyh[i] = 0;
        }
    }

    TGraphAsymmErrors* h_norm = new TGraphAsymmErrors(n, x.data(), y.data(), exl.data(), exh.data(), eyl.data(), eyh.data());
    styleGraph(h_norm, kBlack);
    h_norm->SetTitle(chamber + " : Normalised Gas Gain vs Luminosity");

  gStyle->SetTimeOffset(0.);
  h_norm->GetXaxis()->SetTimeDisplay(1);
  h_norm->GetXaxis()->SetLabelSize(0.02);
  h_norm->GetXaxis()->SetTimeFormat("%m/%d");


    TCanvas* c = new TCanvas();
    h_norm->Draw("AP *");
    c->SaveAs("AllResults/results_gas_gain_" + ratio_type + "_time/intlumi/" + year + "/plots_all/" + chamber + "_" + ratio_type + "_after_ratio_" + year + ".pdf");

    // Normalised the norm histogram with respect to the first bin 
    bool flag_first_bin = false;
    double value_first_bin = 0, value_first_bin_err = 0;
    
    int n1 = h_norm->GetN();
    std::vector<double> x1(n1), y1(n1), exl1(n1), exh1(n1), eyl1(n1), eyh1(n1);
    
    for (int i = 0; i < n1; i++) {
        double xi, yi;
        h_norm->GetPoint(i, xi, yi);
        
        if (yi == 0) {
            y1[i] = 0;
            eyl1[i] = 0;
            eyh1[i] = 0;
        }
        if (yi != 0 && !flag_first_bin) {
            flag_first_bin = true;
            value_first_bin = yi;
            value_first_bin_err = (h_norm->GetErrorYlow(i) + h_norm->GetErrorYhigh(i)) / 2;
        }
        if (yi != 0 && flag_first_bin) {
            double value_err = (h_norm->GetErrorYlow(i) + h_norm->GetErrorYhigh(i)) / 2;
            double ratio = yi / value_first_bin;
            double ratio_err = ratio * sqrt(pow(value_first_bin_err / value_first_bin, 2) + pow(value_err / yi, 2));
            y1[i] = ratio;
            eyl1[i] = ratio_err;
            eyh1[i] = ratio_err;
        }
        x1[i] = xi;
        exl1[i] = h_norm->GetErrorXlow(i);
        exh1[i] = h_norm->GetErrorXhigh(i);
    }
 
    TGraphAsymmErrors* h_norm_first = new TGraphAsymmErrors(n1, x1.data(), y1.data(), exl1.data(), exh1.data(), eyl1.data(), eyh1.data());
    styleGraph(h_norm_first, kBlack);
    h_norm_first->SetTitle(chamber + " :  Normalised Gas Gain vs Luminosity");
    h_norm_first->GetYaxis()->SetRangeUser(0.95,1.05);
  gStyle->SetTimeOffset(0.);
  h_norm_first->GetXaxis()->SetTimeDisplay(1);
  h_norm_first->GetXaxis()->SetLabelSize(0.02);
  h_norm_first->GetXaxis()->SetTimeFormat("%Y/%m/%d");

    TCanvas* c1 = new TCanvas();
    h_norm_first->Draw("AP*");
    c1->SaveAs("AllResults/results_gas_gain_" + ratio_type + "_time/intlumi/" + year + "/plots_all/" + chamber + "_" + ratio_type + "_after_ratio_" + year + "_normalised_first_bin.pdf");
    // end of normalisation for first

    return {h_norm, h_norm_first}; // For brevity, skipping second normalization
}

std::pair<float, float> GasGain::fitGraph(TGraphAsymmErrors* graph, TString chamber, TString year) {
    int n = graph->GetN();
    double xmin = 0, xmax = 0;
    bool found = false;

    for (int i = 0; i < n; ++i) {
        double x, y;
        graph->GetPoint(i, x, y);
        if (y != 0 && !found) {
            xmin = x;
            found = true;
        }
        if (y != 0) xmax = x;
    }

    TF1* fit = new TF1("fit", "[0]*x + [1]", xmin, xmax);
    graph->Fit(fit, "R");

    float slope = fit->GetParameter(0);
    float slopeErr = fit->GetParError(0);

    TCanvas* c = new TCanvas();
    gStyle->SetOptFit(1111);
    graph->Draw("AP*");
    fit->Draw("same");
    c->SaveAs("AllResults/results_gas_gain_" + ratio_type + "_time/intlumi/" + year + "/fits/" + chamber + "_fit.pdf");

    return {slope, slopeErr};
}

void GasGain::initialise_variable(TString ratio_type_string) {
    ratio_type = ratio_type_string;
    Intlumi2016= 39.33;
    Intlumi2017 = 83.85; 
}

std::pair<double, double> GasGain :: NormaliseFirstBin(TGraphAsymmErrors* combined, TString chamber){

    double bin_low_edge, bin_up_edge;
    bool flag_first = false;
    int n = combined->GetN();
    for (int i = 0; i < n; i++) {
         double x, y;
         combined->GetPoint(i, x, y);
         if (y != 0 && !flag_first) {
             bin_low_edge = x;
             flag_first = true;
         }
         if (y != 0) bin_up_edge = x;
     }
     if (bin_low_edge >= bin_up_edge) {
         std::cerr << "Error: Invalid fit range. No non-zero entries found.\n";
     }
     TF1 *fa1 = new TF1("fa1","[0] *x+[1]", bin_low_edge, bin_up_edge);
     combined->Fit(fa1,"R");
     auto labels = this->getRatioLabels();
     combined->SetTitle(chamber+ " : Gas gain after pressure/instlumi corr. (" +labels.first +" noramlised with "+labels.second +" endcaps)");
     this->DrawGraph(combined, "run2/fits_all/", chamber);

     return std::make_pair(fa1->GetParameter(0), fa1->GetParError(0));
    }

void GasGain :: drawMeanPlot(std::vector<float> mean_values_vector, std::vector<float> mean_error_values_vector, TString var, std::vector<TString> chamber_name, TString year){
    
    std::cout<<" started in plot mean plot "<<std::endl;
    std::vector<float> value ={1,2,3,4,5,6,7,8,9,10,11,12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};

    int size_chamber = chamber_name.size();
    value.resize(size_chamber);
    //std::vector<float> value = {1,2,3, 4,5, 6, 7, 8};
    TGraphErrors *graph_mean_values = new TGraphErrors(size_chamber, value.data(), mean_values_vector.data(), 0, mean_error_values_vector.data());
    std::cout<<" after declaring graph "<<std::endl;
    graph_mean_values->GetXaxis()->SetTickLength(1);
    TString graph_title;
    if(var=="pressure" || var=="instlumi"){
      graph_title = " Slope of gas gain dependence on "+var+" : "+year;
    }
    else if(var=="pressure_second"){
      graph_title = " Slope of gas gain dependence on "+var+" after pressure corr : "+year;
    }
    else if(var=="instlumi_second"){
      graph_title = " Slope of gas gain dependence on "+var+" after pressure/instlumi corr : "+year;
    }
    else if(var=="intlumi"){
      graph_title = " Slope of gas gain dependence on integrated luminosity (after pressure/instlumi cor.) : "+year;

    }
    else if(var=="time"){
      graph_title = " Slope of gas gain dependence on time (after pressure/instlumi cor.) : "+year;

    }

    if(var=="pressure"){
      graph_mean_values->GetYaxis()->SetRangeUser(-0.010,0.002); 
    }
    else if(var=="pressure_second") graph_mean_values->GetYaxis()->SetRangeUser(-0.0005,0.0005);
    else if(var=="instlumi") graph_mean_values->GetYaxis()->SetRangeUser(-0.00001,0.00001);
    else if(var=="instlumi_second") graph_mean_values->GetYaxis()->SetRangeUser(-0.00001,0.00001);
    else if(var=="intlumi") graph_mean_values->GetYaxis()->SetRangeUser(-0.0003,0.0003);
    else if(var=="time") graph_mean_values->GetYaxis()->SetRangeUser(-0.0003,0.0003);
    std::cout<<" before declaring axes "<<std::endl;
    TAxis *axis = graph_mean_values->GetXaxis();
    axis->Draw();
   
    for(int i=0; i<chamber_name.size(); i++){
       graph_mean_values->GetXaxis()->SetBinLabel(graph_mean_values->GetXaxis()->FindBin(i + 1.), chamber_name[i]); // Find out     which bin on the x-axis the point corresponds to and set the bin label
    }
    graph_mean_values->GetXaxis()->SetTitleOffset(0.1); 
    TCanvas *canv1 = new TCanvas();
    canv1->cd();
    canv1->SetLeftMargin(0.12);
    canv1->SetGrid();
    gPad->SetGrid();
    graph_mean_values->Draw("AP");
    graph_mean_values->SetTitle(graph_title);
    Double_t *gr_xarray = graph_mean_values->GetX();
    Double_t *gr_yarray = graph_mean_values->GetY();

    std::cout<<" before starting marker "<<std::endl;
    std::map <TString, int> marker_colour = { {"ME11a", 8}, {"ME11b", 8}, 
      {"ME12HV1", 2}, {"ME12HV2", 2}, {"ME12HV3", 2},
      {"ME13HV1", 2}, {"ME13HV2", 2}, {"ME13HV3", 2},
      {"ME21HV1", 4}, {"ME21HV2", 4}, {"ME21HV3", 4},
      {"ME22HV1", 2},{"ME22HV2", 2},{"ME22HV3", 2},{"ME22HV4", 2},{"ME22HV5", 2},
      {"ME31HV1", 4}, {"ME31HV2", 4}, {"ME31HV3", 4},
      {"ME32HV1", 2},{"ME32HV2", 2},{"ME32HV3", 2},{"ME32HV4", 2},{"ME32HV5", 2},
      {"ME41HV1", 4}, {"ME41HV2", 4}, {"ME41HV3", 4},
      {"ME42HV1", 2},{"ME42HV2", 2},{"ME42HV3", 2},{"ME42HV4", 2},{"ME42HV5", 2}
    };
    //int marker_colour[32] = {8, 8, 2, 2, 2, 2,2,2, 4,4,4, 2,2,2,2,2, 4,4,4, 2,2,2,2,2, 4,4,4, 2,2,2,2,2 };
    //int marker_colour[32] = {8, 8, 2, 2, 2, 2,2,2,  4,4,4, 2,2,2,2,2, 4,4,4, 2,2,2,2,2, 4,4,4, 2,2,2,2,2 };
    //int marker_colour[32] = {8, 2, 2, 2, 2, 2,2, 4,4,4, 2,2,2,2,2, 4,4,4, 2,2,2,2,2, 4,4,4, 2,2,2,2,2 };
    TLegend *legend_1 = new TLegend(0.7,0.7,0.9,0.9);
    for (Int_t j=0; j<chamber_name.size(); j++) {
    std::cout<<" declaring gr x arrayr "<<std::endl;
        TMarker *m = new TMarker(gr_xarray[j], gr_yarray[j], 20);
        m->SetMarkerColor(marker_colour[chamber_name[j]]);
        m->Draw();

      std::cout<<"after declaring gr x arrayr "<<std::endl;
			if(j==0) {
				legend_1->AddEntry(m," ME11a, ME11b (10^{0})","p");
			}
			if(j==3) {
			legend_1->AddEntry(m," Outer Chambers (10^{0})","p"); }
			if(j==8)
			legend_1->AddEntry(m," Inner Chambers (20^{0})","p");
   }
   std::cout<<" before drawing mulitgraph "<<std::endl;
	 legend_1->Draw("SAME");

   TLatex* cmslabel_1;
   TLatex* text1,*text2;
   cmslabel_1 = new TLatex(0.18,0.82, "CMS #bf{#it{Preliminary}}");
   cmslabel_1->SetNDC(kTRUE);
   cmslabel_1->SetTextSize(0.06);
   cmslabel_1->SetTextFont(42);
   cmslabel_1->Draw("same");

	 canv1->SaveAs("AllResults/results_gas_gain_"+ratio_type+"_time/mean_slope_values_"+var+"_"+year+"_fit.pdf"); 
  }

// Read gas gain in each individual year, and normalise the gain with respect to desired chambers or endcap
//  make ratio plots and comparison  plots, and combine 
void GasGain::reading_gas_gain_cumulative() {
    std::vector<float> mean_intlumi_run2_value_list;
    std::vector<float> mean_intlumi_run2_error_value_list;

    std::vector<TString> chambers = {"ME11a", "ME11b", "ME12HV1", "ME12HV2", "ME12HV3",
                                     "ME13HV1", "ME13HV2", "ME13HV3", "ME21HV1", "ME21HV2",
                                     "ME21HV3", "ME22HV1", "ME22HV2", "ME22HV3", "ME22HV4",
                                     "ME22HV5", "ME31HV1", "ME31HV2", "ME31HV3", "ME32HV1",
                                     "ME32HV2", "ME32HV3", "ME32HV4", "ME32HV5", "ME41HV1",
                                     "ME41HV2", "ME41HV3", "ME42HV1", "ME42HV2", "ME42HV3",
                                     "ME42HV4", "ME42HV5"};

    std::map<TString, std::vector<float>> slopes, errors;
    output_file = new TFile("output_file_" + ratio_type + ".root", "RECREATE");
    auto [label_first, label_second] = this->getRatioLabels();

    for (const auto& chamber : chambers) {
        std::vector<TGraphAsymmErrors*> yearly_graphs;
        std::vector<TGraphAsymmErrors*> yearly_graphs_ref;
        std::vector<TGraphAsymmErrors*> yearly_graphs_orig;
        for (const auto& year : years) {
            TString filename = year + "_full_time/dataset_output_" + chamber + "_" + year + ".root";
            TFile* file = TFile::Open(filename);
            if (!file) continue;

            auto* dir_initial = (TDirectoryFile*)file->Get("timesecond_initial");
            auto* g_ref_initial = (TGraphAsymmErrors*)dir_initial->Get("dataset_trimmed_" + chamber + "_allgoodchannelsvs_timesecond_initial_" + label_first);
            auto* g_orig_initial = (TGraphAsymmErrors*)dir_initial->Get("dataset_trimmed_" + chamber + "_allgoodchannelsvs_timesecond_initial_" + label_second);
            this->normaliseGraph(g_orig_initial, chamber, year, g_ref_initial);

////            auto* dir = (TDirectoryFile*)file->Get("timesecond_final");
////            auto* g_ref = (TGraphAsymmErrors*)dir->Get("dataset_trimmed_" + chamber + "_allgoodchannelsvs_timesecond_final_" + label_first);
////            auto* g_orig = (TGraphAsymmErrors*)dir->Get("dataset_trimmed_" + chamber + "_allgoodchannelsvs_timesecond_final_" + label_second);
////
////            if (!g_ref || !g_orig) continue;
////
////            this->normaliseGraph(g_orig, chamber, year, g_ref);
////            yearly_graphs_orig.push_back(g_orig);
////            yearly_graphs_ref.push_back(g_ref);
////            auto [g_norm, g_norm_first] = this->normaliseAndFit(g_orig, chamber, year, g_ref);
////            yearly_graphs.push_back(g_norm);
////
////            auto [slope, err] = this->fitGraph(g_norm_first, chamber, year);
////            slopes[year].push_back(slope);
////            errors[year].push_back(err);
        }

////        auto* combined_org = this->combiningGraphs(yearly_graphs_orig, chamber);
////        auto* combined_ref = this->combiningGraphs(yearly_graphs_ref, chamber);
////
////
////
////        this->normaliseGraph(combined_org, chamber, "run2", combined_ref);
////
////        auto* combined = this->combiningGraphs(yearly_graphs, chamber);
////        combined->SetTitle("Final Normalised Gas Gain: " + chamber);
////        combined->GetYaxis()->SetRangeUser(0.9,1.1);
////        output_file->cd();
////        combined->Write();
////        this->DrawGraph(combined, "run2/plots_all/" , chamber);
////
////        // Normalise combine histogram wrt first bin and fit too 
////        auto *combined_normalised = this->NormalisingCombinedGraphs(combined);
////        combined_normalised->GetYaxis()->SetRangeUser(0.95,1.05);
////        combined_normalised->Write();
////        std::pair<double, double> slope_mean_list;
////        slope_mean_list = this->NormaliseFirstBin(combined_normalised, chamber);
////
////        mean_intlumi_run2_value_list.push_back(slope_mean_list.first);
////        mean_intlumi_run2_error_value_list.push_back(slope_mean_list.second);

    }

//    for (const auto& year : years) {
//        drawMeanPlot(slopes[year], errors[year], "time", chambers, year);
//    }
//     drawMeanPlot(mean_intlumi_run2_value_list, mean_intlumi_run2_error_value_list, "time", chambers, "run2");
    output_file->Close();
}
void GasGain_CrossEndcap_time(TString ratio_type) {
    GasGain obj;
    obj.initialise_variable(ratio_type);
    obj.reading_gas_gain_cumulative();
}
