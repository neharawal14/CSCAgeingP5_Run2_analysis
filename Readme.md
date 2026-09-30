## Details

1. Code need to update the HV values, intlumi/instlumi values; remove events with low bunch crossing, and VanderMeer scans

 This all is done and explained in : 	Updating_ntuples_code

2. Just to plot HV values, difference between HV and nominal HV value; and to plot the charge distribution across years:
  Reading_HV_charge

3. BrilCalc_evaluating_luminosity : This is to just derive integratelumi information for each runNb, also similar installation setup is in the ntuple maker code. So it is kind of repetitive here

4. Counting_Z_events : We tried to check the number of Z events in our ntuples, and see this rate vs integrated luminosity. This code is so separate, since our ntuples are recHit, and a Z event could have multiple recHits corresponding to it. So our job to find number of Z events, is to select the unique recHits, which have same_eventNb; which correspond to 1 Z event. 
Hence, this folder is used for that

5. Obtaining_HV_from_database  : To obtain HV values for each chamber, corresponding to all the time periods which are in our ntuples. 

6. Counting_Hits : To count the toal number of Hits in each event, make plot of hit vs integrated luminosity and all.
