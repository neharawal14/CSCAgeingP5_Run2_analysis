# How to find testing prescale lumi 
Installation brilcal : website or this code : 
Calculated luminosity information from BRIL-CAL software
brilcalc lumi -c web -r 160431
pip install brilcalc

Main Brilcal : https://twiki.cern.ch/twiki/bin/viewauth/CMS/BrilcalcQuickStart
https://cms-service-lumi.web.cern.ch/cms-service-lumi/brilwsdoc.html

Further luminosity information 
https://cmsoms.cern.ch/cms/run_3/index
Finding the luminosity goldenjson fles at : https://cms-service-dqmdc.web.cern.ch/CAF/certification/
https://twiki.cern.ch/twiki/bin/viewauth/CMS/BrilcalcQuickStart

In the analysis brilcalc is run over a single hltPathTrigger to obtain integrated luminosity corresponding to the specific trigger, not the whole golden json
We use delievered luminosity as luminosity for our analysis

code 
```
export PATH=$HOME/.local/bin:/cvmfs/cms-bril.cern.ch/brilconda/bin:$PATH
```

```
/cvmfs/cms-bril.cern.ch/brilconda310/bin/python3 -m pip install --user --upgrade brilws
```

```
normtag='/cvmfs/cms-bril.cern.ch/cms-lumi-pog/Normtags/normtag_PHYSICS.json'
```
Luminosity for a particular hltpath
```
 brilcalc lumi --normtag "${normtag}" --hltpath "HLT_IsoMu24_v*" -u /fb -i "Cert_294927-306462_13TeV_PromptReco_Collisions17_JSON.txt" -o 2017lumi_HLTIsoMu24.csv
```
 ```
  brilcalc lumi --normtag "${normtag}" --byls --hltpath "HLT_IsoMu24_v*" -u /ub -i "Cert_294927-306462_13TeV_PromptReco_Collisions17_JSON.txt" -o 2017lumi_HLTIsoMu24_byls.csv
```
