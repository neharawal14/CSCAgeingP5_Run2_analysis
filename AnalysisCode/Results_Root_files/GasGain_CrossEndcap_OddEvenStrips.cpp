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
    std::vector<TString> years = {"Run2"};

    TString outputfileName;
 
   double Intlumi2016 ;double Intlumi2017; 
    void initialise_variable(TString, TString);
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
        {"odd_even_strips", {"odd_strips", "even_strips"}}
//        {"plus_minus_endcap", {"minus", "plus"}},
//        {"odd_even_lumiBlock", {"odd_lumiBlock", "even_lumiBlock"}},
//        {"odd_even_chambers", {"odd_chambers", "even_chambers"}},
//        {"odd_even_layers", {"odd_layers", "even_layers"}},
//        {"lower_plus_minus_endcap", {"lower_minus", "lower_plus"}},
//        {"upper_plus_minus_endcap", {"upper_minus", "upper_plus"}}
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


  TCanvas *c = new TCanvas();
  c->cd();
  graphs->Draw("AP*");
//  line1->Draw("same");
//  line2->Draw("same");

  graphs->GetXaxis()->SetTitle("Integrated luminosity (fb^{-1})");
  graphs->SetMarkerSize(0.5);
  graphs->SetMarkerStyle(20);

  
  c->SaveAs("AllResults/results_gas_gain_" + ratio_type + "/intlumi/"+save_path+ chamber + "_" + ratio_type + "_before_ratio.pdf");
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
  TGraphAsymmErrors* graph_clone =  (TGraphAsymmErrors*)graph->Clone();
  TGraphAsymmErrors* ref_clone =  (TGraphAsymmErrors*)ref->Clone();

  std::pair<double, double> range; 
///  range =  FindingRange(graph_clone, ref_clone); 
///  graph_clone->GetYaxis()->SetRangeUser(range.first,range.second);
///  ref_clone->GetYaxis()->SetRangeUser(range.first,range.second);

  graph_clone->GetYaxis()->SetRangeUser(280,500);
  ref_clone->GetYaxis()->SetRangeUser(280,500);
    styleGraph(graph_clone, kBlue);
    styleGraph(ref_clone, kRed);

    TLine *line1; 
    TLine *line2;
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
//    line1->Draw("same");
//    line2->Draw("same");
    }
    auto labels = getRatioLabels();
    TLegend* leg = new TLegend(0.1, 0.8, 0.3, 0.9);
    leg->AddEntry(graph_clone, chamber + " " + labels.first, "lep");
    leg->AddEntry(ref_clone, chamber + " " + labels.second, "lep");
    leg->Draw();

    c->SaveAs("AllResults/results_gas_gain_" + ratio_type + "/intlumi/run2/plots_all/" + chamber + "_" + ratio_type + "_before_ratio_" + year + ".pdf");
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

    TCanvas* c = new TCanvas();
    h_norm->Draw("AP *");
    c->SaveAs("AllResults/results_gas_gain_" + ratio_type + "/intlumi/run2/plots_all/" + chamber + "_" + ratio_type + "_after_ratio_" + year + ".pdf");

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
         }
        //std::cout<<" ************** taking ratio with for chamber **********"<<chamber_name<<" : "<<value_first_bin<<std::endl;
         double ratio = yi / value_first_bin;
          y1[i] = ratio;
          double ratio_err_low = h_norm->GetErrorYlow(i) * (1./value_first_bin);
          double ratio_err_up = h_norm->GetErrorYhigh(i) * (1./value_first_bin);
          eyl1[i] = ratio_err_low;
          eyh1[i] = ratio_err_up;

         x1[i] = xi;
         exl1[i] = h_norm->GetErrorXlow(i);
         exh1[i] = h_norm->GetErrorXhigh(i);
     }

    TGraphAsymmErrors* h_norm_first = new TGraphAsymmErrors(n1, x1.data(), y1.data(), exl1.data(), exh1.data(), eyl1.data(), eyh1.data());
    styleGraph(h_norm_first, kBlack);
    h_norm_first->SetTitle(chamber + " :  Normalised Gas Gain vs Luminosity");
    h_norm_first->GetYaxis()->SetRangeUser(0.95,1.05);

    TCanvas* c1 = new TCanvas();
    h_norm_first->Draw("AP*");
    c1->SaveAs("AllResults/results_gas_gain_" + ratio_type + "/intlumi/run2/plots_all/" + chamber + "_" + ratio_type + "_after_ratio_" + year + "_normalised_first_bin.pdf");
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
       TLatex* cmslabel_1, *cmslabel_2;
       TLatex* text1,*text2;
       cmslabel_1 = new TLatex(0.10,0.91, "#bf{CMS} #it{Preliminary}");
       cmslabel_1->SetNDC(kTRUE);
       cmslabel_1->SetTextSize(0.06);
       cmslabel_1->SetTextFont(42);
       cmslabel_2 = new TLatex(0.78,0.91,"13 TeV");
       cmslabel_2->SetNDC(kTRUE);
       cmslabel_2->SetTextSize(0.06);
       cmslabel_2->SetTextFont(42);


    TF1* fit = new TF1("fit", "[0]*x + [1]", xmin, xmax);
    graph->Fit(fit, "R");
    TString ytitle = "G_{odd strips("+chamber+")}/G_{even strips("+chamber+")}";
    graph->GetYaxis()->SetTitle(ytitle);
    graph->GetXaxis()->SetTitle("integrated luminosity (fb^{-1})");
    graph->SetTitle("");


    float slope = fit->GetParameter(0);
    float slopeErr = fit->GetParError(0);

    TCanvas* c = new TCanvas();
    c->cd();
    //gStyle->SetOptFit(1111);
    gStyle->SetOptStat(0);
    graph->Draw("AP*");
    fit->Draw("same");
    cmslabel_1->Draw("same");
    cmslabel_2->Draw("same");
    c->SaveAs("AllResults/results_gas_gain_" + ratio_type + "/intlumi/run2/fits/" + chamber + "_fit.pdf");

    return {slope, slopeErr};
}

void GasGain::initialise_variable(TString ratio_type_string, TString outputfileName1) {
    ratio_type = ratio_type_string;
    outputfileName = outputfileName1;
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
    std::vector<float> mean_values_vector_multiplied;
    mean_values_vector_multiplied.reserve(mean_values_vector.size());
    std::vector<float> mean_error_values_vector_multiplied;
    mean_error_values_vector_multiplied.reserve(mean_error_values_vector.size());

    TLatex *scaleLabel = new TLatex();
    scaleLabel->SetNDC(kTRUE);
    scaleLabel->SetTextFont(42);
    scaleLabel->SetTextSize(0.06);
    for(auto v : mean_values_vector){
     mean_values_vector_multiplied.push_back(v*10000);   
    }
    for(auto v : mean_error_values_vector){
     mean_error_values_vector_multiplied.push_back(v*10000);   
    }


    TGraphErrors *graph_mean_values = new TGraphErrors(size_chamber, value.data(), mean_values_vector_multiplied.data(), 0, mean_error_values_vector_multiplied.data());
    //TGraphErrors *graph_mean_values = new TGraphErrors(size_chamber, value.data(), mean_values_vector.data(), 0, mean_error_values_vector.data());
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
    if(var=="pressure"){
      graph_mean_values->GetYaxis()->SetRangeUser(-0.010,0.002); 
      graph_mean_values->GetYaxis()->SetTitle("(#DeltaG/G)/P (/hPa)"); 
    }
    else if(var=="pressure_second") {graph_mean_values->GetYaxis()->SetRangeUser(-0.0005,0.0005);
      graph_mean_values->GetYaxis()->SetTitle("(#DeltaG/G)/P (/hPa)"); 
    }
    else if(var=="instlumi") {graph_mean_values->GetYaxis()->SetRangeUser(-0.00001,0.00001);
      graph_mean_values->GetYaxis()->SetTitle("(#DeltaG/G)/L_{inst} (/1*10^{34} cm^{-2} s^{-1})"); 
    
    }
    else if(var=="instlumi_second") { 
	graph_mean_values->GetYaxis()->SetRangeUser(-0.00001,0.00001);
        graph_mean_values->GetYaxis()->SetTitle("(#DeltaG/G)/L_{inst} (/1*10^{34} cm^{-2} s^{-1})"); 
    }
    else if(var=="intlumi") {graph_mean_values->GetYaxis()->SetRangeUser(-2,2); 
	graph_mean_values->GetYaxis()->SetTitle("(#DeltaG/G)/L, fb"); 
    }
    TAxis *axis = graph_mean_values->GetXaxis();
    axis->Draw();
  
    for(int i=0; i<chamber_name.size(); i++){
       graph_mean_values->GetXaxis()->SetBinLabel(graph_mean_values->GetXaxis()->FindBin(i + 1.), chamber_name[i]); // Find out     which bin on the x-axis the point corresponds to and set the bin label
      graph_mean_values->GetXaxis()->SetLabelSize(0.040);
    }
   // graph_mean_values->GetXaxis()->SetTitleOffset(0.1); 
    //TLegend *legend_1 = new TLegend(0.63,0.7,0.9,0.9);
//    graph_mean_values->GetYaxis()->SetTitleSize(0.05);
//    graph_mean_values->GetYaxis()->SetLabelSize(0.05);
    graph_mean_values->GetXaxis()->SetLimits(0, size_chamber+1);

    TLegend *legend_1 = new TLegend(0.55,0.65,0.90,0.9);
    auto band = new TBox(0, -0.3, 33, 0.3);
    band->SetFillColorAlpha(kOrange-4, 0.25); // light translucent
    band->SetLineColor(0);                   // no border
    TCanvas *canv1 = new TCanvas();
    canv1->cd();
    canv1->SetLeftMargin(0.14);
    canv1->SetBottomMargin(0.14);
    //canv1->SetGrid();
    //gPad->SetGrid();
    graph_mean_values->Draw("AP");
    band->Draw("same");
    scaleLabel->DrawLatex(0.14, 0.92, "#times10^{-4}");
    graph_mean_values->SetTitle("");

    graph_mean_values->GetYaxis()->SetTitleSize(0.05);
    graph_mean_values->GetYaxis()->SetLabelSize(0.05);
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
    for (Int_t j=0; j<chamber_name.size(); j++) {
    std::cout<<" declaring gr x arrayr "<<std::endl;
        TMarker *m = new TMarker(gr_xarray[j], gr_yarray[j], 20);
        m->SetMarkerColor(marker_colour[chamber_name[j]]);
        m->Draw();

      std::cout<<"after declaring gr x arrayr "<<std::endl;
			if(j==0) {
				legend_1->AddEntry(m,"ME11a, ME11b (10#circ)","p");
			}
			if(j==3) {
			legend_1->AddEntry(m,"Outer Chambers (10#circ)","p"); }
			if(j==8)
			legend_1->AddEntry(m,"Inner Chambers (20#circ)","p");
   }
   std::cout<<" before drawing mulitgraph "<<std::endl; 
   legend_1->SetMargin(0.1);
   legend_1->SetTextSize(0.05);
   legend_1->SetTextAlign(12);

    legend_1->Draw("SAME");


   TLatex* cmslabel_1, *cmslabel_2;
   TLatex* text1,*text2;
   cmslabel_1 = new TLatex(0.16,0.82, "#bf{CMS} #it{Preliminary}");
   cmslabel_1->SetNDC(kTRUE);
   cmslabel_1->SetTextSize(0.06);
   cmslabel_1->SetTextFont(42);
   cmslabel_1->Draw("same");
   //cmslabel_2 = new TLatex(0.62,0.91,"159 fb^{-1} (13 TeV)");
   cmslabel_2 = new TLatex(0.62,0.91,"141 fb^{-1} (13 TeV)");
   cmslabel_2->SetNDC(kTRUE);
   cmslabel_2->SetTextSize(0.06);
   cmslabel_2->SetTextFont(42);
   cmslabel_2->Draw("same");

   canv1->SaveAs("AllResults/results_gas_gain_"+ratio_type+"/mean_slope_values_"+var+"_"+year+"_fit.pdf"); 
  
  TFile * outFile = new TFile(outputfileName,"UPDATE");
  outFile->cd();
  canv1->SetName("Mean_values_"+var+"_"+year+"_"+ratio_type);
  canv1->Write();
  outFile->Close();
  }

// Read gas gain in each individual year, and normalise the gain with respect to desired chambers or endcap
//  make ratio plots and comparison  plots, and combine 
void GasGain::reading_gas_gain_cumulative() {
    std::vector<float> mean_intlumi_run2_value_list;
    std::vector<float> mean_intlumi_run2_error_value_list;

    std::vector<TString> chambers = {"ME11a", "ME11b", 
                                     "ME12HV1",
                                     "ME12HV2",
                                      "ME12HV3",
                                     "ME13HV1", "ME13HV2", "ME13HV3", "ME21HV1", "ME21HV2",
                                     "ME21HV3", "ME22HV1", "ME22HV2", "ME22HV3", "ME22HV4",
                                     "ME22HV5", "ME31HV1", "ME31HV2", "ME31HV3", "ME32HV1",
                                     "ME32HV2",
                                     "ME32HV3", "ME32HV4", "ME32HV5", "ME41HV1",
                                     "ME41HV2", "ME41HV3", "ME42HV1", "ME42HV2", "ME42HV3",
                                     "ME42HV4", "ME42HV5"};

    std::map<TString, std::vector<float>> slopes, errors;
    auto [label_first, label_second] = this->getRatioLabels();

        std::vector<TGraphAsymmErrors*> histograms_var;
    for (const auto& chamber : chambers) {
        std::vector<TGraphAsymmErrors*> yearly_graphs;
        std::vector<TGraphAsymmErrors*> yearly_graphs_ref;
        std::vector<TGraphAsymmErrors*> yearly_graphs_orig;
        for (const auto& year : years) {
            TString filename = "../"+year + "_oddStrips/dataset_output_" + chamber + "_run2.root";
            TString filename_ref = "../"+year + "_evenStrips/dataset_output_" + chamber + "_run2.root";
            TFile* file = TFile::Open(filename);
            if (!file) continue;
            auto* dir = (TDirectoryFile*)file->Get("intlumi_final");
            auto* g_orig = (TGraphAsymmErrors*)dir->Get("dataset_trimmed_" + chamber + "_allgoodchannelsvs_intlumi_final_" + label_first);

            if (!g_orig) continue;
            std::cout<<" yes obtained the first graphs"<<std::endl;

            TFile* file_ref = TFile::Open(filename_ref);
            if (!file_ref) continue;
            auto* dir_ref = (TDirectoryFile*)file_ref->Get("intlumi_final");
            TString to_print  ="dataset_trimmed_" + chamber + "_allgoodchannelsvs_intlumi_final_" + label_second;
            std::cout<<" the output histogram "<<to_print<<std::endl;
            auto* g_ref = (TGraphAsymmErrors*)dir_ref->Get("dataset_trimmed_" + chamber + "_allgoodchannelsvs_intlumi_final_" + label_second);
            
            if (!g_ref || !g_orig) continue;
            std::cout<<" yes obtained the graphs"<<std::endl;

            this->normaliseGraph(g_orig, chamber, year, g_ref);
            yearly_graphs_orig.push_back(g_orig);
            yearly_graphs_ref.push_back(g_ref);
            auto [g_norm, g_norm_first] = this->normaliseAndFit(g_orig, chamber, year, g_ref);
            yearly_graphs.push_back(g_norm);

            auto [slope, err] = this->fitGraph(g_norm_first, chamber, year);
            slopes[year].push_back(slope);
            errors[year].push_back(err);
        }

        auto* combined_org = this->combiningGraphs(yearly_graphs_orig, chamber);
        auto* combined_ref = this->combiningGraphs(yearly_graphs_ref, chamber);
        this->normaliseGraph(combined_org, chamber, "run2", combined_ref);

        auto* combined = this->combiningGraphs(yearly_graphs, chamber);
        combined->SetTitle("Final Normalised Gas Gain: " + chamber);
        combined->GetYaxis()->SetRangeUser(0.9,1.1);
        this->DrawGraph(combined, "run2/plots_all/" , chamber);

        // Normalise combine histogram wrt first bin and fit too 
        auto *combined_normalised = this->NormalisingCombinedGraphs(combined);
        combined_normalised->GetYaxis()->SetRangeUser(0.95,1.05);
        combined_normalised->SetName("normalised_gas_gain_"+chamber);
        histograms_var.push_back(combined_normalised);

        std::pair<double, double> slope_mean_list;
        slope_mean_list = this->NormaliseFirstBin(combined_normalised, chamber);

        mean_intlumi_run2_value_list.push_back(slope_mean_list.first);
        mean_intlumi_run2_error_value_list.push_back(slope_mean_list.second);

    }
      TFile * outFile = new TFile(outputfileName,"RECREATE");
      outFile->cd();
      for(auto &hist : histograms_var){
          hist->Write();
      }
      outFile->Close();

//    for (const auto& year : years) {
//        drawMeanPlot(slopes[year], errors[year], "intlumi", chambers, year);
//    }
    drawMeanPlot(mean_intlumi_run2_value_list, mean_intlumi_run2_error_value_list, "intlumi", chambers, "run2");
}

void GasGain_CrossEndcap_OddEvenStrips(TString ratio_type) {
    TString outputfileName = "results_gas_gain_run2_"+ratio_type+".root" ;
    GasGain obj;
    obj.initialise_variable(ratio_type, outputfileName);
    obj.reading_gas_gain_cumulative();
}
