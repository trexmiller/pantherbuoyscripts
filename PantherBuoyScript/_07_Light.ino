void readPARa() {
  ads.setGain(GAIN_TWOTHIRDS);
  ads.begin();
  paraStats.clear();
  PARa = 0;
  for (int i = 0; i < 50; ++i) {
    PARa0 = ads.readADC_SingleEnded(0);
    LightRef = ads.readADC_SingleEnded(3);
    PARa = ((PARa0*0.1875)-(LightRef*0.1875));
    paraStats.add(PARa);
    delay(10);
  }
  paraAvg = paraStats.average();
  paraStd = paraStats.pop_stdev();
  paraStats.clear();
}

void readIR() {
  ads.setGain(GAIN_TWOTHIRDS);
  ads.begin();
  irStats.clear();
  IR = 0;
  for (int i = 0; i < 50; ++i) {
    IR0 = ads.readADC_SingleEnded(1);
    LightRef = ads.readADC_SingleEnded(3);
    IR = ((IR0*0.1875)-(LightRef*0.1875));
    irStats.add(IR);
    delay(10);
  }
  IRAvg = irStats.average();
  IRStd = irStats.pop_stdev();
  irStats.clear();
}

void readUV() {
  ads.setGain(GAIN_FOUR); 
  ads.begin();
  uvStats.clear();
  UV = 0;
  for (int i = 0; i < 50; ++i) {
    UV0 = ads.readADC_SingleEnded(2);
    LightRef = ads.readADC_SingleEnded(3);
    UV = ((UV0*0.1875)-(LightRef*0.1875));
    uvStats.add(UV);
    delay(10);
  }
  UVAvg = uvStats.average();
  UVStd = uvStats.pop_stdev();
  if (UVAvg < 0){
    UVAvg = 0;
    }
  uvStats.clear();
}

void readLight(){
  unsigned int LightNowTime = millis();
  unsigned int LightInterval = 10000; //Give sensor 10 seconds to get data otherwise move on
  while (LightNowTime + LightInterval > millis()){
    tempsensor.begin(0x18);
    tempsensor.setResolution(3);
    tcaselect(7);
    delay(100);
    mcp.pinMode(10,OUTPUT);
    mcp.digitalWrite(10,HIGH);
    delay(1000);

    tcaselect(3);
    delay(100);
    tempsensor.wake();
    delay(1000);
    LightTemp = tempsensor.readTempC();
    readPARa();
    readIR();
    readUV();
  }
}
