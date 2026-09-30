import os
current_path="/afs/cern.ch/work/n/nrawal/CSC_Run2_analysis/AnalysisCode/cumulative_plots/"
#year_period=["2016_test"]    
#year_period=["2016_full", "2017_full", "2018_full"]    
#year_period=["2016_full_new", "2017_full_new", "2018_full_new"]    
#year_period=["2016_pressure_corrected", "2017_pressure_corrected", "2018_pressure_corrected", "Run2_pressure_corrected"]    
#year_period=["2016_PV_complete_rang", "2017_PV_complete_rang", "2018_PV_complete_rang"]    
#year_period=["2016_PV_single_bin_second", "2017_PV_single_bin_second", "2018_PV_single_bin_second"]    
year_period=["2016_all_PV_corr_diff", "2017_all_PV_corr_diff", "2018_all_PV_corr_diff", "Run2_all_PV_corr_diff"]    
#year_period=["2016_all", "2017_all", "2018_all", "run2_all"]    
#year_period=["run2_without_correction_plots"]    
#year_period=["Run2_minus_uncertainty"]    
#year_period=["Run2_plus_uncertainty", "Run2_minus_uncertainty"]    
#year_period=["2016_rederived", "2017_rederived", "2018_rederived" ]    
#year_period=["run2_all_relaxed_hits_new","run2_all_new"]    
#year_period=["Run2_og_hits_nPV_newstyle"]    
#year_period=["2016_correct_HV_recuperated", "2017_correct_HV_recuperated", "2018_correct_HV_recuperated"]    
#year_period=["2016_without_correction", "2017_without_correction", "2018_without_correction"]    
#year_period=["2017_correct_HV",  "Run2_correct_HV", "2018_correct_HV"]    
#year_period=["Run2_oddLumiBlock", "Run2_evenLumiBlock"]    
#year_period=["Run2_oddStrips", "Run2_evenStrips"]    
chamber_list = ["ME11a", "ME11b", "ME12HV1", "ME12HV2","ME12HV3", "ME13HV1","ME13HV2","ME13HV3","ME21HV1","ME21HV2", "ME21HV3", "ME22HV1", "ME22HV2", "ME22HV3", "ME22HV4","ME22HV5","ME31HV1","ME31HV2", "ME31HV3", "ME32HV1", "ME32HV2", "ME32HV3", "ME32HV4","ME32HV5","ME41HV1","ME41HV2", "ME41HV3", "ME42HV1", "ME42HV2", "ME42HV3", "ME42HV4","ME42HV5"]
#chamber_list = ["ME11b"]
#chamber_list = [ "ME12HV1", "ME21HV1"]
for year_period_num in year_period: 
    os.chdir(f"{current_path}")
    os.system(f"mkdir {year_period_num}")
    os.chdir(f"{year_period_num}")
    os.system("mkdir output_plots")
    os.chdir("output_plots")
    os.chdir("../")
    os.system("mkdir all_channels")
    os.chdir("all_channels")
    for chamber in chamber_list : 
        os.system(f"mkdir {chamber}")
        os.chdir(f"{chamber}")
        os.system(f"mkdir timesecond_initial")
        os.system(f"mkdir timesecond_final")
        os.system(f"mkdir pressure")
        os.system(f"mkdir pressure_second")
        os.system(f"mkdir instlumi")
        os.system(f"mkdir instlumi_second")
        os.system(f"mkdir intlumi_initial")
        os.system(f"mkdir intlumi_final")
        os.chdir("../")
