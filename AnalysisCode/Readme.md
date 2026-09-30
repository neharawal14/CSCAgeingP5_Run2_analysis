#Details

This code is to analyse the ntuples with CSC charge information. 
We derive gain dependence on pressure/instlumi, correct for them, and look for gain dependence on integrated luminosity-- the final ntuples are stored in other root files, in the folder "Results_Root_files"

To run the program use the folder with "Scripts"
On the stored root files, we run again some code to make final plots that is in "Plotting_Results"

The important codes are:
Analysis_Gas_gain : To study gain dependence on pressure/instlumi; 
evenLumiBlock : To study gain dependence on even Lumi sections
oddnLumiBlock : To study gain dependence for odd Lumi sections
evenCFEBs : To study gain dependence for even CFEBs getting triggered
oddCFEBs : To study gain dependence for even CFEBs getting triggered


