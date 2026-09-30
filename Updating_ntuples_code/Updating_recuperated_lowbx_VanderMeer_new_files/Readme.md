## Details
This program is to remove those entries from the root file, for which:
 1. the number of colliding bunches are lower than certain number
 2. Removing VanderMeer scan period ; during which instlumi is not stable
 3. Remove periods corresponding to recuperated gas ; which we do not want to analyse

I currently do not remember, if I build the executable using running.sh and use it; or I use the  CMakeList.txt and use build folder; 
please check for yourself
But it could be figured out later

Also the folder BunchCrossing_per_fill -> has more function to read the number of colliding bunches per fill and all.
 Checking_VanderMeer_scans -> Code to read and study instlumi values for each fill, to understand and design the code to remove VanderMeer Scan
