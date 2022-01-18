/*****************************************BME680 Functions**************************************
The functions below read the fluorometers with autogaining 
*************************************************************************************************/
//Generic Function to read a fluorometer, take average readings and standard deviation
float FluorRead(int pin) {
  fluorStats.clear();
  float val = 0;
  for (int i = 0; i < 50; ++i) {
    val = readVolts(pin);
    fluorStats.add(val);
    delay(50);
  }
  FluorAvg = fluorStats.average();
  FluorStd = fluorStats.pop_stdev();
  return (FluorAvg);
  return (FluorStd);
  fluorStats.clear();
}

//function to read the chlorophyll fluorometer
void readChl() {
  tcaselect(7);
  mcp.pinMode(8, OUTPUT); //Controls relay to turn on chl fluorometer
  mcp.digitalWrite(8, HIGH);
  delay(100);

  //Set gain to 100X
  mcp.pinMode(0, INPUT); //To be floating for 100X gain
  mcp.pinMode(1, OUTPUT); //Grounded for 100X gain
  mcp.digitalWrite(1, LOW);
  GainVal = 0.01; //multiply by 0.01 for to normalize the reading by gain value of 100X
  delay(100);  //required after changing gains
  FluorRead(ChlPin);
  Serial.print("Initial Chl Reading =  "); Serial.println(FluorAvg);

  //auto gain
  if (FluorAvg < 4400) {
    ChlAvg = FluorAvg * GainVal;
    ChlStd = FluorStd * GainVal;
  }
  if (FluorAvg > 4400) {
    //decrease gain to 10X
    mcp.pinMode(0, OUTPUT);
    mcp.pinMode(1, INPUT);
    mcp.digitalWrite(0, LOW);
    delay(100);
    GainVal = 0.1;
    //get average
    FluorRead(ChlPin);
    ChlAvg = FluorAvg * GainVal;
    ChlStd = FluorStd * GainVal;
    Serial.print("10X Reading =  "); Serial.println(FluorAvg);
  }

  if (FluorAvg > 4400) {
    //decrease gain to 1X by floating both pins 5 and 6 on fluorometer
    GainVal = 1;
    mcp.pinMode(0, INPUT);
    mcp.pinMode(1, INPUT);
    delay(100);
    FluorRead(ChlPin);
    ChlAvg = FluorAvg * GainVal;
    ChlStd = FluorStd * GainVal;
    Serial.print("1X Reading= "); Serial.println(FluorAvg);
  }
  Serial.print("Chlorophyll Average= "); Serial.println(ChlAvg);
  Serial.print("Chlorophyll Standard Deviation = "); Serial.println(ChlStd);
  Serial.println("------------------------------------------");
  tcaselect(7);
  mcp.digitalWrite(8, LOW);
}


//function to read phycocyanin fluorometer
void readPhyco() {
  tcaselect(7);
  mcp.pinMode(9, OUTPUT); //Controls relay to turn on phyco fluorometer
  mcp.digitalWrite(9, HIGH);
  delay(100);

  //Set gain to 100X
  mcp.pinMode(2, INPUT); //To be floating for 100X gain
  mcp.pinMode(3, OUTPUT); //Grounded for 100X gain
  mcp.digitalWrite(1, LOW);
  GainVal = 0.01; //multiply by 0.01 for to normalize the reading by gain value of 100X
  delay(100);  //required after changing gains
  FluorRead(PhycoPin);
  Serial.print("Initial Phyco Reading =  "); Serial.println(FluorAvg);

  //auto gain
  if (FluorAvg < 4400) {
    PhycoAvg = FluorAvg * GainVal;
    PhycoStd = FluorStd * GainVal;
  }
  if (FluorAvg > 4400) {
    //decrease gain to 10X
    mcp.pinMode(2, OUTPUT);
    mcp.pinMode(3, INPUT);
    mcp.digitalWrite(0, LOW);
    delay(100);
    GainVal = 0.1;
    //get average
    FluorRead(PhycoPin);
    PhycoAvg = FluorAvg * GainVal;
    PhycoStd = FluorStd * GainVal;
    Serial.print("10X Reading =  "); Serial.println(FluorAvg);
  }

  if (FluorAvg > 4400) {
    //decrease gain to 1X by floating both pins 5 and 6 on fluorometer
    GainVal = 1;
    mcp.pinMode(0, INPUT);
    mcp.pinMode(1, INPUT);
    delay(100);
    FluorRead(PhycoPin);
    PhycoAvg = FluorAvg * GainVal;
    PhycoStd = FluorStd * GainVal;
    Serial.print("1X Reading= "); Serial.println(FluorAvg);
  }
  Serial.print("Phycocyanin Average= "); Serial.println(PhycoAvg);
  Serial.print("Phycocyanin Standard Deviation = "); Serial.println(PhycoStd);
  Serial.println("------------------------------------------");
  tcaselect(7);
  mcp.digitalWrite(9, LOW);
}