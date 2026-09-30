void reading_fill(){
  TFile * file = TFile::Open("/eos/home-n/nrawal/CSCAgeing/Run2_combine_new_selections/2016_updated/csc_output_2016_ME11a_tree_HVupdated.root","READ");
  TTree * tree = (TTree*) file->Get("tree");
  double _instlumi;
  ULong64_t _runNb;
 UInt_t _timesecond;

 UInt_t time_old, time_end;
 ULong64_t run_old;
 double instlumi_start, instlumi_end;
  tree->SetBranchAddress("_instlumi", &_instlumi); 
  tree->SetBranchAddress("_runNb", &_runNb); 
  tree->SetBranchAddress("_timesecond", &_timesecond); 

  int entries = 0;
  TGraph * graph = new TGraph();
  bool first=true;
  for(int i=0 ; i<tree->GetEntries(); i++){
  //for(int i=0 ; i<10000; i++){
    tree->GetEntry(i);

    if(_runNb >=275809 && _runNb<=275848){
      entries++;
      graph->SetPoint(i, _timesecond, _instlumi);
      if(first) {time_old =_timesecond; first=false; run_old=_runNb;
          std::cout<<" initial time "<<time_old<<" run "<<run_old<<std::endl;
          time_end = _timesecond;  
          instlumi_start = _instlumi;
          instlumi_end = _instlumi;
         }
      if(_timesecond < time_old ){
        time_old = _timesecond;
        run_old = _runNb;
        instlumi_start = _instlumi;
      }
      if(_timesecond > time_end){
         time_end=  _timesecond;
          instlumi_end = _instlumi;
      }
    }
  }
  std::cout<<" initial time "<<time_old<<" run old "<<run_old<<" final Time "<<time_end<<std::endl;
  std::cout<<" inst start time "<<instlumi_start<<" instlumi end "<<instlumi_end<<std::endl;
  graph->SetTitle("Instlumi Fill 5094");
  graph->GetXaxis()->SetTitle("Day-Time");
  graph->GetYaxis()->SetTitle("Instlumi (*10^33) cm^{-2}s^{-1}");
  graph->GetXaxis()->SetTimeFormat("%d-%H:%M");
  graph->GetXaxis()->SetTimeDisplay(1);
  graph->GetXaxis()->SetRangeUser(time_old-3600, time_end+3600);
  graph->GetXaxis()->SetTimeOffset(0,"gmt");
  TCanvas * c= new TCanvas();
  c->cd();
  graph->Draw("AP");
  graph->SetMarkerColor(kBlue);
  c->SaveAs("instlumi_fill_5094.pdf");
  std::cout<<" entries "<<entries<<std::endl;
}
