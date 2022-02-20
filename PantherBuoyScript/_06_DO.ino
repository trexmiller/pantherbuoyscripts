

void readDO() {
    tcaselect(7);
    delay(100);
    mcp.pinMode(6,OUTPUT);
    mcp.digitalWrite(6,HIGH);
    delay(2000);
    
    sdi12.begin();
    
    unsigned int RDONowTime = millis();
    unsigned int RDOInterval = 10000; //Give sensor 10 seconds to send good data
    float RDOData[4] = {0};
    
    DO = 0; //Set to zero before new measurement
    DOS = 0;  //Set to zero before new measurement
    RDOTemp = 0;  //Set to zero before new measurement
    while (RDONowTime + RDOInterval > millis()){ //timeout at interval
      if (DO == 0 || DOS == 0 || RDOTemp == 0){ 
        Serial.print(F("."));
        sdi12.clearBuffer();
        String command1 = "0M!"; //Tell sensor to make a measurement
        sdi12.sendCommand(command1);
        delay(3000); //RDO pro needs two seconds to make measurement
        
        
        sdi12.clearBuffer();
        String command2 = "0D0!"; //Tell sensor to send us the data
        sdi12.sendCommand(command2);
        delay(100);

        for (uint8_t i = 0; i < 4; i++){
          RDOData[i] = sdi12.parseFloat();
        }

        DO = RDOData[1];
        DOS = RDOData[2];
        RDOTemp = RDOData[3];

        sdi12.clearBuffer();
        sdi12.end();   
      } 
    }
}
