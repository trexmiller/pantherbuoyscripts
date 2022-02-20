void readWind(){
  
  tcaselect(7);
  mcp.pinMode(8,OUTPUT);
  delay(100);
  mcp.digitalWrite(8,HIGH);

  readWaves();
  sdi12.begin();

  unsigned int ATMONowTime = millis();
  unsigned int ATMOInterval = 5000; //Give sensor 5 seconds max to send good data
  float ATMOresult[9] = {0};
    
  WindSpeed = 9999;  //Set to -1 before new measurement
  WindDir = 9999;    //Set to -1 before new measurement
  while (ATMONowTime + ATMOInterval > millis()){ //timeout at interval
    if(WindSpeed == 9999 || WindDir == 9999 || WindSpeed == -9999 || WindDir == -9999){ 
      sdi12.clearBuffer();
      delay(30);
      String command1 = String(1) + "R0!"; 
      sdi12.sendCommand(command1);
      delay(30);
        
      for (uint8_t i = 0; i < 9; i++){
        ATMOresult[i] = sdi12.parseFloat();
       }
          
      WindSpeed = ATMOresult[1];
      WindDir = ATMOresult[2];
      TrueWindDir = WindDir + WVHeading;
      if (TrueWindDir > 360)
        TrueWindDir = TrueWindDir - 360;

      if (TrueWindDir < 0)
        TrueWindDir = TrueWindDir + 360;
    
      Gust = ATMOresult[3];
      ATMOAirT = ATMOresult[4];
      ATMOX = ATMOresult[5];
      ATMOY = ATMOresult[6];
        
      sdi12.clearBuffer();
    }
  }
  sdi12.end();
}
