import uproot
import numpy as np
import pandas as pd
import argparse
import ROOT
from datetime import datetime

def read_lumi_file_golden(year):
    #input_file_name = f"../../Brilcalc_evaluating_luminosity/testing_prescales/{year}lumi_HLTIsoMu24_byls.csv"
    input_file_name = f"../../Brilcalc_evaluating_luminosity/{year}_lumibyls.csv"
    if(year=="2016" or year=="2018"):
        nb_footer = 5
    elif(year=="2017"):
        nb_footer=5
    df = pd.read_csv(input_file_name, sep=',',skiprows=1, skipfooter=nb_footer, engine='python')  # required when using skipfooter)
    return df
def read_lumi_file_dcs(year):
    #input_file_name = f"/afs/cern.ch/work/n/nrawal/Brilcal_new_env/Run2_UTC_lumi_evaluation/testing_prescales/{year}lumi_HLTIsoMu24_byls_dcsonly.csv"
    #input_file_name = f"../../Brilcalc_evaluating_luminosity/testing_prescales/{year}lumi_HLTIsoMu24_byls_dcsonly.csv"
    input_file_name = f"../../Brilcalc_evaluating_luminosity/{year}_lumibyls_DCSONLY.csv"
    if(year=="2016" or year=="2018"):
        nb_footer = 5
    elif(year=="2017"):
        nb_footer=5
    df = pd.read_csv(input_file_name, sep=',',skiprows=1, skipfooter=nb_footer, engine='python')  # required when using skipfooter)
    return df

def process_lumi_file(df):
    # Step 2: Remove the hashtag from '#run:fill' if it's there
    df.columns = [col.lstrip('#') for col in df.columns]
    # Step 3: Split 'run:fill' into 'run' and 'fill'
    df[['run', 'fill']] = df['run:fill'].str.split(':', expand=True)
    # First, attempt to split the column into two parts
    ls_split = df['ls'].str.split(':', expand=True)
    
    # Convert the first part to numeric (errors='coerce' will turn non-numeric into NaN)
    first_part = pd.to_numeric(ls_split[0], errors='coerce')
    second_part = pd.to_numeric(ls_split[1], errors='coerce')
    
    # Use second part only if first is NaN or 0
    df['ls_value'] = first_part.where((first_part.notna()) & (first_part != 0), second_part)
    
    # Optionally convert ls to integer if clean
    df['ls_value'] = df['ls_value'].astype('uint64')  # or 'int' if you're sure there's no NaN
    df['run'] = df['run'].astype('uint64')  # or 'int' if you're sure there's no NaN
    # Optional: Drop the original 'run:fill' column if no longer needed
    df = df.drop(columns=['run:fill','time','E(GeV)', 'beamstatus', 'avgpu', 'source','ls'])
    df['delivered(/ub)'] = pd.to_numeric(df['delivered(/ub)'], errors='coerce')
    df['recorded(/ub)'] = pd.to_numeric(df['recorded(/ub)'], errors='coerce')
    # Create cumulative sum columns
    df['_instlumi_delivered'] = df['delivered(/ub)']/23.31
    df['int_delivered(/ub)'] = df['delivered(/ub)'].cumsum()
    df['int_recorded(/ub)'] = df['recorded(/ub)'].cumsum()
    #print(" ************* df columns **********", df.columns)

    return df

def MergingLumi_golden(df_root, df_Lumi) : 
    # rename the HV read column so to merge with the current ntuple 
    df_renamed = df_Lumi.rename(columns={"run": "_runNb", "ls_value": "_lumiBlock"})
    #print(df_root[['_runNb', '_lumiBlock']].dtypes)
    #print(df_renamed[['_runNb', '_lumiBlock']].dtypes)
    #print(" after masking for particlular rhid : the dataframe ")
    #print("len of dataframe", df_root)
    merged = pd.merge(df_root, df_renamed, on=["_runNb", "_lumiBlock"], how="left")

    merged['int_delivered(/fb)'] = merged['int_delivered(/ub)']/(10**9)
    merged['int_recorded(/fb)'] = merged['int_recorded(/ub)']/(10**9)
    merged.drop(columns=['delivered(/ub)', 'recorded(/ub)', 'int_recorded(/ub)', 'int_delivered(/ub)','fill'], inplace=True)
    
    #print("merged columns ", merged.columns)
    merged.rename(columns={"int_delivered(/fb)" : "_intlumi_delivered_goldenjson"}, inplace=True)
    merged.rename(columns={"int_recorded(/fb)" : "_intlumi_recorded_goldenjson"}, inplace=True)
    merged.rename(columns={"_instlumi_delivered" : "_instlumi_goldenjson"}, inplace=True)
    filtered = merged

    print("length after merging and filtereing", len(filtered))  
    #filtered.drop(columns=["Starttime", "Endtime", "start_epoch", "end_epoch"], inplace=True, errors="ignore")
    # Keep a version of filtered aligned to df_root (drop HVvalue, etc.)
    filtered_aligned = filtered[df_root.columns]
    #print(" *******df root columns ***** ", df_root.columns)  
   # print(" ******merged  columns ***** ", filtered.columns)  

    # Get unmatched rows by subtracting
    unmatched = pd.concat([df_root, filtered_aligned]).drop_duplicates(keep=False)

    print("Unmatched length:", len(unmatched))
    print("Unmatched dataframe")
    print(unmatched)
    # Tag
    filtered["lumifound"] = True
    unmatched = unmatched.copy()
    unmatched["lumifound"] = False
    unmatched["lumivalue"] = np.nan  # Add HVvalue column for compatibility

    return filtered, unmatched 


def MergingLumi_dcs(df_root, df_Lumi) : 
    # rename the HV read column so to merge with the current ntuple 
    df_renamed = df_Lumi.rename(columns={"run": "_runNb", "ls_value": "_lumiBlock"})
    #print(df_root[['_runNb', '_lumiBlock']].dtypes)
    #print(df_renamed[['_runNb', '_lumiBlock']].dtypes)
    #print(" after masking for particlular rhid : the dataframe ")
    #print("len of dataframe", df_root)
    merged = pd.merge(df_root, df_renamed, on=["_runNb", "_lumiBlock"], how="left")

    merged['int_delivered(/fb)'] = merged['int_delivered(/ub)']/(10**9)
    merged['int_recorded(/fb)'] = merged['int_recorded(/ub)']/(10**9)
    merged.drop(columns=['delivered(/ub)', 'recorded(/ub)', 'int_recorded(/ub)', 'int_delivered(/ub)','fill'], inplace=True)
    
    #print("merged columns ", merged.columns)
    merged.rename(columns={"int_delivered(/fb)" : "_intlumi_delivered_dcsjson"}, inplace=True)
    merged.rename(columns={"int_recorded(/fb)" : "_intlumi_recorded_dcsjson"}, inplace=True)
    merged.rename(columns={"_instlumi_delivered" : "_instlumi_dcsjson"}, inplace=True)
    filtered = merged
    #print("length after merging and filtereing", len(filtered))  
    #print(" *******df root columns ***** ", df_root.columns)  
    #print(" ******merged  columns ***** ", filtered.columns)  
    #filtered.drop(columns=["Starttime", "Endtime", "start_epoch", "end_epoch"], inplace=True, errors="ignore")
    # Keep a version of filtered aligned to df_root (drop HVvalue, etc.)
    filtered_aligned = filtered[df_root.columns]
    # Get unmatched rows by subtracting
    unmatched = pd.concat([df_root, filtered_aligned]).drop_duplicates(keep=False)

    print("Unmatched length:", len(unmatched))
    print("Unmatched dataframe")
    print(unmatched)
    # Tag
    filtered["lumifound"] = True
    unmatched = unmatched.copy()
    unmatched["lumifound"] = False
    unmatched["lumivalue"] = np.nan  # Add HVvalue column for compatibility

    return filtered, unmatched 

def Savecsv(df, csv_file):
    df.to_csv(csv_file+".csv", index=False)
def saveFile(df, chamber, year) :
    #csv_file = "/afs/cern.ch/user/n/nrawal/eos/CSCAgeing/Run2_Ntuples/"+year+"_updated/csc_output_"+year+"_"+chamber+"_tree_updated"
    csv_file = "/afs/cern.ch/user/n/nrawal/eos/CSCAgeing/Run2_Ntuples_complete_relaxed/"+year+"_updated/csc_output_"+year+"_"+chamber+"_tree_updated"
    #csv_file = "/afs/cern.ch/user/n/nrawal/eos/CSCAgeing/Run2_Ntuples_with_relaxed_hit_per_layer/"+year+"_updated/csc_output_"+year+"_"+chamber+"_tree_updated"
    Savecsv(df, csv_file)
    df_RDF = ROOT.RDF.FromCSV(csv_file+".csv")
    print("RDF :",df_RDF)
    # Snapshot it to a ROOT file
    output_file = csv_file+".root"
    df_RDF.Snapshot("tree", output_file)



def read_root_file(input_name):
    file = uproot.open(input_name)
    tree = file["tree"]  # or your actual TTree name
    df_root = tree.arrays([ "_eventNb", "_rhid", "_runNb", "_lumiBlock", "_timesecond",
        "_stationring", "_nearestStrip", "_rhsumQ", "_rhsumQ_RAW", "_HV",
         "_pressure", "_instlumi", "_integratelumi", "_bunchcrossing", 
        "_etamuon", "_phimuon", "_ptmuon", "_n_PV"
        ], library="pd")  # event_ti
#    df_root = tree.arrays([ "_eventNb", "_rhid", "_runNb", "_lumiBlock", "_timesecond",
#        "_stationring", "_nearestStrip", "_nearestWireGroup", "_rhsumQ", "_rhsumQ_RAW", "_HV", 
#         "_pressure", "_instlumi", "_integratelumi", "_bunchcrossing", 
#        "_etamuon", "_phimuon", "_ptmuon"
#        ], library="pd", entry_start =0, entry_stop = 1000)  # event_ti

    df_root["_HV"] = df_root["_HV"].astype("float64")
    return df_root

def separating_plus_minus(df_root) :
    # Convert rhid to string and check first character
    df_root_plus = df_root[df_root['_rhid'].astype(str).str.startswith('1')]
    df_root_minus = df_root[df_root['_rhid'].astype(str).str.startswith('2')]
    df_root_plus['_rhid'] = df_root_plus['_rhid'].astype(int)
    df_root_minus['_rhid'] = df_root_minus['_rhid'].astype(int)
    return df_root_plus, df_root_minus

def reading_HV_file(input_file) :
    #df = pd.read_csv(input_file, delim_whitespace=True)
    df = pd.read_csv(input_file, sep='\s')
    # Combine the "Start" and "time" columns into a single "Starttime" column
    df['Starttime'] = df['Start'] + ' ' + df['time']
    # Combine the "End" and "time" columns into a single "Endtime" column
    df['Endtime'] = df['End'] + ' ' + df['time.1']
    # Drop the original "Start", "time", "End", and "time.1" columns
    df.drop(columns=['Start', 'time', 'End', 'time.1'], inplace=True)

    # Convert to datetime
    df["Starttime"] = pd.to_datetime(df["Starttime"], format="%Y-%m-%d %H:%M:%S")
    df["Endtime"] = pd.to_datetime(df["Endtime"], format="%Y-%m-%d %H:%M:%S")

    # Convert datetime to epoch time (in seconds)
    df["start_epoch"] = df["Starttime"].astype("int64") // 10**9
    df["end_epoch"] = df["Endtime"].astype("int64") // 10**9
    df["HVvalue"] = df["HVvalue"].astype("float64")

    if(chamber=="ME11a"): 
        df["rhid"] = (
        df["rhid"].astype(str).str[:2] + '4' +  # replace 3rd digit with '4'
        df["rhid"].astype(str).str[3:] +  # rest of the string
        df["rhid"].astype(str).str[-1]  # append last digit
        )
        df["rhid"] = df["rhid"].astype(int)
    elif(chamber=="ME11b"):
        df["rhid"] = (df["rhid"].astype(str) +
        df["rhid"].astype(str).str[-1]  # append last digit
        )
        df["rhid"] = df["rhid"].astype(int)
    return df

def MergingHV(df_root, df_HV) : 
    # rename the HV read column so to merge with the current ntuple 
    df_HV_renamed = df_HV.rename(columns={"rhid": "_rhid", "RunNb": "_runNb"})
    print(" after masking for particlular rhid : the dataframe ")
    print("len of dataframe", df_root)
    merged = pd.merge(df_root, df_HV_renamed, on=["_rhid", "_runNb"], how="left")
    # We merge the dataframes on the basis of rhid and runNb, and we check if the time lies in the start and end period
    mask = (merged["start_epoch"] <= merged["_timesecond"]) & (merged["end_epoch"] > merged["_timesecond"])

    filtered = merged[mask]
    print("length after merging and filtereing", len(filtered))  
    filtered.drop(columns=["Starttime", "Endtime", "start_epoch", "end_epoch"], inplace=True, errors="ignore")
    # Keep a version of filtered aligned to df_root (drop HVvalue, etc.)
    filtered_aligned = filtered[df_root.columns]
    # Get unmatched rows by subtracting
    unmatched = pd.concat([df_root, filtered_aligned]).drop_duplicates(keep=False)

    print("Unmatched length:", len(unmatched))
    print("Unmatched dataframe")
    print(unmatched)
    # Tag
    filtered["HVfound"] = True
    unmatched = unmatched.copy()
    unmatched["HVfound"] = False
    unmatched["HVvalue"] = np.nan  # Add HVvalue column for compatibility

    return filtered, unmatched 


def UncorrGasGain(chamber, df):
    if chamber in ["ME11a", "ME11b"]:
        const_B_ = 6.26e-3
        HV_standard = 2900
    elif chamber in ["ME21HV1", "ME21HV2", "ME21HV3", 
                         "ME31HV1", "ME31HV2", "ME31HV3",
                         "ME41HV1", "ME41HV2", "ME41HV3"]:
        const_B_ = 5.193e-3
        HV_standard = 3600
    else:
        const_B_ = 5.463e-3
        HV_standard = 3600
    
    df['_rhsumQ_equalised_HV_data'] = df['_rhsumQ_RAW'] * np.exp(-const_B_* (df['_HV'] - HV_standard) ) 
    return df

def find_hv_value_wide(row, df_HV, delta=300):  # 5 minutes = 300 seconds
    candidates = df_HV[
        (df_HV["rhid"] == row["_rhid"]) &
        (df_HV["RunNb"] == row["_runNb"]) &
        ((df_HV["start_epoch"] - delta) <= row["_timesecond"]) &
        ((df_HV["end_epoch"] + delta) >= row["_timesecond"])
    ]
    if not candidates.empty:
        return candidates.iloc[0]["HVvalue"]
    else:
        return np.nan

def finding_cumsum_lumi(type):
    delivered_lumi_dict = {}
    recorded_lumi_dict = {}
    sum_delivered_lumi = 0
    sum_recorded_lumi = 0
    for year in ["2016", "2017", "2018"]:
        if(year=="2016" or year=="2018"):
            nb_footer=5
        elif(year=="2017"):
            nb_footer=5

        if(type=="golden"):
            input_file_name = f"../../Brilcalc_evaluating_luminosity/{year}_lumibyls.csv"
            #input_file_name = f"../../Brilcalc_evaluating_luminosity/testing_prescales/{year}lumi_HLTIsoMu24_byls.csv"
        elif(type=="dcs"):
            input_file_name = f"../../Brilcalc_evaluating_luminosity/{year}_lumibyls_DCSONLY.csv"
            #input_file_name = f"../../Brilcalc_evaluating_luminosity/testing_prescales/{year}lumi_HLTIsoMu24_byls_dcsonly.csv"
        df = pd.read_csv(input_file_name, sep=',',skiprows=1, skipfooter=nb_footer, engine='python')  # required when using skipfooter)
        df['delivered(/ub)'] = pd.to_numeric(df['delivered(/ub)'], errors='coerce')
        df['recorded(/ub)'] = pd.to_numeric(df['recorded(/ub)'], errors='coerce')
        # Create cumulative sum columns
        df['int_delivered(/ub)'] = df['delivered(/ub)'].cumsum()
        df['int_recorded(/ub)'] = df['recorded(/ub)'].cumsum()
        df['int_delivered(/fb)'] = df['int_delivered(/ub)']/(10**9)
        df['int_recorded(/fb)'] = df['int_recorded(/ub)']/(10**9)
        delivered_lumi = df['int_delivered(/fb)'].iloc[-1] 
        recorded_lumi = df['int_recorded(/fb)'].iloc[-1] 
        delivered_lumi_dict[year] = sum_delivered_lumi
        recorded_lumi_dict[year] = sum_recorded_lumi
        sum_delivered_lumi += delivered_lumi
        sum_recorded_lumi += recorded_lumi

        #print("delivered lumi dict ", delivered_lumi_dict)
        #print("recorded lumi dict ", recorded_lumi_dict)    
        #print("year ", year, "delivered : ", sum_delivered_lumi , " recorded : ", sum_recorded_lumi)
    return delivered_lumi_dict  , recorded_lumi_dict
if __name__ == "__main__" :
    now = datetime.now()
    start_time = now.strftime("%H:%M:%S")
    print("Current Time =",start_time)

    parser = argparse.ArgumentParser()
    parser.add_argument("--chamber", help="chamber")
    parser.add_argument("--year", help="year")
    args = parser.parse_args()
    chamber = args.chamber
    year = args.year
   
    #input_name = "/afs/cern.ch/user/n/nrawal/eos/CSCAgeing/Run2_Ntuples/"+year+"_all/csc_output_"+year+"_"+chamber+"_tree.root"
    input_name = "/afs/cern.ch/user/n/nrawal/eos/CSCAgeing/Run2_Ntuples_complete_relaxed/"+year+"_all/csc_output_"+year+"_"+chamber+"_tree.root"
    #input_name = "/afs/cern.ch/user/n/nrawal/eos/CSCAgeing/Run2_Ntuples_with_relaxed_hit_per_layer/"+year+"_all/csc_output_"+year+"_"+chamber+"_tree.root"
    # Read the root file and update it later
    df_root = read_root_file(input_name)
    # Separate the root file into plus and minus side
    df_root_plus, df_root_minus = separating_plus_minus(df_root)
    print("read the root file and separated into plus and minus df")
    print("len root plus ", len(df_root_plus))
    print("len root minus ", len(df_root_minus))
    
    #Read the HV information for the chamber
    HV_type = chamber[:4]
    #input_HV_path = "/afs/cern.ch/work/n/nrawal/CSCAgeing_code_study/Obtaining_HV_from_database/final_HV_info_files_new/"
    input_HV_path = "/afs/cern.ch/user/n/nrawal/work/CSC_Run2_analysis/Obtaining_HV_from_database/final_HV_info_files_new/"
    input_file_HV_plus = input_HV_path+"HV_info_golden_ME11_asc"+year+".txt"
    input_file_HV_minus = input_HV_path+"HV_info_golden_ME11_asc"+year+".txt"
    df_HV_plus = reading_HV_file(input_file_HV_plus)
    df_HV_minus = reading_HV_file(input_file_HV_minus)
    print("read the HV plus and minus HV information")
    
    #Now merge the two cases so 
    final_df_plus, final_df_plus_not_matched = MergingHV(df_root_plus, df_HV_plus)
    #Now merge the two cases so to add the read HV to the ntuple
    if(len(final_df_plus_not_matched) !=0):
        final_df_plus_not_matched.drop(columns=["HVfound", "HVvalue"], inplace=True)
        final_df_plus_not_matched["HVvalue"] = final_df_plus_not_matched.apply(lambda row: find_hv_value_wide(row, df_HV_plus), axis=1)
        final_df_plus_not_matched["HVfound"] = final_df_plus_not_matched["HVvalue"].notna()
    if(len(final_df_plus_not_matched) !=0):
        final_df_plus_updated = pd.concat([final_df_plus, final_df_plus_not_matched], ignore_index=True)
    else: 
        final_df_plus_updated = final_df_plus
    print('length all plus ',len(final_df_plus_updated)) 

    print("done with finding HV for plus")

    final_df_minus, final_df_minus_not_matched = MergingHV(df_root_minus, df_HV_minus)
    #Now merge the two cases so to add the read HV to the ntuple
    if(len(final_df_minus_not_matched) !=0):
        final_df_minus_not_matched.drop(columns=["HVfound", "HVvalue"], inplace=True)
        final_df_minus_not_matched["HVvalue"] = final_df_minus_not_matched.apply(lambda row: find_hv_value_wide(row, df_HV_minus), axis=1)
        final_df_minus_not_matched["HVfound"] = final_df_minus_not_matched["HVvalue"].notna()
    if(len(final_df_minus_not_matched) !=0):
        final_df_minus_updated = pd.concat([final_df_minus, final_df_minus_not_matched], ignore_index=True)
    else:
        final_df_minus_updated = final_df_minus
    print('length all minus ',len(final_df_minus_updated)) 
    print("done with finding HV for minus")

    final_df = pd.concat([final_df_plus_updated, final_df_minus_updated], ignore_index=True)
    # Modify the integratedluminosity column; so we can combine 2016, 2017, and 2018
    final_df_renamed = final_df.rename(columns={"_HV" : "_HV_nominal", "HVvalue" : "_HV"})
    # Updated the charge according to the read HV
    final_df_updated_HV = UncorrGasGain(chamber, final_df_renamed)

    delivered_lumi_golden_dict = {}
    recorded_lumi_golden_dict = {}
    delivered_lumi_dcs_dict = {}
    recorded_lumi_dcs_dict = {}

    # Read the cumulative lumi for each year separately using a function, and use it further to add to the final luminosity
    delivered_lumi_golden_dict, recorded_lumi_golden_dict = finding_cumsum_lumi("golden")
    delivered_lumi_dcs_dict, recorded_lumi_dcs_dict = finding_cumsum_lumi("dcs")
    print(" delivered lumi : recorded lumi : year")
    print(delivered_lumi_golden_dict["2016"]," : ", recorded_lumi_golden_dict["2016"], "2016")
    print(delivered_lumi_golden_dict["2017"], " : ", recorded_lumi_golden_dict["2017"], "2017")
    print(delivered_lumi_golden_dict["2018"], " : ",recorded_lumi_golden_dict["2018"], "2018")
    print(delivered_lumi_dcs_dict["2016"]," : ", recorded_lumi_dcs_dict["2016"], "2016")
    print(delivered_lumi_dcs_dict["2017"], " : ", recorded_lumi_dcs_dict["2017"], "2017")
    print(delivered_lumi_dcs_dict["2018"], " : ",recorded_lumi_dcs_dict["2018"], "2018")

#    year_to_lumi_golden = {
#            "2016": 0,
#            "2017": 40.110055756,
#            "2018": 77.906667}
#    year_to_lumi_dcs = {
#            "2016": 0,
#            "2017": 39.091604120,
#            "2018": 81.07889}
    df_lumi_golden = read_lumi_file_golden(year)
    df_lumi_golden = process_lumi_file(df_lumi_golden)
    df_lumi_dcs = read_lumi_file_dcs(year)
    df_lumi_dcs = process_lumi_file(df_lumi_dcs)

    final_df, final_df_not_matched = MergingLumi_golden(final_df_updated_HV, df_lumi_golden)
    final_df['_intlumi_delivered_goldenjson'] = final_df['_intlumi_delivered_goldenjson'] + delivered_lumi_golden_dict[year]
    final_df['_intlumi_recorded_goldenjson'] = final_df['_intlumi_recorded_goldenjson'] + recorded_lumi_golden_dict[year]
    #final_df['_intlumi_delivered_goldenjson'] = final_df['_intlumi_delivered_goldenjson'] +year_to_lumi_golden[year]
    #final_df['_intlumi_recorded_goldenjson'] = final_df['_intlumi_recorded_goldenjson'] +year_to_lumi_golden[year]
    print('final file length ', len(final_df))
    print('final file not length ', len(final_df_not_matched))
    not_lumi = final_df[final_df['lumifound']==False]
    print("len not found ", len(not_lumi))
    # Save the final dataframe into another root file
    final_df_dcs, final_df_not_matched_dcs = MergingLumi_dcs(final_df, df_lumi_dcs)
    #final_df_dcs['_intlumi_delivered_dcsjson'] = final_df_dcs['_intlumi_delivered_dcsjson'] +year_to_lumi_dcs[year]
    #final_df_dcs['_intlumi_recorded_dcsjson'] = final_df_dcs['_intlumi_recorded_dcsjson'] +year_to_lumi_dcs[year]
    final_df_dcs['_intlumi_delivered_dcsjson'] = final_df_dcs['_intlumi_delivered_dcsjson'] +delivered_lumi_dcs_dict[year]
    final_df_dcs['_intlumi_recorded_dcsjson'] = final_df_dcs['_intlumi_recorded_dcsjson'] +recorded_lumi_dcs_dict[year]
    print("saving final root file")
    saveFile(final_df_dcs, chamber, year)
#    year_to_lumi_golden = {
#            "2016": 0,  
#            "2017": 37.801336,
#            "2018": 82.070905}
#    year_to_lumi_dcs = {
#            "2016": 0,  
#            "2017": 39.32673126400002,
#            "2018": 83.85340826572357}

