import os
path="/afs/cern.ch/work/n/nrawal/CSC_Run2_analysis/AnalysisCode/cumulative_plots/Final_analysis/Results/"
#type_channels = ["results_gas_gain_all_normalised_ME13HV3_Run2_PV_corr"] 
#type_channels = ["results_gas_gain_all_normalised_ME13HV3_2018_PV_without_instlumi_normalised", 
#"results_gas_gain_all_normalised_ME13HV3_2017_instlumi_normal_PV_normalised", 
#"results_gas_gain_all_normalised_ME13HV3_2016_instlumi_normal_PV_normalised"
#]
#"results_gas_gain_all_normalised_ME13HV3_Run2_instlumi"] 
type_channels = [#"results_gas_gain_all_normalised_ME13HV3_Run2_removing_bad_channels_again" , 
"results_gas_gain_odd_even_lumiBlock"
]
#"results_gas_gain_all_normalised_ME13HV3_2016_PV_without_instlumi_normalised", 
#"results_gas_gain_all_normalised_ME13HV3_2017_PV_without_instlumi_normalised", 
#"results_gas_gain_all_normalised_ME13HV3_2018_PV_without_instlumi_normalised"]
#"results_gas_gain_all_normalised_ME13HV3_2016_instlumi_normalised", 
#"results_gas_gain_all_normalised_ME13HV3_2017_instlumi_normalised", 
#"results_gas_gain_all_normalised_ME13HV3_2018_instlumi_normalised"] 
#type_channels = ["output_2016_complete_range_bx", "output_2017_complete_range_bx", "output_2018_complete_range_bx", 
#"output_2016_complete_range", "output_2017_complete_range", "output_2018_complete_range", 
#"output_2016_single_bin_first_bx", "output_2017_single_bin_first_bx", "output_2018_single_bin_first_bx", 
#"output_2016_single_bin_second_bx", "output_2017_single_bin_second_bx", "output_2018_single_bin_second_bx", 
#"output_2016_single_bin_third_bx", "output_2017_single_bin_third_bx", "output_2018_single_bin_third_bx", 
#"output_2016_single_bin_first", "output_2017_single_bin_first", "output_2018_single_bin_first", 
#"output_2016_single_bin_second", "output_2017_single_bin_second", "output_2018_single_bin_second", 
#"output_2016_single_bin_third", "output_2017_single_bin_third", "output_2018_single_bin_third" 
#] #, 
#"results_gas_gain_all_normalised_ME13HV3_2017_n_PV_complete_range_bx", 
#"results_gas_gain_all_normalised_ME13HV3_2018_n_PV_complete_range_bx"
#]
for types in type_channels :
    print(f" type {types}")
    os.chdir(f"{path}")
    os.system(f"mkdir {types}")
    os.chdir(f"./{types}")
    os.system("mkdir intlumi")
    os.system("mkdir pressure")
    os.system("mkdir instlumi")
    os.system("mkdir PV")
   
    os.chdir("pressure")
    os.system("mkdir overlapping")
    os.system("mkdir 2016")
    os.system("mkdir 2017")
    os.system("mkdir 2018")
    os.system("mkdir run2")
    os.chdir("2016")
    os.system("mkdir plots_all")
    os.system("mkdir fits")
    os.chdir("../2017")
    os.system("mkdir plots_all")
    os.system("mkdir fits")
    os.chdir("../2018")
    os.system("mkdir plots_all")
    os.system("mkdir fits_all")
    os.chdir("../run2")
    os.system("mkdir fits_all")
    os.system("mkdir fits")
    os.system("mkdir plots_all")
   
    os.chdir("../../instlumi")
    os.system("mkdir overlapping")
    os.system("mkdir 2016")
    os.system("mkdir 2017")
    os.system("mkdir 2018")
    os.system("mkdir run2")
    os.chdir("2016")
    os.system("mkdir plots_all")
    os.system("mkdir fits")
    os.chdir("../2017")
    os.system("mkdir plots_all")
    os.system("mkdir fits")
    os.chdir("../2018")
    os.system("mkdir plots_all")
    os.system("mkdir fits")
    os.chdir("../run2")
    os.system("mkdir fits_all")
    os.system("mkdir fits")
    os.system("mkdir plots_all")
 
    os.chdir("../../PV")
    os.system("mkdir overlapping")
    os.system("mkdir 2016")
    os.system("mkdir 2017")
    os.system("mkdir 2018")
    os.system("mkdir run2")
    os.chdir("2016")
    os.system("mkdir plots_all")
    os.system("mkdir fits")
    os.chdir("../2017")
    os.system("mkdir plots_all")
    os.system("mkdir fits")
    os.chdir("../2018")
    os.system("mkdir plots_all")
    os.system("mkdir fits_all")
    os.chdir("../run2")
    os.system("mkdir fits_all")
    os.system("mkdir fits")
    os.system("mkdir plots_all")
   
    os.chdir("../../intlumi")
    os.system("mkdir overlapping")
    os.system("mkdir 2016")
    os.system("mkdir 2017")
    os.system("mkdir 2018")
    os.system("mkdir run2")
    os.system("mkdir all")
    os.chdir("2016")
    os.system("mkdir fits")
    os.system("mkdir plots_all")
    os.chdir("../2017")
    os.system("mkdir fits")
    os.system("mkdir plots_all")
    os.chdir("../2018")
    os.system("mkdir fits")
    os.system("mkdir plots_all")
    os.chdir("../run2")
    os.system("mkdir fits_all")
    os.system("mkdir fits")
    os.system("mkdir plots_all")
    os.chdir("../all")
    os.system("mkdir fits_all")
    os.system("mkdir fits")
    os.system("mkdir plots_all")

