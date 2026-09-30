#include "tree_class.h"
void TreeStructure::initialise(TString input_file_path, TString output_file_path){
 
  std::cout<<" entered in intiialisation "<<std::endl;
  f = TFile::Open(input_file_path,"READ");
  tree= (TTree*) f->Get("tree");

  f_out = TFile::Open(output_file_path, "RECREATE");
  tree_new= new TTree("tree", " a tree");

  std::cout<<" entries in tree "<<tree->GetEntries()<<std::endl;
//   tree->SetBranchAddress("_passZmumusel", &old_passZmumusel, &b_old_passZmumusel);
//   tree->SetBranchAddress("_passisomuondzdxy", &old_passisomuondzdxy, &b_old_passisomuondzdxy);
   tree->SetBranchAddress("_eventNb", &old_eventNb, &b_old_eventNb);
   tree->SetBranchAddress("_n_PV", &old_n_PV, &b_old_n_PV);
   tree->SetBranchAddress("_runNb", &old_runNb, &b_old_runNb);
   tree->SetBranchAddress("_lumiBlock", &old_lumiBlock, &b_old_lumiBlock);
   tree->SetBranchAddress("_nearestStrip", &old_nearestStrip, &b_old_nearestStrip);
   tree->SetBranchAddress("_rhid", &old_rhid, &b_old_rhid);
   tree->SetBranchAddress("_stationring", &old_stationring, &b_old_stationring);
   tree->SetBranchAddress("_rhsumQ", &old_rhsumQ, &b_old_rhsumQ);
   tree->SetBranchAddress("_rhsumQ_RAW", &old_rhsumQ_RAW, &b_old_rhsumQ_RAW);
   tree->SetBranchAddress("_rhsumQ_equalised_HV_data", &old_rhsumQ_equalised_HV_data, &b_old_rhsumQ_equalised_HV_data);
   tree->SetBranchAddress("_HV", &old_HV, &b_old_HV);
   tree->SetBranchAddress("_HV_nominal", &old_HV_nominal, &b_old_HV_nominal);
//   tree->SetBranchAddress("_current", &old_current, &b_old_current);
   tree->SetBranchAddress("_pressure", &old_pressure, &b_old_pressure);
//   tree->SetBranchAddress("_temperature", &old_temperature, &b_old_temperature);
   tree->SetBranchAddress("_instlumi", &old_instlumi, &b_old_instlumi);
   tree->SetBranchAddress("_integratelumi", &old_integratelumi, &b_old_integratelumi);
   tree->SetBranchAddress("_instlumi_goldenjson", &old_instlumi_goldenjson, &b_old_instlumi_goldenjson);
   tree->SetBranchAddress("_instlumi_dcsjson", &old_instlumi_dcsjson, &b_old_instlumi_dcsjson);
   tree->SetBranchAddress("_intlumi_delivered_dcsjson", &old_intlumi_delivered_dcsjson, &b_old_intlumi_delivered_dcsjson);
   tree->SetBranchAddress("_intlumi_delivered_goldenjson", &old_intlumi_delivered_goldenjson, &b_old_intlumi_delivered_goldenjson);
   tree->SetBranchAddress("_intlumi_recorded_dcsjson", &old_intlumi_recorded_dcsjson, &b_old_intlumi_recorded_dcsjson);
   tree->SetBranchAddress("_intlumi_recorded_goldenjson", &old_intlumi_recorded_goldenjson, &b_old_intlumi_recorded_goldenjson);
   tree->SetBranchAddress("_timesecond", &old_timesecond, &b_old_timesecond);
//   tree->SetBranchAddress("_n_PV", &old_n_PV, &b_old_n_PV);
   tree->SetBranchAddress("_bunchcrossing", &old_bunchcrossing, &b_old_bunchcrossing);
   tree->SetBranchAddress("_etamuon", &old_etamuon, &b_old_etamuon);
   tree->SetBranchAddress("_phimuon", &old_phimuon, &b_old_phimuon);
   tree->SetBranchAddress("_ptmuon", &old_ptmuon, &b_old_ptmuon);
//   tree->SetBranchAddress("z_pt", &old_z_pt, &b_old_z_pt);
//   tree->SetBranchAddress("z_eta", &old_z_eta, &b_old_z_eta);
//   tree->SetBranchAddress("z_phi", &old_z_phi, &b_old_z_phi);
//   tree->SetBranchAddress("z_mass", &old_z_mass, &b_old_z_mass);
//
//   tree->SetBranchAddress("iso_PF_first", &old_iso_PF_first, &b_old_iso_PF_first);
//   tree->SetBranchAddress("iso_PF_second", &old_iso_PF_second, &b_old_iso_PF_second);
//   tree->SetBranchAddress("isolation1", &old_isolation1, &b_old_isolation1);
//   tree->SetBranchAddress("isolation2", &old_isolation2, &b_old_isolation2);


}

void TreeStructure :: Setup_new_tree(){
//  tree_new->Branch("_passZmumusel", &new_passZmumusel, "new_passZmumusel/O");
//  tree_new->Branch("_passisomuondzdxy", &new_passisomuondzdxy, "new_passisomuondzdxy/O");
  tree_new->Branch("_eventNb",&new_eventNb, "new_eventNb/L");
  tree_new->Branch("_n_PV",&new_n_PV, "new_n_PV/I");
  tree_new->Branch("_runNb",&new_runNb, "new_runNb/L");
  tree_new->Branch("_lumiBlock",&new_lumiBlock, "new_lumiBlock/L");
  tree_new->Branch("_nearestStrip", &new_nearestStrip, "new_nearestStrip/L");
  tree_new->Branch("_rhid",&new_rhid, "new_rhid/L");
  tree_new->Branch("_stationring",&new_stationring, "new_stationring/L");
  tree_new->Branch("_rhsumQ",&new_rhsumQ , "new_rhsumQ/D");
  tree_new->Branch("_rhsumQ_equalised_HV_data",&new_rhsumQ_equalised_HV_data , "new_rhsumQ_equalised_HV_data/D");
  tree_new->Branch("_rhsumQ_RAW", &new_rhsumQ_RAW, "new_rhsumQ_RAW/D");
  tree_new->Branch("_HV", &new_HV, "new_HV/D");
  tree_new->Branch("_HV_nominal", &new_HV_nominal, "new_HV_nominal/D");
//  tree_new->Branch("_current", &new_current, "new_current/D");
  tree_new->Branch("_pressure", &new_pressure, "new_pressure/D");
//  tree_new->Branch("_temperature", &new_temperature, "new_temperature/D");
  tree_new->Branch("_instlumi", &new_instlumi, "new_instlumi/D");
  tree_new->Branch("_integratelumi", &new_integratelumi, "new_integratelumi/D");
  tree_new->Branch("_instlumi_goldenjson", &new_instlumi_goldenjson, "new_instlumi_goldenjson/D");
  tree_new->Branch("_instlumi_dcsjson", &new_instlumi_dcsjson, "new_instlumi_dcsjson/D");
  tree_new->Branch("_intlumi_delivered_dcsjson", &new_intlumi_delivered_dcsjson, "new_intlumi_delivered_dcsjson/D");
  tree_new->Branch("_intlumi_delivered_goldenjson", &new_intlumi_delivered_goldenjson, "new_intlumi_delivered_goldenjson/D");
  tree_new->Branch("_intlumi_recorded_dcsjson", &new_intlumi_recorded_dcsjson, "new_intlumi_recorded_dcsjson/D");
  tree_new->Branch("_intlumi_recorded_goldenjson", &new_intlumi_recorded_goldenjson, "new_intlumi_recorded_goldenjson/D");
  tree_new->Branch("_timesecond",&new_timesecond, "new_timesecond/L");
//  tree_new->Branch("_n_PV", &new_n_PV, "new_n_PV/I");
  tree_new->Branch("_bunchcrossing", &new_bunchcrossing, "new_bunchcrossing/L");

  tree_new->Branch("_etamuon", &new_etamuon, "new_etamuon/D");
  tree_new->Branch("_phimuon", &new_phimuon, "new_phimuon/D");
  tree_new->Branch("_ptmuon", &new_ptmuon, "new_ptmuon/D");
//  tree_new->Branch("z_pt", &new_z_pt, "new_z_pt/D");
//  tree_new->Branch("z_eta", &new_z_eta, "new_z_eta/D");
//  tree_new->Branch("z_phi", &new_z_phi, "new_z_phi/D");
//  tree_new->Branch("z_mass", &new_z_mass, "new_z_mass/D");
//
//  tree_new->Branch("iso_PF_first", &new_iso_PF_first, "new_iso_PF_first/D");
//  tree_new->Branch("iso_PF_second", &new_iso_PF_second, "new_iso_PF_second/D");
//  tree_new->Branch("isolation1", &new_isolation1, "new_isolation1/D");
//  tree_new->Branch("isolation2", &new_isolation2, "new_isolation2/D");

}
void TreeStructure::WriteNew(){

  f_out->cd();
  tree_new->Write();
  f_out->Close();
}
TreeStructure::~TreeStructure(){
   if ( !f) return;
   delete f;
}
