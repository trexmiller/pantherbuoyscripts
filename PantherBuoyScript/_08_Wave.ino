const char * FirstNMEA;
const char * PSVSW = "$PSVSW";
const char * LastNMEA;
const char * UWPH2 = "UWPH2";

void parseWave(){
    String WaveReceived = ss.readStringUntil('\n');
    WaveReceived.toCharArray(WaveData,200);
    Serial.print("WaveData = "); Serial.println(WaveData);
    Serial.println("");
    
    char * strtokIndx; 
    
    strtokIndx = strtok(WaveData,","); 
    FirstNMEA = strtokIndx;

    strtokIndx = strtok(NULL,","); 
    WVHeading = atof(strtokIndx);
    
    strtokIndx = strtok(NULL, ",");
    WVHT = atof(strtokIndx);   

    strtokIndx = strtok(NULL, ",");
    WVPeriod = atof(strtokIndx); 

    strtokIndx = strtok(NULL, ",");
    WVDir = atof(strtokIndx);

    strtokIndx = strtok(NULL, ",");
    MeanWVDir = atof(strtokIndx); 

    strtokIndx = strtok(NULL, ",");
    GN = atof(strtokIndx);

    strtokIndx = strtok(NULL, ",");
    DP1 = atof(strtokIndx);

    strtokIndx = strtok(NULL, ",");
    DP2 = atof(strtokIndx);

    strtokIndx = strtok(NULL, ",");
    LastNMEA = strtokIndx;
}

void readWaves(){
  tcaselect(7);
  delay(1000);

  mcp.pinMode(4,OUTPUT);
  mcp.digitalWrite(4,LOW);
  delay(1000);
  
  mcp.pinMode(14,OUTPUT);
  mcp.digitalWrite(14,HIGH);
  delay(1000);
  
  mcp.pinMode(15,OUTPUT);
  mcp.digitalWrite(15,LOW);
  delay(1000);

  tcaselect(7);
  delay(100);
  mcp.pinMode(8,OUTPUT);
  mcp.digitalWrite(8,HIGH);
  delay(1000);

  unsigned int WVNowTime = millis();
  unsigned int WVInterval = 10000; //Give sensor 10 seconds to send good data

  FirstNMEA = "XXX";
  while(WVNowTime + WVInterval > millis()){ //timeout at interval
    if(strcmp(FirstNMEA,PSVSW) != 0 & strcmp(LastNMEA,UWPH2) !=0){ //If the NMEA sentence is not correct and if not correct length then go get data again
      parseWave();
    }  
  }
}

void readRawWave(){
  Serial.println("Preparing to read raw wave sensor messages");
  Serial.println("Enter any character to stop");
  tcaselect(7);
  delay(1000);

  mcp.pinMode(4,OUTPUT);
  mcp.digitalWrite(4,LOW);
  delay(1000);
  
  mcp.pinMode(14,OUTPUT);
  mcp.digitalWrite(14,HIGH);
  delay(1000);
  
  mcp.pinMode(15,OUTPUT);
  mcp.digitalWrite(15,LOW);
  delay(1000);

  tcaselect(7);
  delay(100);
  mcp.pinMode(8,OUTPUT);
  delay(1000);
  mcp.digitalWrite(8,HIGH);
  delay(5000);
  
  staticTime = millis();
  while (staticTime+(1000*60*10) > millis() & Serial.available() == 0){
  if (Serial.available()) {
    char c = Serial.read();
    ss.write(c);
  }
  if (ss.available()) {
    char c = ss.read();
    Serial.write(c);

  }
 }
}
  
