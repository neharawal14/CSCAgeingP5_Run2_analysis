import uproot
import numpy as np
import pandas as pd
import argparse
import ROOT
from datetime import datetime

def read_lumi_file_golden(year):
    input_file_name = f"../../Brilcalc_evaluating_luminosity/testing_prescales/{year}lumi_HLTIsoMu24_byls.csv"
    if(year=="2016" or year=="2018"):
        footer_nb=10
    elif(year=="2017"):
        footer_nb=14
    df = pd.read_csv(input_file_name, sep=',',skiprows=1, skipfooter=footer_nb, engine='python')  # required when using skipfooter)
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
    df = df.drop(columns=['run:fill','time', 'hltpath', 'avgpu', 'source','ls'])
    df['delivered(/ub)'] = pd.to_numeric(df['delivered(/ub)'], errors='coerce')
    df['recorded(/ub)'] = pd.to_numeric(df['recorded(/ub)'], errors='coerce')
    # Create cumulative sum columns
    df['int_delivered(/ub)'] = df['delivered(/ub)'].cumsum()
    df['int_recorded(/ub)'] = df['recorded(/ub)'].cumsum()

    return df
def finding_cumsum_lumi(type):
    delivered_lumi_dict = {}
    recorded_lumi_dict = {}
    sum_delivered_lumi = 0
    sum_recorded_lumi = 0
    for year in ["2016", "2017", "2018"]:
        if(year=="2016" or year=="2018"):
            nb_footer=10
        elif(year=="2017"):
            nb_footer=14
    
        if(type=="golden"):
            input_file_name = f"../../Brilcalc_evaluating_luminosity/testing_prescales/{year}lumi_HLTIsoMu24_byls.csv"
            #input_file_name = f"/afs/cern.ch/work/n/nrawal/Brilcal_new_env/Run2_UTC_lumi_evaluation/testing_prescales/{year}lumi_HLTIsoMu24_byls.csv"
        elif(type=="dcs"):
            #input_file_name = f"/afs/cern.ch/work/n/nrawal/Brilcal_new_env/Run2_UTC_lumi_evaluation/testing_prescales/{year}lumi_HLTIsoMu24_byls_dcsonly.csv"
            input_file_name = f"../../Brilcalc_evaluating_luminosity/testing_prescales/{year}lumi_HLTIsoMu24_byls_dcsonly.csv"
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
    return delivered_lumi_dict,recorded_lumi_dict
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
    merged.drop(columns=['delivered(/ub)', 'recorded(/ub)', 'int_recorded(/ub)', 'int_delivered(/ub)'], inplace=True)
    
    #print("merged columns ", merged.columns)
    merged.rename(columns={"int_delivered(/fb)" : "_intlumi_delivered_goldenjson"}, inplace=True)
    merged.rename(columns={"int_recorded(/fb)" : "_intlumi_recorded_goldenjson"}, inplace=True)
    filtered = merged
    print("length after merging and filtereing", len(filtered))  
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
def saveFile(df, dataset,year) :
    csv_file = f"/afs/cern.ch/user/n/nrawal/eos/CSCAgeing/Run2_Ntuples/Zfiles/Input_withSelection/csc_output_{year}{dataset}_tree_updatedLumi"
    Savecsv(df, csv_file)
    df_RDF = ROOT.RDF.FromCSV(csv_file+".csv")
    print("RDF :",df_RDF)
    # Snapshot it to a ROOT file
    output_file = csv_file+".root"
    df_RDF.Snapshot("tree", output_file)

def read_root_file(input_name):
    file = uproot.open(input_name)
    tree = file["tree"]  # or your actual TTree name
    df_root = tree.arrays([ "_eventNb", "_runNb", "_lumiBlock", "_ptmuon"], library="pd")  # event_ti
    return df_root

if __name__ == "__main__" :
    now = datetime.now()
    start_time = now.strftime("%H:%M:%S")
    print("Current Time =",start_time)

    parser = argparse.ArgumentParser()
    parser.add_argument("--year", help="year")
    parser.add_argument("--dataset", help="dataset")
    args = parser.parse_args()
    year = args.year
    dataset = args.dataset
   
    input_name = f"/afs/cern.ch/user/n/nrawal/eos/CSCAgeing/Run2_Ntuples/Zfiles/Input_withSelection/csc_output_{year}{dataset}_tree.root"
    # Read the root file and update it later
    df_root = read_root_file(input_name)
    # Separate the root file into plus and minus side
    df_lumi_golden = read_lumi_file_golden(year)
    df_lumi_golden = process_lumi_file(df_lumi_golden)
    
    delivered_lumi_golden_dict = {}
    #delivered_lumi_dcs_dict = 0
    recorded_lumi_golden_dict = {}
    #recorded_lumi_dcs_dict = 0
    final_df, final_df_not_matched = MergingLumi_golden(df_root, df_lumi_golden)
    delivered_lumi_golden_dict, recorded_lumi_golden_dict = finding_cumsum_lumi("golden")
    final_df['_intlumi_delivered_goldenjson'] = final_df['_intlumi_delivered_goldenjson'] + delivered_lumi_golden_dict[year]
    final_df['_intlumi_recorded_goldenjson'] = final_df['_intlumi_recorded_goldenjson'] + recorded_lumi_golden_dict[year]

    print('final file length ', len(final_df))
    print('final file not length ', len(final_df_not_matched))
    not_lumi = final_df[final_df['lumifound']==False]
    print("len not found ", len(not_lumi))
    # Save the final dataframe into another root file
    print("saving final root file")
    saveFile(final_df,  dataset, year)
