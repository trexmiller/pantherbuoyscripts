
void PrintResults(){
  if (Verbose > 1){
    Serial.print("YY"); Serial.print(",");
    Serial.print("MM"); Serial.print(",");
    Serial.print("DD"); Serial.print(",");
    Serial.print("HH"); Serial.print(",");
    Serial.print("MM"); Serial.print(",");
    Serial.print("SS"); Serial.print(",");
  
    //IMU data
    Serial.print("accelx"); Serial.print(",");
    Serial.print("accely"); Serial.print(",");
    Serial.print("accelz"); Serial.print(",");
    Serial.print("magx"); Serial.print(",");
    Serial.print("magy"); Serial.print(",");
    Serial.print("magz"); Serial.print(",");
    Serial.print("gyrox"); Serial.print(",");
    Serial.print("gyroy"); Serial.print(",");
    Serial.print("gyroz"); Serial.print(",");
    Serial.print("roll"); Serial.print(",");
    Serial.print("pitch"); Serial.print(",");
    Serial.print("heading"); Serial.print(",");

    //Modem data
    Serial.print("rsi"); Serial.print(",");
    Serial.print("ModemTemp"); Serial.print(",");
    Serial.print("RegStatus"); Serial.print(",");
  
    //GPS data
    Serial.print("gpsyear"); Serial.print(",");
    Serial.print("gpsmonth"); Serial.print(",");
    Serial.print("gpsday"); Serial.print(",");
    Serial.print("gpshour"); Serial.print(",");
    Serial.print("gpsmin"); Serial.print(",");
    Serial.print("gpssec"); Serial.print(",");
    Serial.print("latitude"); Serial.print(",");
    Serial.print("longitude"); Serial.print(",");
    Serial.print("course"); Serial.print(",");
    Serial.print("speed"); Serial.print(",");
    Serial.print("alt"); Serial.print(",");
    Serial.print("sat"); Serial.print(",");
  
    //Battery data
    Serial.print("Batv"); Serial.print(",");
  
    //Fluorometer data
    Serial.print("ChlAvg"); Serial.print(",");
    Serial.print("ChlStd"); Serial.print(",");
    Serial.print("PhyAvg"); Serial.print(",");
    Serial.print("PhyStd"); Serial.print(",");
  
    //PAR data
    Serial.print("PARAvg"); Serial.print(",");
    Serial.print("PARStd"); Serial.print(",");
  
    //temp data
    Serial.print("Temp0"); Serial.print(",");
    Serial.print("Temp1"); Serial.print(",");
    Serial.print("Temp2"); Serial.print(",");
    Serial.print("Temp3"); Serial.print(",");
    Serial.print("Temp4"); Serial.print(",");
  
    //RDO Data
    Serial.print("DOmgL"); Serial.print(",");
    Serial.print("DOTemp"); Serial.print(",");
    Serial.print("DOSat"); Serial.print(",");

    //Wave data
    Serial.print("Heading = "); Serial.println(WVHeading);
    Serial.print("WaveHt =  "); Serial.println(WVHT);
    Serial.print("Wave Direction =  "); Serial.println(WVDir);
    Serial.print("MeanWVDir =  "); Serial.println(MeanWVDir);
    Serial.print("GN =  "); Serial.println(GN);
    Serial.print("DP1 =  "); Serial.println(DP1);
    Serial.print("DP2 =  "); Serial.println(DP2);
  }
}
