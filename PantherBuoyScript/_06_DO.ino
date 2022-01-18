

void readDO() {
    Wire.begin();
    delay(100);
    tcaselect(7);
    mcp.begin();

    delay(1000);
    mcp.pinMode(6,OUTPUT);
    mcp.digitalWrite(6,HIGH);
    delay(1000);
    sdi12.begin();
    
    unsigned int RDONowTime = millis();
    unsigned int RDOInterval = 10000; //Give sensor 10 seconds to send good data
    char RDOData[100] = {0};
    DO = 0; //Set to zero before new measurement
    DOS = 0;  //Set to zero before new measurement
    RDOTemp = 0;  //Set to zero before new measurement
    int RDOLength = 0;
    if (RDONowTime + RDOInterval > millis()){ //timeout at interval
      while(DO == 0 || DOS == 0 || RDOTemp == 0){ 
        Serial.println("Requesting SDI12 sensor make a measurement");
        sdi12.clearBuffer();
        String command1 = "0M!"; //Tell sensor to make a measurement
        sdi12.sendCommand(command1);
        delay(3000); //RDO pro needs two seconds to make measurement

        Serial.println("Requesting SDI12 sensor send the data");
        sdi12.clearBuffer();
        String command2 = "0D0!"; //Tell sensor to send us the data
        sdi12.sendCommand(command2);
        delay(100);
      
        String sdiResponse = sdi12.readStringUntil('\n'); //Read data until new line
        sdiResponse.toCharArray(RDOData,100);
        Serial.print("SDI Response = "); Serial.println(RDOData);
        
        char * strtokIndx;
        strtokIndx = strtok(RDOData,"+");      // get data up to first "+"
        
        char Address[1];
        strcpy(Address, strtokIndx); 
          
        strtokIndx = strtok(NULL, "+"); // get next bit of data up to next "+"
        DO = atof(strtokIndx); 
          
        strtokIndx = strtok(NULL, "+"); // get next bit of data up to next "+"
        DOS = atof(strtokIndx); 
          
        strtokIndx = strtok(NULL, "+"); // get next bit of data up to next "+"
        RDOTemp = atof(strtokIndx); 
          
        sdi12.clearBuffer();   
      } 
    }
    Serial.println("");
    tcaselect(7);
    mcp.digitalWrite(6,LOW);
}
