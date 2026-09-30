#ifndef TREE_STRUCTURE_H
#define TREE_STRUCTURE_H
#include <iostream>
#include "TTree.h"
#include "TFile.h"
using namespace std;
//bool isDY=false;
//bool isJpsi = true;
class TreeStructure{
   private :
    TFile * f;
    TFile * f_out;
    public : 
    TTree          *tree;   //!pointer to the analyzed TTree or TChain
    TTree          *tree_new;   //!pointer to the analyzed TTree or TChain

   Long64_t        new_eventNb;
   Long64_t        new_n_PV;
   Long64_t        new_runNb;
   Long64_t        new_lumiBlock;
   Long64_t        new_rhid;
   Long64_t        new_nearestStrip;
   Long64_t        new_stationring;
   Long64_t        new_timesecond;
   Long64_t        new_bunchcrossing;
   Double_t        new_rhsumQ;
   Double_t        new_rhsumQ_RAW;
   Double_t        new_rhsumQ_equalised_HV_data;
   Double_t        new_HV;
   Double_t        new_HV_nominal;
   Double_t        new_pressure;
   Double_t        new_instlumi;
   Double_t        new_instlumi_goldenjson;
   Double_t        new_instlumi_dcsjson;
   Double_t        new_integratelumi;
   Double_t        new_intlumi_delivered_dcsjson;
   Double_t        new_intlumi_recorded_dcsjson;
   Double_t        new_intlumi_delivered_goldenjson;
   Double_t        new_intlumi_recorded_goldenjson;
   Double_t        new_etamuon;
   Double_t        new_phimuon;
   Double_t        new_ptmuon;

   Long64_t        old_eventNb;
   Long64_t        old_n_PV;
   Long64_t        old_runNb;
   Long64_t        old_lumiBlock;
   Long64_t        old_rhid;
   Long64_t        old_nearestStrip;
   Long64_t        old_stationring;
   Long64_t        old_timesecond;
   Long64_t        old_bunchcrossing;
   Double_t        old_rhsumQ;
   Double_t        old_rhsumQ_RAW;
   Double_t        old_rhsumQ_equalised_HV_data;
   Double_t        old_HV;
   Double_t        old_HV_nominal;
   Double_t        old_pressure;
   Double_t        old_instlumi;
   Double_t        old_instlumi_goldenjson;
   Double_t        old_instlumi_dcsjson;
   Double_t        old_integratelumi;
   Double_t        old_intlumi_delivered_dcsjson;
   Double_t        old_intlumi_recorded_dcsjson;
   Double_t        old_intlumi_delivered_goldenjson;
   Double_t        old_intlumi_recorded_goldenjson;
   Double_t        old_etamuon;
   Double_t        old_phimuon;
   Double_t        old_ptmuon;


////   //Long64_t        nan;
////   //
////   Bool_t          new_passZmumusel;
////   Bool_t          new_passisomuondzdxy;
////   ULong64_t       new_eventNb;
////   ULong64_t       new_runNb;
////   ULong64_t       new_lumiBlock;
////   Int_t           new_rhid;
////   Int_t           new_stationring;
////   Double_t        new_rhsumQ;
////   Double_t        new_rhsumQ_RAW;
////   Double_t        new_rhsumQ_equalised_HV_data;
////   Double_t        new_HV;
////   Double_t        new_HV_nominal;
////   Double_t        new_current;
////   Double_t        new_pressure;
////   Double_t        new_temperature;
////   Double_t        new_instlumi;
////   Double_t        new_integratelumi;
////   UInt_t          new_timesecond;
////   Int_t           new_n_PV;
////   Int_t           new_bunchcrossing;
////   Double_t        new_etamuon;
////   Double_t        new_phimuon;
////   Double_t        new_ptmuon;
////   Double_t        new_z_pt;
////   Double_t        new_z_eta;
////   Double_t        new_z_phi;
////   Double_t        new_z_mass;
////   Long64_t        new_dpid;
//////   Long64_t        new_iso_PF_first;
//////   Long64_t        new_iso_PF_second;
//////   Long64_t        new_isolation1;
//////   Long64_t        new_isolation2;
////   Double_t        new_iso_PF_first;
////   Double_t        new_iso_PF_second;
////   Double_t        new_isolation1;
////   Double_t        new_isolation2;
////
////
//// 
////   Bool_t          old_passZmumusel;
////   Bool_t          old_passisomuondzdxy;
////   ULong64_t       old_eventNb;
////   ULong64_t       old_runNb;
////   ULong64_t       old_lumiBlock;
////   Int_t           old_rhid;
////   Int_t           old_stationring;
////   Double_t        old_rhsumQ;
////   Double_t        old_rhsumQ_equalised_HV_data;
////   Double_t        old_rhsumQ_RAW;
////   Double_t        old_HV;
////   Double_t        old_HV_nominal;
////   Double_t        old_current;
////   Double_t        old_pressure;
////   Double_t        old_temperature;
////   Double_t        old_instlumi;
////   Double_t        old_integratelumi;
////   UInt_t          old_timesecond;
////   Int_t           old_n_PV;
////   Int_t           old_bunchcrossing;
////   Double_t        old_etamuon;
////   Double_t        old_phimuon;
////   Double_t        old_ptmuon;
////   Double_t        old_z_pt;
////   Double_t        old_z_eta;
////   Double_t        old_z_phi;
////   Double_t        old_z_mass;
//////   Long64_t        old_iso_PF_first;
//////   Long64_t        old_iso_PF_second;
//////   Long64_t        old_isolation1;
//////   Long64_t        old_isolation2;
////   Double_t        old_iso_PF_first;
////   Double_t        old_iso_PF_second;
////   Double_t        old_isolation1;
////   Double_t        old_isolation2;
////

   // List of branches
   TBranch        *b_old_passZmumusel;   //!
   TBranch        *b_old_passisomuondzdxy;   //!
   TBranch        *b_old_eventNb;   //!
   TBranch        *b_old_n_PV;   //!
   TBranch        *b_old_runNb;   //!
   TBranch        *b_old_lumiBlock;   //!
   TBranch        *b_old_nearestStrip;   //!
   TBranch        *b_old_rhid;   //!
   TBranch        *b_old_stationring;   //!
   TBranch        *b_old_rhsumQ;   //!
   TBranch        *b_old_rhsumQ_equalised_HV_data;   //!
   TBranch        *b_old_rhsumQ_RAW;   //!
   TBranch        *b_old_HV;   //!
   TBranch        *b_old_HV_nominal;   //!
   TBranch        *b_old_current;   //!
   TBranch        *b_old_pressure;   //!
   TBranch        *b_old_temperature;   //!
   TBranch        *b_old_instlumi;   //!
   TBranch        *b_old_instlumi_goldenjson;   //!
   TBranch        *b_old_instlumi_dcsjson;   //!
   TBranch        *b_old_integratelumi;   //!
   TBranch        *b_old_intlumi_delivered_dcsjson;   //!
   TBranch        *b_old_intlumi_recorded_dcsjson;   //!
   TBranch        *b_old_intlumi_delivered_goldenjson;   //!
   TBranch        *b_old_intlumi_recorded_goldenjson;   //!
   TBranch        *b_old_timesecond;   //!
   TBranch        *b_old_bunchcrossing;   //!
   TBranch        *b_old_etamuon;   //!
   TBranch        *b_old_phimuon;   //!
   TBranch        *b_old_ptmuon;   //!
   TBranch        *b_old_z_pt;   //!
   TBranch        *b_old_z_eta;   //!
   TBranch        *b_old_z_phi;   //!
   TBranch        *b_old_z_mass;   //!
   TBranch        *b_old_iso_PF_first;
   TBranch        *b_old_iso_PF_second;
   TBranch        *b_old_isolation1;
   TBranch        *b_old_isolation2;

 
   void initialise(TString, TString);
   void Setup_new_tree();
   void WriteNew();
   ~TreeStructure();
}; 
#endif
