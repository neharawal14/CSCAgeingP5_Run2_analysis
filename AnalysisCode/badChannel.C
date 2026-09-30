bool isbadchannel(TString chamber_type_name, int rhid, int strip_number, TString year){


  std::cout<<" entered into bad channel"<<std::endl; 
  int endcap = (rhid<400)? 1:2;
  if(rhid>400) rhid -=400;

  int layer = rhid%10;
  int chamber = floor(rhid/10);

  if(strip_number>=1 && strip_number <=16) cfeb_number =1;
  if(strip_number>16 && strip_number <=32) cfeb_number =2;
  if(strip_number>32 && strip_number <=48) cfeb_number =3;
  if(strip_number>48 && strip_number <=64) cfeb_number =4;
  if(strip_number>64 && strip_number <=80) cfeb_number =5;
  if(strip_number>80 && strip_number <=96) cfeb_number =6;


  std::cout<<" rhid "<<rhid<<" chamber "<<chamber<<" cfeb number "<<cfeb_number<<" strip number "<<strip_number<<std::endl; 
  //ME31HV1, ME31HV2, ME31HV3 
  //chamber with issue
  if(chamber_type_name.Index("ME31")>=0) {
  if(chamber_type_name.Index("ME31")>=0 && year=="2016" && endcap==2 &&chamber== 9)return true;
  if(chamber_type_name.Index("ME31")>=0 && year=="2017" && endcap==2 &&chamber== 9)return true;
  if(chamber_type_name.Index("ME31")>=0 && year=="2018" && endcap==2 &&chamber== 9)return true;
  // Chamber CFEB issues
  if(chamber_type_name.Index("ME31")>=0 && year=="2016" && endcap== 2 &&chamber== 3 &&layer== 2 && cfeb_number==3 )return true;
  if(chamber_type_name.Index("ME31")>=0 && year=="2018" && endcap== 2 &&chamber== 3 &&layer== 2 && cfeb_number==3 )return true;
  
  // Chamber CFEB dead
  if(chamber_type_name.Index("ME31")>=0 && (year=="2016" || year=="2017" || year=="2018") && endcap== 2 &&chamber== 13 && cfeb_number==3 )return true;
  // low efficiency 
  if(chamber_type_name.Index("ME31")>=0 && year=="2016" && endcap== 1 &&chamber== 3 && cfeb_number==2 )return true;
  // CFEB dead
  if(chamber_type_name.Index("ME31")>=0   && endcap== 1 &&chamber== 7 && cfeb_number==3 && 
     (year=="2016" || year=="2017" || year=="2018"))return true;
  if(chamber_type_name.Index("ME31")>=0   && endcap== 1 &&chamber== 11 && cfeb_number==3 && year=="2016")return true;
  // low efficiency 
  if(chamber_type_name.Index("ME31HV1")>=0 && (year=="2016" || year=="2017" || year=="2018") && endcap== 1 &&chamber== 12 && cfeb_number==5 )return true;
  }

  //ME41HV1, ME41HV2, ME41HV3
  // Dead CFEBs -endcap
  if(chamber_type_name.Index("ME41")>=0) {
  if(chamber_type_name.Index("ME41")>=0 && (year=="2016" || year=="2017" || year=="2018")  && endcap== 2 &&chamber== 1 && cfeb_number==3)return true;
  if(chamber_type_name.Index("ME41")>=0 && (year=="2016" || year=="2017" || year=="2018")  && endcap== 2 &&chamber== 11 && cfeb_number==5)return true;
  if(chamber_type_name.Index("ME41")>=0 && (year=="2016" || year=="2017" || year=="2018")  && endcap== 2 &&chamber== 15 && cfeb_number==5)return true;

  // Dead CFEBs +endcap
  if(chamber_type_name.Index("ME41")>=0 && (year=="2016" || year=="2017" || year=="2018")  && endcap== 1 &&chamber== 15 && cfeb_number==5)return true;
  // Low efficiency
  if(chamber_type_name.Index("ME41")>=0 && (year=="2016" || year=="2017" || year=="2018")  && endcap== 1 &&chamber== 15 && cfeb_number==2)return true; 
   }


   //ME32HV1, ME32HV2, ME32HV3, ME32HV4, ME32HV5
   // Some CFEB issue, not always but removing

  if(chamber_type_name.Index("ME32")>=0) {
   if(chamber_type_name.Index("ME32")>=0 && year=="2016"  && endcap== 2 &&chamber== 3 && cfeb_number==5)return true;
   // CFEB issue
   if(chamber_type_name.Index("ME32")>=0 && year=="2016"  && endcap== 2 &&chamber== 24 && cfeb_number==5)return true;
   // some issues 
   if(chamber_type_name.Index("ME32HV1")>=0 && year=="2017"  && endcap== 2 &&chamber== 3 && layer==5)return true;
   if(chamber_type_name.Index("ME32")>=0 && year=="2017"  && endcap== 2 &&chamber== 22 && cfeb_number==5)return true;
   if(chamber_type_name.Index("ME32HV1")>=0 && year=="2018"  && endcap== 2 &&chamber== 3 && layer==5)return true;
   //+endcap
 
   if(chamber_type_name.Index("ME32")>=0  && endcap== 1 &&chamber== 29 && cfeb_number==4)return true;
   if(chamber_type_name.Index("ME32")>=0  && endcap== 1 &&chamber== 19 && cfeb_number==5)return true;
   if(chamber_type_name.Index("ME32")>=0 && year=="2016"  && endcap== 2 &&chamber== 3 && cfeb_number==5)return true;
   }

   // ME42HV1, ME42HV2, ME42HV3, ME42HV4, ME42HV5

  if(chamber_type_name.Index("ME42")>=0) {
   if(chamber_type_name.Index("ME42")>=0  && endcap== 2 &&chamber== 1 && cfeb_number==3)return true;
   if(chamber_type_name.Index("ME42")>=0  && endcap== 2 &&chamber== 8 && cfeb_number==2)return true;
   if(chamber_type_name.Index("ME42")>=0  && endcap== 2 &&chamber== 34 && cfeb_number==5)return true;
   if(chamber_type_name.Index("ME42")>=0  && endcap== 2 &&chamber== 14 && cfeb_number==2)return true;

   if(chamber_type_name.Index("ME42")>=0  && (year=="2017" || year=="2018") && endcap== 2 &&chamber== 21)return true;
   if(chamber_type_name.Index("ME42")>=0  && year=="2018" && endcap== 2 &&chamber== 27 && cfeb_number==3)return true;
  
   // +endcap 
   if(chamber_type_name.Index("ME42")>=0   && endcap== 1 &&chamber== 32 && cfeb_number==1)return true;
  }

  //ME21HV1, ME21HV2, ME21HV3
  if(chamber_type_name.Index("ME21")>=0) {
  if(chamber_type_name.Index("ME21")>=0   && endcap== 2 &&chamber== 3)return true;
  if(chamber_type_name.Index("ME21")>=0   && endcap== 2 &&chamber== 9 && cfeb_number==4)return true;
  if(chamber_type_name.Index("ME21")>=0   && endcap== 2 &&chamber== 17)return true;
  
  if(chamber_type_name.Index("ME21")>=0 year=="2018"  && endcap== 2 &&chamber== 9 && layer==5)return true;
  if(chamber_type_name.Index("ME21")>=0 && (year=="2016" || year=="2017") && endcap== 1 &&chamber== 1 && cfeb_number==5)return true;
  if(chamber_type_name.Index("ME21")>=0 && endcap== 1 &&chamber== 3 && cfeb_number==2)return true;
  }

  //ME22HV1, ME22HV2, ME22HV3, ME22HV4, ME22HV5
  if(chamber_type_name.Index("ME22")>=0) {
  if(chamber_type_name.Index("ME22")>=0 && (year=="2016" || year=="2017") && endcap== 2 &&chamber==3 && cfeb_number==2)return true;
  if(chamber_type_name.Index("ME22")>=0 && (year=="2016" || year=="2017") && endcap== 2 &&chamber==31 && cfeb_number==2 && layer==1)return true;
  if(chamber_type_name.Index("ME22")>=0 && (year=="2016" || year=="2017" || year =="2018") && endcap== 2 &&chamber==1 && cfeb_number==4)return true;
  if(chamber_type_name.Index("ME22HV5")>=0 && year=="2016" && endcap== 2 &&chamber==7 && cfeb_number==5)return true;
  if(chamber_type_name.Index("ME22")>=0 && year=="2017" && endcap== 2 &&chamber==1)return true;
  
  if(chamber_type_name.Index("ME22")>=0 && year=="2016" && endcap== 1 &&chamber==29)return true;
  if(chamber_type_name.Index("ME22")>=0 && endcap== 1 &&chamber==15 &&cfeb_number==5)return true;
  if(chamber_type_name.Index("ME22")>=0 && year=="2016" && endcap== 1 &&chamber==14 && layer==4)return true;
  if(chamber_type_name.Index("ME22")>=0 && year=="2016" && endcap== 1 &&chamber==19 && cfeb_number==5)return true;
  } 

  //ME13HV1, ME13HV2, ME13HV3
  
  if(chamber_type_name.Index("ME13")>=0) {
  if(chamber_type_name.Index("ME13")>=0 && (year=="2017" || year=="2018") && endcap== 2 &&chamber==29 && layer==1 &&cfeb_number==4)return true;
  if(chamber_type_name.Index("ME13")>=0 && (year=="2017" || year=="2018") && endcap== 2 &&chamber==22 &&cfeb_number==4)return true;
  if(chamber_type_name.Index("ME13")>=0 && year=="2016" && endcap== 1 &&chamber==15 && cfeb_number==1)return true;
  if(chamber_type_name.Index("ME13")>=0 &&year=="2016" &&  endcap== 1 &&chamber==16 && cfeb_number==2)return true;
  if(chamber_type_name.Index("ME13HV1")>=0 && year =="2016" && endcap== 1 &&chamber==29 && (cfeb_number==1 || cfeb_number==2))return true;
  if(chamber_type_name.Index("ME13")>=0 && endcap== 1 && year=="2016" && chamber==11 && cfeb_number==3)return true;
  if(chamber_type_name.Index("ME13")>=0 && year=="2017" && endcap== 1 && chamber==15)return true;
  if(chamber_type_name.Index("ME13")>=0 && year=="2017" && endcap== 1 && chamber==11 && cfeb_number==4)return true;
  if(chamber_type_name.Index("ME13")>=0 && year=="2017" && endcap== 1 && chamber==11 && cfeb_number==3)return true;
  if(chamber_type_name.Index("ME13")>=0 && year=="2017" && endcap== 1 && chamber==16 && cfeb_number==2)return true;
  if(chamber_type_name.Index("ME13")>=0 && year=="2018" && endcap== 1 && chamber==16 && cfeb_number==2)return true;
  if(chamber_type_name.Index("ME13")>=0 && year=="2018" && endcap== 1 && chamber==11 && cfeb_number==3)return true;
  if(chamber_type_name.Index("ME13")>=0 && year=="2018" && endcap== 1 && chamber==18 && (cfeb_number==3 || cfeb_number==2))return true;
  }
 //ME12HV1, ME12HV2, ME12HV3
 
  if(chamber_type_name.Index("ME12")>=0) {
  if(chamber_type_name.Index("ME12")>=0  && year=="2016" && endcap== 2 && chamber==7)return true;
  if(chamber_type_name.Index("ME12")>=0  && endcap== 2 && chamber==33 && cfeb_number==1)return true;
  if(chamber_type_name.Index("ME12")>=0  && (year=="2017" || year=="2018") && endcap== 2 && chamber==21 &&cfeb_number==3)return true;
  if(chamber_type_name.Index("ME12")>=0  && year=="2017" && endcap== 2 && chamber==22)return true;

  if(chamber_type_name.Index("ME12")>=0  && year=="2016" && endcap== 1 && chamber==15 && cfeb_number==1)return true;
  if(chamber_type_name.Index("ME12")>=0  && year=="2017" && endcap== 1 && chamber==22)return true;
  if(chamber_type_name.Index("ME12")>=0  && year=="2018" && endcap== 1 && chamber==8)return true;
  if(chamber_type_name.Index("ME12")>=0  && year=="2018" && endcap== 1 && chamber==20 &&cfeb_number==3)return true;
  if(chamber_type_name.Index("ME12")>=0  && endcap== 1 && chamber==1 &&cfeb_number==4)return true;
  if(chamber_type_name.Index("ME12")>=0  && endcap== 1 && chamber==13 &&cfeb_number==5)return true;
  if(chamber_type_name.Index("ME12")>=0  && endcap== 1 && chamber==21 && (cfeb_number==4 || cfeb_number==2))return true;
  if(chamber_type_name.Index("ME12")>=0  && endcap== 1 && chamber==31 &&cfeb_number==5)return true;
 }
 
  // ME11a, ME11b
  // 2016

    if(chamber_type_name.Index("ME11")>=0 ) {
  	if(chamber_type_name.Index("ME11")>=0 && year=="2016"  && endcap== 2 && chamber==12)return true;
  	if(chamber_type_name.Index("ME11")>=0 && year=="2016"  && endcap== 2 && chamber==19)return true;
  	if(chamber_type_name.Index("ME11")>=0 && year=="2016"  && endcap== 2 && chamber==9 && layer==2)return true;
  	
  	if(chamber_type_name.Index("ME11a")>=0 && year=="2016"  && endcap== 2 && chamber==18 && (cfeb_number==1 || cfeb_number==2))return true;
  	if(chamber_type_name.Index("ME11b")>=0 && year=="2016"  && endcap== 2 && chamber==14 && cfeb_number==4)return true;
  	if(chamber_type_name.Index("ME11a")>=0 && year=="2016"  && endcap== 2 && chamber==21 && cfeb_number==2)return true;
  	if(chamber_type_name.Index("ME11a")>=0 && year=="2016"  && endcap== 2 && chamber==31 && cfeb_number==2)return true;
  	if(chamber_type_name.Index("ME11b")>=0 && year=="2016"  && endcap== 2 && chamber==30 && cfeb_number==4)return true;

  	 // 2017 , mostly channel with HV issue also removed
  	if(chamber_type_name.Index("ME11a")>=0 && year=="2017"  && endcap== 2 && chamber==7)return true;
  	if(chamber_type_name.Index("ME11")>=0  && year=="2017" && endcap== 2 && chamber==11 && cfeb_number==2)return true;
  	if(chamber_type_name.Index("ME11")>=0  && year=="2017" && endcap== 2 && (chamber==34 || chamber==35))return true;
  	if(chamber_type_name.Index("ME11")>=0  && year=="2017" && endcap== 2 && chamber==28 && layer==3)return true;
  	if(chamber_type_name.Index("ME11a")>=0 && year=="2017"  && endcap== 2 && chamber==33 && cfeb_number==2 && (layer==4 || layer==5 || layer==6))return true;

  	if(chamber_type_name.Index("ME11")>=0  && year=="2017" && endcap== 2 && (layer==1 || layer==2) && chamber==5)return true;
  	if(chamber_type_name.Index("ME11")>=0  && year=="2017" && endcap== 2 && layer==2 && chamber==9)return true;
  	if(chamber_type_name.Index("ME11")>=0  && year=="2017" && endcap== 2 && layer==4 && chamber==25)return true;
  	if(chamber_type_name.Index("ME11")>=0  && year=="2017" && endcap== 2 && layer==3 && chamber==19)return true;
  	if(chamber_type_name.Index("ME11")>=0  && year=="2017" && endcap== 2 && (layer==4 || layer==6) && chamber==25)return true;
  	
  	// 2018
  	if(chamber_type_name.Index("ME11a")>=0  && year=="2018" && endcap== 2 &&  chamber==7)return true;
  	if(chamber_type_name.Index("ME11a")>=0  && year=="2018" && endcap== 2 && cfeb_number==3 && chamber==11)return true;
  	if(chamber_type_name.Index("ME11")>=0  && year=="2018" && endcap== 2 && chamber==28 && layer==3)return true;
  	if(chamber_type_name.Index("ME11")>=0 && year=="2018"  && endcap== 2 && chamber==25 && layer==6 )return true;
  	if(chamber_type_name.Index("ME11")>=0 && (year=="2017" || year=="2018")  && endcap== 2 && chamber==5 && layer==1 )return true;
 
  	// plus endcap 2016
  	if(chamber_type_name.Index("ME11a")>=0 && year=="2016"  && endcap== 1 && chamber==1 && layer==5)return true;
  	if(chamber_type_name.Index("ME11a")>=0 && year=="2016"  && endcap== 1 && chamber==3 && cfeb_number==2)return true;
  	if(chamber_type_name.Index("ME11a")>=0 && year=="2016"  && endcap== 1 && chamber==13 && cfeb_number==2)return true;
  	if(chamber_type_name.Index("ME11a")>=0 && year=="2016"  && endcap== 1 && chamber==17)return true;
  	if(chamber_type_name.Index("ME11a")>=0 && year=="2016"  && endcap== 1 && chamber==25 && cfeb_number==3)return true;
  	if(chamber_type_name.Index("ME11a")>=0 && year=="2016"  && endcap== 1 && chamber==26)return true;
  	if(chamber_type_name.Index("ME11a")>=0 && year=="2016"  && endcap== 1 && chamber==31 &&cfeb_number==1)return true;
  	if(chamber_type_name.Index("ME11a")>=0 && year=="2016"  && endcap== 1 && chamber==32 &&cfeb_number==1)return true;
  	if(chamber_type_name.Index("ME11a")>=0 && year=="2016"  && endcap== 1 && chamber==33 && (layer==1 && layer==3))return true;

  	if(chamber_type_name.Index("ME11a")>=0 && year=="2017"  && endcap== 1 && chamber==1 && layer==5)return true;
  	if(chamber_type_name.Index("ME11a")>=0 && year=="2017"  && endcap== 1 && chamber==8 && cfeb_number==3)return true;
  	if(chamber_type_name.Index("ME11a")>=0 && year=="2017"  && endcap== 1 && chamber==10 && cfeb_number==3)return true;
  	if(chamber_type_name.Index("ME11a")>=0 && year=="2017"  && endcap== 1 && chamber==17)return true;
  	if(chamber_type_name.Index("ME11a")>=0 && year=="2017"  && endcap== 1 && chamber==13 && layer==1)return true;
  	if(chamber_type_name.Index("ME11a")>=0 && year=="2017"  && endcap== 1 && chamber==33 && (layer==1 && layer==3))return true;
  	if(chamber_type_name.Index("ME11a")>=0 && year=="2017"  && endcap== 1 && chamber==32)return true;

  	if(chamber_type_name.Index("ME11b")>=0 && year=="2016"  && endcap== 1 && chamber==3 && cfeb_number==1)return true;
  	if(chamber_type_name.Index("ME11b")>=0 && year=="2016"  && endcap== 1 && chamber==12 && cfeb_number==1)return true;
  	if(chamber_type_name.Index("ME11b")>=0 && year=="2016"  && endcap== 1 && chamber==13 && cfeb_number==1)return true;
  	if(chamber_type_name.Index("ME11b")>=0 && year=="2016"  && endcap== 1 && chamber==25 && cfeb_number==2)return true;
  	if(chamber_type_name.Index("ME11b")>=0 && year=="2016"  && endcap== 1 && chamber==26)return true;
  	if(chamber_type_name.Index("ME11b")>=0 && year=="2016"  && endcap== 1 && chamber==33 && layer==3)return true;

  	if(chamber_type_name.Index("ME11b")>=0 && year=="2017"  && endcap== 1 && chamber==1 && layer==5)return true;
  	if(chamber_type_name.Index("ME11b")>=0 && year=="2017"  && endcap== 1 && chamber==13 && layer==1)return true;
  	if(chamber_type_name.Index("ME11b")>=0 && year=="2017"  && endcap== 1 && chamber==17 && cfeb_number==4)return true;
  	if(chamber_type_name.Index("ME11b")>=0 && year=="2017"  && endcap== 1 && chamber==32)return true;
  	if(chamber_type_name.Index("ME11b")>=0 && year=="2017"  && endcap== 1 && chamber==33 && (layer==1 || layer==3))return true;

  	// 2018
  	if(chamber_type_name.Index("ME11a")>=0  && year=="2018" && endcap== 1 &&  chamber==5 && layer==1)return true;
  	if(chamber_type_name.Index("ME11a")>=0  && year=="2018" && endcap== 1 &&  chamber==7)return true;
  	if(chamber_type_name.Index("ME11a")>=0  && year=="2018" && endcap== 1 &&  chamber==11 && cfeb_number==3)return true;
  	if(chamber_type_name.Index("ME11a")>=0  && year=="2018" && endcap== 1 && chamber==28 && layer==3)return true;
  	if(chamber_type_name.Index("ME11a")>=0 && year=="2018"  && endcap== 1 && chamber==25 && layer==6 )return true;
  	if(chamber_type_name.Index("ME11")>=0 && year=="2018"  && endcap== 1 && chamber==32 )return true;
  	if(chamber_type_name.Index("ME11b")>=0 && year=="2018"  && endcap== 1 && chamber==1 )return true;
  	if(chamber_type_name.Index("ME11b")>=0 && year=="2018"  && endcap== 1 && chamber==33 && layer==3 )return true;
     } // end of ME11


return false;

}

