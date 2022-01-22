/*****************************MENU***********************************************************/

/***********************Main Menu**************************************/
int mainChoice;
void menu(){ 
  Serial.println();
  Serial.println(F("=========================================================================="));
  Serial.println(F("Panther Buoy Version 2.001, Serial Number: 200120211228"));
  Serial.println(F("Firmware Version 20211226"));
  Serial.println(F("Invented by the Miller Laboratory, University of Wisconsin - Milwaukee"));
  Serial.println(F("==========================================================================="));
  Serial.println();
  Serial.println(F("You have entered the menu functions page."));
  Serial.println(F("SET SERIAL WINDOW OPTION TO NO LINE ENDING"));
  Serial.println(F("To navigate the menu options choose a number and press enter."));
  Serial.println();
  Serial.println(F("---MAIN MENU---"));
  Serial.println(F("(1) LCD Options"));
  Serial.println(F("(2) Print Results Options"));
  Serial.println(F("(3) Data Rate"));
  Serial.println(F("(4) Network Menu"));
  Serial.println(F("(5) System Date and Time Menu"));
  Serial.println(F("(6) Sensors Menu"));
  Serial.println(F("(7) GPS Options"));
  Serial.println(F("(8) SD Card Menu"));
  Serial.println(F("(9) Battery Voltage"));
  Serial.println(F("(A) Auxiliary Functions"));
  Serial.println(F("(&) Restart the device"));
  Serial.println(F("(#) Deploy"));
  Serial.print(F(""));
  
  mainMenuChoice();
  switch (mainChoice) {
  case '1':   
    LCDOption();
  break;  
  case '2':   
    printResultsOptions();
  break;
  case '3':   
    Serial.println(F("Enter a data rate in seconds. The fastest data rate possible is every 120 seconds so enter 120 or greater."));
    intervalOptions();  
  break;
  case '4':
    Serial.println();    
    networkMenu();
  break;                          
  case '5':
  Serial.println(F("********************************************************************************"));
    Serial.println();
    Serial.print(F("Current System Time: "));   
    Serial.print(F("20"));Serial.print(rtc.getYear()); Serial.print(F("-"));
    Serial.print(rtc.getMonth()); Serial.print(F("-"));
    Serial.print(rtc.getDay()); Serial.print(F(" "));
    Serial.print(rtc.getHours()); Serial.print(F(":"));
    Serial.print(rtc.getMinutes()); Serial.print(F(":"));
    Serial.print(rtc.getSeconds()); Serial.println();
    Serial.println(F("********************************************************************************"));
    DateTimeMenu();
  return menu();
  break;
  case '6': 
    sensorMenu();
  break;
  case '7': 
    GPSMenu();
  break;
  case '8':
    sdMenu();
  break;
  case '9':
    Serial.println(); 
    Serial.println(F("********************************************************************************"));
    Serial.print(F("Battery Voltage (mV): ")); 
    readBat();
    Serial.println(Batv);
    Serial.println(F("********************************************************************************"));
  return menu();
  break;
  case 'A':
    Serial.println(); 
    AuxMenu();
  break;
  case '&':
    countdownMS = Watchdog.enable(5000);
    Serial.println(F("System will reset in 5 seconds"));
  break;
  case '#':
  break;
  } 
  //Just go on with setup now
}

int mainMenuChoice(){
  while(Serial.available() > 0){
    char t = Serial.read(); // make sure buffer is clear
  }
                               // before waiting for input  
  while(Serial.available() == 0);
  mainChoice = Serial.read();
  return(mainChoice);
}


/***********************LCD Submenu**************************************/
int LCDChoice;
void LCDOption(){
  Serial.println();
  Serial.println(F("---LCD Options---"));
  Serial.println(F("(1) Use LCD (Default)"));
  Serial.println(F("(2) Do Not Use LCD"));
  Serial.println(F("(*) Back to Main Menu"));

  LCDMenuChoice();
  switch (LCDChoice) {
  case '1':
      doLCD = 1;   
    return menu();
  case '2':  
      doLCD = 0;
    return menu();
    break;
  case '*':     
    break; //We break here with nothing else and defaults back to main menu
    }
  return menu();
}

int LCDMenuChoice(){
   if(Serial.available() > 0);            
   Serial.read();
   
   // before waiting for input  
   while(Serial.available() == 0);
    LCDChoice = Serial.read();
    return(LCDChoice);
}

/***********************Print Results Submenu**************************************/
int printResultsChoice;
void printResultsOptions(){
  
  Serial.println();
  Serial.println(F("---Print Results Options---"));
  Serial.println(F("(1) Print All Results to Serial Window (Default)"));
  Serial.println(F("(2) Do Not Print All Results to Serial Window"));
  Serial.println(F("(*) Back to Main Menu"));

  printResultsMenuChoice();
  switch (printResultsChoice) {
  case '1':
      Verbose = 1;   
    return menu();
  case '2':  
      Verbose = 0;
    return menu();
    break;
  case '*':     
    break; //We break here with nothing else and defaults back to main menu
    }
  return menu();
}

int printResultsMenuChoice(){
   if(Serial.available() > 0);            
   Serial.read();
   
   // before waiting for input  
   while(Serial.available() == 0);
    printResultsChoice = Serial.read();
    return(printResultsChoice);
}

/***********************Date and Time Submenu**************************************/
int DateTimeChoice;
char read_array[4];    
void DateTimeMenu(){ 
  Serial.println();
  Serial.println(F("---Choose A Menu Item---"));
  Serial.println(F("(1) View Current System Date and Time"));
  Serial.println(F("(2) Set Date And Time With Modem"));
  Serial.println(F("(3) Set Date And Time With GPS"));
  Serial.println(F("(4) Set Date and Time Manually"));
  Serial.println(F("(*) Back to Main Menu"));
  Serial.println(F(""));
  Serial.println(F("NOTE: During deployment time is tracked with real time clock software."));
  Serial.println(F("The date and time will be set by the modem every time the modem connects."));
  Serial.println(F("The modem will get the date and time from GNSS."));

  DateTimeMenuChoice();
  switch (DateTimeChoice) {
  case '1':
    Serial.println(F("********************************************************************************"));
    Serial.println();
    Serial.print(F("Current System Time: "));   
    Serial.print(F("20"));Serial.print(rtc.getYear()); Serial.print(F("-"));
    Serial.print(rtc.getMonth()); Serial.print(F("-"));
    Serial.print(rtc.getDay()); Serial.print(F(" "));
    Serial.print(rtc.getHours()); Serial.print(F(":"));
    Serial.print(rtc.getMinutes()); Serial.print(F(":"));
    Serial.print(rtc.getSeconds()); Serial.println();
    Serial.println(F("********************************************************************************")); 
  return DateTimeMenu();
  break;
  case '2': 
    Serial.print(F("Setting date and time with modem (please wait).")); 
    SetTime(); 
  return DateTimeMenu();
  break;
  case '3':  
      Serial.println("Set date time with GPS (this is not yet implemented)"); 
  return DateTimeMenu();
  break;
  case '4':  
      Serial.println(F("."));
      Serial.println(F("."));
      Serial.println(F("."));
      Serial.println(F(".")); 
      Serial.println(F("")); 
      Serial.println(F("Set date and time manually")); 
      RTC_Enter();
      
  return DateTimeMenu();
  break;
  case '*':     
    break; //We break here with nothing else and defaults back to main menu
    }
  return menu();
}

int DateTimeMenuChoice(){
   if(Serial.available() > 0);            
   Serial.read();
   
   // before waiting for input  
   while(Serial.available() == 0);
    DateTimeChoice = Serial.read();
    return(DateTimeChoice);
}

void readInput() {
  while (Serial.available() == 0);       // wait for user input
  for (int i = 0; i <= 4; i++) {
    read_array[i] = Serial.read();       // read incoming data
    delay(10);
  }
}

void RTC_Enter() {
  byte year;
  byte month = 1;
  byte day = 1;
  byte hour = 1;
  byte minute = 0;
  byte second = 0;
  Serial.println(F("Enter the current two digit year without century (YY)"));
  while (Serial.available() > 0) {        // make sure buffer is clear
    Serial.read();                        // before waiting for input
  }
  readInput();
  year =  atoi(read_array);              // array to int
  rtc.setYear(year);
  delay(100);
  Serial.print("Year is set to: "); Serial.println(rtc.getYear());
  Serial.println(F("Enter the current month (1 - 12)"));
  while (Serial.available() > 0) {       // make sure buffer is clear
    Serial.read();                       // before waiting for input
  }
  readInput();
  month = atoi(read_array);              // array to int
  if (month >= 1 && month <= 12) {
    rtc.setMonth(month);
    Serial.print("Month set to: "); Serial.println(rtc.getMonth());
  }
  else {
    Serial.println(("start again"));
    delay(3000);
    RTC_Enter();
  }
  // DAY
  Serial.println(("Enter the current day (1 - 31)"));
  while (Serial.available() > 0) {      // make sure buffer is clear
    Serial.read();                      // before waiting for input
  }
  readInput();
  day = atoi(read_array);              // array to int
  if (day >= 1 && day <= 31) {
    rtc.setDay(day);
    Serial.print("Day set to: "); Serial.println(rtc.getDay());
  }
  else {
    Serial.println(("start again"));
    delay(3000);
    RTC_Enter();
  }
  // Hour
  Serial.println(("Enter the current hour (1-24)"));
  while (Serial.available() > 0) {               // make sure buffer is clear
    Serial.read();                             // before waiting for input
  }
  readInput();
  hour = atoi(read_array);  // array to int
  if (hour >= 1 && hour <= 24) {
    rtc.setHours(hour);
    Serial.print("Hour set to: "); Serial.println(rtc.getHours());
  }
  else {
    Serial.println(("try again"));
    delay(3000);
    RTC_Enter();
  }
  Serial.println(("Enter the current minute (0-59)"));
  while (Serial.available() > 0) {                // make sure buffer is clear
    Serial.read();                             // before waiting for input
  }
  readInput();
  minute = atoi(read_array);  // array to int
  if (minute >= 0 && minute <= 59) {
    rtc.setMinutes(minute);
    Serial.print("Minute set to: "); Serial.println(rtc.getMinutes());
  }
  else {
    Serial.println(("try again"));
    delay(3000);
    RTC_Enter();
  }
  Serial.println(("Enter the current second (0-59)"));
  while (Serial.available() > 0) {               // make sure buffer is clear
    Serial.read();                             // before waiting for input
  }
  readInput();
  second = atoi(read_array);  // array to int
  if (second >= 0 && second <= 59) {
    rtc.setSeconds(second);
    Serial.print("Second set to: "); Serial.println(rtc.getSeconds());
  }
  else {
    Serial.println(("try again"));
    delay(3000);
    RTC_Enter();
  }
  print_timestamp();
}

void print_timestamp() { // gets current date and time from the SD card
  Serial.print("Date Time = "); Serial.print("20"); Serial.print(rtc.getYear());
  Serial.print("-");
  Serial.print(rtc.getMonth());
  Serial.print("-");
  Serial.print(rtc.getDay());
  Serial.print(" ");
  Serial.print(rtc.getHours());
  Serial.print(":");
  Serial.print(rtc.getMinutes());
  Serial.print(":");
  Serial.println(rtc.getSeconds());
  Serial.print("Millis = "); Serial.print(millis()); Serial.println(" milliseconds since system start");
}

/***********************Interval Submenu**************************************/
int intervalChoice;
void intervalOptions(){ 
  Serial.println();
  Serial.println(F("---Choose A Data Rate---"));
  Serial.println(F("(1) Every 2 minutes (max data rate)"));
  Serial.println(F("(2) Every 5 minutes"));
  Serial.println(F("(3) Every 10 minutes"));
  Serial.println(F("(4) Every 30 minutes"));
  Serial.println(F("(5) Every 60 minutes"));
  Serial.println(F("(*) Back to Main Menu"));

  intervalOptionsChoice();
  switch (intervalChoice) {
  case '1':
      Interval = 1000*60*2;   
    return intervalOptions();
    break;
  case '2':  
      Interval = 1000*60*5;  
    return intervalOptions();
    break;
  case '3':  
      Interval = 1000*60*10;  
    return intervalOptions();
    break;
   case '4':  
      Interval = 1000*60*30;  
    return intervalOptions();
    break;
   case '5':  
      Interval = 1000*60*60;  
    return intervalOptions();
    break;
  case '*':     
    break; //We break here with nothing else and defaults back to main menu
    }
  return menu();
}

int intervalOptionsChoice(){
   if(Serial.available() > 0);            
   Serial.read();
   
   // before waiting for input  
   while(Serial.available() == 0);
    intervalChoice = Serial.read();
    return(intervalChoice);
}

/***********************Network Submenu**************************************/
int networkChoice;
void networkMenu(){
  Serial.println();
  Serial.println(F("Use the network menu to test modem functions"));
  Serial.println(F("To navigate the menu options choose a number and press enter."));
  Serial.println();
  Serial.println(F("---NETWORK MENU---"));
  Serial.println(F("(1) Cellular signal"));
  Serial.println(F("(2) Network Reg Status"));
  Serial.println(F("(3) SIM Card"));
  Serial.println(F("(4) Modem Info"));
  Serial.println(F("(5) Modem Temperature"));
  Serial.println(F("(6) Restart Modem"));
  Serial.println(F("(7) Power Down Modem"));
  Serial.println(F("(*) Back to Main Menu"));

  networkMenuChoice();
  switch (networkChoice) {
  case '1':   
    Serial.println();
    Serial.println(F("********************************************************************************"));  
    GetCSQ();
    Serial.print("Cell Strength = ");
    Serial.println(RSI);
    Serial.println(F("********************************************************************************"));
    return networkMenu();
    break;
  case '2':  
      Serial.println(); 
      Serial.println(F("********************************************************************************"));
      GetReg();
      Serial.print(F("Registration Status = ")); Serial.println(F(RegStatus));
      Serial.println(F(""));
      Serial.println(F("0 = Not registered, ME is not currently searching a new operator to register to"));
      Serial.println(F("1 = Registered, home network"));
      Serial.println(F("2 = Not registered, but ME is currently searching a new operator to register to"));
      Serial.println(F("3 = Registration denied"));
      Serial.println(F("4 = Unknown"));
      Serial.println(F("5 = Registered, roaming"));
      Serial.println(F("********************************************************************************"));
    return networkMenu();
    break;
  case '3':  
      Serial.println(); 
      Serial.println(F("********************************************************************************"));
      GetSIM();
      Serial.print(F("SIM Card ID = ")); Serial.println(SIM);
      Serial.println(F("********************************************************************************"));
    return networkMenu();
    break; 
  case '4': 
      Serial.println();  
      Serial.println(F("********************************************************************************"));  
      ModemInfo();
      Serial.println(F("********************************************************************************"));
    return networkMenu();
    break; 
  case '5': 
      Serial.println(); 
      Serial.println(F("********************************************************************************")); 
      GetModemTemp();  
      Serial.print("Modem Temperature = "); Serial.print(ModemTemp); Serial.println(F(" degrees C"));
      Serial.println(F("********************************************************************************"));
    return networkMenu();
    break;
  case '6': 
      Serial.println();
      Serial.println(F("Restarting modem (please wait)")); 
      RestartModem();  
  return networkMenu();
  break;
  case '7': 
      Serial.println();
      Serial.println(F("Turning modem off")); 
      ModemOff();  
  return networkMenu();
  break;
  case '*':     
    break; //We break here with nothing else and defaults back to main menu
    }
  return menu();
 }

int networkMenuChoice(){
   if(Serial.available() > 0);            
   Serial.read();
   
   // before waiting for input  
   while(Serial.available() == 0);
    networkChoice = Serial.read();
    return(networkChoice);
}


/***********************GPS Submenu**************************************/
int GPSChoice;
void GPSMenu(){
  Serial.println();
  Serial.println(F("---GPS MENU---"));
  Serial.println(F("(1) Measure GPS Variables"));
  Serial.println(F("(2) Attempt to Get Satelite Fix (10 Min Timeout)"));
  Serial.println(F("(3) Halt Program Start to Wait for GPS Fix"));
  Serial.println(F("(4) Do Not Halt for GPS Fix"));
  Serial.println(F("(*) Return to Main Menu"));

  GPSMenuChoice();
  switch (GPSChoice) {
  case '1':  
    Serial.println();
    Serial.println(F("********************************************************************************"));  
    readGPS();
    
    Serial.print(F("gpsyear = ")); Serial.println(GPSYear);
    Serial.print(F("gpsmonth = ")); Serial.println(GPSMonth);
    Serial.print(F("gpsday = ")); Serial.println(GPSDay);
    Serial.print(F("gpshour = ")); Serial.println(GPSHour);
    Serial.print(F("gpsmin = ")); Serial.println(GPSMin);
    Serial.print(F("gpssec = ")); Serial.println(GPSSec);
    Serial.print(F("latitude = ")); Serial.println(Lat);
    Serial.print(F("longitude = ")); Serial.println(Lon);
    Serial.print(F("course = ")); Serial.println(Course);
    Serial.print(F("speed = ")); Serial.println(Speed);
    Serial.print(F("alt = ")); Serial.println(Alt);
    Serial.print(F("sat = ")); Serial.println(Sat);
   
    Serial.println(F("********************************************************************************"));
    return GPSMenu();
    break;
  case '2':  
      Serial.println(); 
      Serial.println(F("********************************************************************************"));
      TestGPS();
      Serial.println(F("********************************************************************************"));
    return GPSMenu();
    break;
  case '3':  
      Serial.println(); 
      Serial.println(F("********************************************************************************"));
      GPSHold = 1;
      Serial.println(F("The program will wait for GPS satelite fix (GPSHold  = 1"));
      Serial.println(F("********************************************************************************"));
    return GPSMenu();
    break;
  case '4':  
      Serial.println(); 
      Serial.println(F("********************************************************************************"));
      GPSHold = 0;
      Serial.println(F("The program will NOT wait for GPS satelite fix (GPSHold  = 0"));
      Serial.println(F("********************************************************************************"));
    return GPSMenu();
    break; 
    case '*':     
    break; //We break here with nothing else and defaults back to main menu
    }
  return menu();
 }

int GPSMenuChoice(){
   if(Serial.available() > 0);            
   Serial.read();
   
   // before waiting for input  
   while(Serial.available() == 0);
    GPSChoice = Serial.read();
    return(GPSChoice);
}

//Test GPS

void TestGPS(){
  GPSON();
  staticTime = millis();
  while (staticTime+GPSHold > millis()){
      int SatSetup = -1;
      while (SatSetup < 5) {
        while (ss.available() > 0)
          gps.encode(ss.read());
        SatSetup = gps.satellites.value();
        Serial.print(F("Waiting for GPS satellite fix. ")); Serial.print(F("GPS Satellites = ")); Serial.println(SatSetup);
        delay(1000);
      }

      float LatSetup = -1;
      while (LatSetup < 0) {
        while (ss.available() > 0)
          gps.encode(ss.read());
          LatSetup = gps.location.lat();
          Serial.print(F("Waiting for location update. ")); Serial.print(F("Latitude = ")); Serial.print(LatSetup); Serial.print(F(", Longitude = ")); Serial.println(gps.location.lng());
        }
    }
  GPSOFF();
}


/**********Auxiliary Tools Menu***********/
int AuxChoice;
void AuxMenu(){
  Serial.println();
  Serial.println(F("---Auxiliary Tools---"));
  Serial.println(F("(1) Turn on the 3.3V1 switched power rail"));
  Serial.println(F("(2) Turn on the 3.3V2 switched power rail"));
  Serial.println(F("(3) Turn on the 5V1 switched power rail"));
  Serial.println(F("(4) Turn on the 5V2 switched power rail"));
  Serial.println(F("(5) Turn on the 5V3 switched power rail"));
  Serial.println(F("(6) Turn on the 12V switched power rail"));
  Serial.println(F("(7) Turn off all switched power rails"));
  Serial.println(F("(*) Back to Main Menu"));

  AuxMenuChoice();
  switch (AuxChoice) {
  case '1':
      tcaselect(7);
      mcp.pinMode(11,OUTPUT);
      mcp.digitalWrite(11,HIGH);
      delay(1000);
      Serial.println(F("The 3.3VS1 switched power rail is on!"));
    return AuxMenu();
  case '2':  
      tcaselect(7);
      mcp.pinMode(13,OUTPUT);
      mcp.digitalWrite(13,HIGH);
      delay(1000);
      Serial.println(F("The 3.3VS2 switched power rail is on!"));
    return AuxMenu();
    break;
  case '3':  
      tcaselect(7);
      mcp.pinMode(8,OUTPUT);
      mcp.digitalWrite(8,HIGH);
      delay(1000);
      Serial.println(F("The 5VS1 switched power rail is on!"));
    return AuxMenu();
    break;
  case '4':  
      tcaselect(7);
      mcp.pinMode(9,OUTPUT);
      mcp.digitalWrite(9,HIGH);
      delay(1000);
      Serial.println(F("The 5VS2 switched power rail is on!"));
    return AuxMenu();
    break;
  case '5':  
      tcaselect(7);
      mcp.pinMode(10,OUTPUT);
      mcp.digitalWrite(10,HIGH);
      delay(1000);
      Serial.println(F("The 5VS3 switched power rail is on!"));
    return AuxMenu();
    break;
  case '6':  
      tcaselect(7);
      mcp.pinMode(6,OUTPUT);
      mcp.digitalWrite(6,HIGH);
      delay(1000);
      Serial.println(F("The 12V switched power rail is on!"));
    return AuxMenu();
    break;
  case '7':  
      tcaselect(7);
      pinsLow();
      delay(1000);
      Serial.println(F("All switched power rails are off!"));
    return AuxMenu();
    break;
  case '*':     
    break; //We break here with nothing else and defaults back to main menu
    }
  return menu();
}

int AuxMenuChoice(){
   if(Serial.available() > 0);            
   Serial.read();
   
   // before waiting for input  
   while(Serial.available() == 0);
    AuxChoice = Serial.read();
    return(AuxChoice);
}


  
/***********************Sensors Submenu**************************************/
int sensorChoice;
void sensorMenu(){
  Serial.println(F("Select a sensor to test."));
  Serial.println();
  Serial.println(F("---SENSOR MENU---"));
  Serial.println(F("(0) Read DO"));
  Serial.println(F("(1) Read Wind"));
  Serial.println(F("(2) Read Temp Chain"));
  Serial.println(F("(3) Read Chlorophyll"));
  Serial.println(F("(4) Read Phycocyanin"));
  Serial.println(F("(5) Read Air Temp, Humidity, Pressure, Altitude, and Air Quality Index"));
  Serial.println(F("(6) Read PAR"));
  Serial.println(F("(7) Read Panel Temp and Humidity"));
  Serial.println(F("(8) Read IMU"));
  Serial.println(F("(9) Calibrate Sensors"));
  Serial.println(F("(*) Back to Main Menu"));

  sensorMenuChoice();
  switch (sensorChoice) {
  case '0':   
    Serial.println(); \
    Serial.println(F("********************************************************************************")); 
    Serial.println(F("Measuring dissolved oxygen sensor (please wait)"));
    readDO();
    Serial.print(F("DO (mg/L) = ")); Serial.println(DO); 
    Serial.print(F("DO Saturation (%) = ")); Serial.println(DOS); 
    Serial.print(F("Water Temperature (C) = ")); Serial.println(RDOTemp); 
    Serial.println(F("********************************************************************************"));
    Serial.println("");
    return sensorMenu();
    break;
  case '1':   
    Serial.println();  
    Serial.println(F("Measuring Wind Speed and Direction (please wait)"));
    readWindSpeed();
    readWindDir();
    Serial.print(F("Wind Speed = ")); Serial.print(WindSpeed); Serial.println(F(" (mph)"));
    Serial.print(F("Wind Direction = ")); Serial.print(WindDir); Serial.println(F(" (degrees)"));
    Serial.print(F("Buoy Heading = ")); Serial.print(Heading); Serial.println(F(" (degrees)"));
    Serial.print(F("Calibrated Wind Direction = ")); Serial.print(CalDirection); Serial.println(F(" (degrees)"));
    return sensorMenu();
    break;
  case '2':  
      Serial.println(); 
      Serial.println(F("********************************************************************************"));
      Serial.println(F("Reading temp chain sensors (please wait)"));
      readTemps();
      for (int i = 0; i < NSENSORS; i++) {
        Serial.print(F("Temp"));Serial.print(i);Serial.print(F("m = ")); Serial.println(Temp[i])  ;
        }
      Serial.println(F("********************************************************************************"));
    return sensorMenu();
    break;
  case '3':  
      Serial.println(); 
      Serial.println(F("********************************************************************************"));  
      Serial.println(F("Measuring chlorophyll (please wait)"));
      readChl();
      Serial.println(F("********************************************************************************"));
    return sensorMenu();
    break; 
  case '4': 
      Serial.println();    
      Serial.println(F("********************************************************************************"));
      Serial.println(F("Measuring phycocyanin (please wait)"));
      readPhyco();
      Serial.println(F("********************************************************************************"));
      
    return sensorMenu();
    break; 
  case '5':  
      Serial.println(); 
      Serial.println(F("********************************************************************************"));  
      Serial.println(F("Measuring air temperature, relative humidity, pressure, altitude and air quality index (please wait)"));
      readBME();  
      Serial.print(F("Air Temp (C) = ")); Serial.println(ATemp);
      Serial.print(F("Pressure (hPa) = ")); Serial.println(Pressure);
      Serial.print(F("Humidity (%) = ")); Serial.println(Humidity);
      Serial.print(F("Air Quality Index = ")); Serial.println(Gas);
      Serial.print(F("Estimated Altitude = ")); Serial.println(BMEAlt);
      Serial.println(F("")); 
      Serial.println(F("********************************************************************************"));
    return sensorMenu();
    break;  
  case '6': 
      Serial.println();    
      Serial.println(F("Measuring PAR (please wait)"));
      readPAR();
      Serial.print(F("PAR (average) = ")); Serial.print(PARAvg); Serial.println(F("(photons/m^2/s)"));
      Serial.print(F("PAR (std dev) = ")); Serial.print(PARStd); Serial.println(F("(photons/m^2/s)"));
    return sensorMenu();
    break;
  case '7':
      Serial.println(); 
      Serial.println(F("Measuring Panel Temp and Humidity (not working right now!)"));
    return sensorMenu();
    break;
  case '8':
      Serial.println(); 
      Serial.println(F("Measuring IMU (roll, pitch, and heading"));
      readIMU();
      Serial.print(F("Roll = ")); Serial.print(Roll); Serial.println(F("( degrees)"));
      Serial.print(F("Pitch = ")); Serial.print(Pitch); Serial.println(F("( degrees)"));
      Serial.print(F("Heading = ")); Serial.print(Heading); Serial.println(F(" (degrees)"));
    return sensorMenu();
    break; 
  case '9':
      Serial.println(); 
      calSensorsMenu();
    break;
  case '*':
    break; //We break here with nothing else and defaults back to main menu
    }
  return menu();
 }

int sensorMenuChoice(){
   if(Serial.available() > 0);            
   Serial.read();
   
   // before waiting for input  
   while(Serial.available() == 0);
    sensorChoice = Serial.read();
    return(sensorChoice);
}


/***********************Sensor Calibration Sub Sub Menu**************************************/
int calChoice;
void calSensorsMenu(){
  Serial.println(F("Select a sensor to calibrate."));
  Serial.println(F("All calibration data will be stored on the SD card and used for future deployments"));
  Serial.println();
  Serial.println(F("---Sensor Calibration MENU---"));
  Serial.println(F("(1) Calibrate Accelerometer"));
  Serial.println(F("(2) Calibrate Magnetometer"));
  Serial.println(F("(3) Calibrate Gyroscope"));
  Serial.println(F("(4) Calibrate Phycocyanin"));
  Serial.println(F("(5) Calibrate Chlorophyll"));
  Serial.println(F("(6) Calibrate PAR"));
  Serial.println(F("(7) View Calibration Coefficients"));
  Serial.println(F("(8) Read Temp Chain Node Addresses"));
  Serial.println(F("(*) Back to Sensor Menu"));

  calMenuChoice();
  switch (calChoice) {
  case '1':   
    Serial.println();  
    CalAccelPanther();
    
    return calSensorsMenu();
    break;
  case '2':  
      Serial.println(); 
      Serial.println(F("gyroscope"));
    return calSensorsMenu();
    break;
  case '3':  
      Serial.println(); 
      Serial.println(F("magnetometer"));
    return calSensorsMenu();
    break; 
  case '4': 
      Serial.println();    
      Serial.println(F("Calibrate phycocyanin... "));
    return calSensorsMenu();
    break; 
  case '5':  
      Serial.println();
      Serial.println();  
      Serial.println(F("********************************************************************************"));
      Serial.println(F("Welcome to the chlorophyll sensor calibration function."));
      Serial.println(F("Be sure to have chlorophyll standards prepared and the chlorophyll sensor connected to the buoy."));
      Serial.println(); 
      CalChl();
    return calSensorsMenu();
    break;  
  case '6': 
      Serial.println(); 
      Serial.println(F("Calibrate PAR"));
    return calSensorsMenu();
    break;
  case '7':
      Serial.println(); 
      calViewMenu();
    return calSensorsMenu();
    break;
  case '8':
      Serial.println(); 
      ReadTempNodeInfo();
  return calSensorsMenu();
  break;
  case '*':
    break; //We break here with nothing else and defaults back to main menu
    }
  return sensorMenu();
 }

int calMenuChoice(){
   if(Serial.available() > 0);            
   Serial.read();
   
   // before waiting for input  
   while(Serial.available() == 0);
    calChoice = Serial.read();
    return(calChoice);
}

/***********************View Calibration Sub Sub Sub Menu**************************************/
int calViewChoice;
void calViewMenu(){
  Serial.println(F("----Sensor Calibration Coefficients---"));
  Serial.println(F("(1) Accelerometer"));
  Serial.println(F("(2) Magnetometer"));
  Serial.println(F("(3) Gyroscope"));
  Serial.println(F("(4) Phycocyanin"));
  Serial.println(F("(5) Chlorophyll"));
  Serial.println(F("(6) PAR"));
  Serial.println(F("(7) Temp Chain Nodes"));
  Serial.println(F("(*) Back to Sensor Menu"));

  calViewMenuChoice();
  switch (calViewChoice) {
  case '1':   
    Serial.println();  
    readAccelOffset();
    return calViewMenu();
    break;
  case '2':  
      Serial.println(); 
      Serial.println(F("magnetometer"));
    return calViewMenu();
    break;
  case '3':  
      Serial.println(); 
      Serial.println(F("gyroscope"));
    return calViewMenu();
    break; 
  case '4': 
      Serial.println();    
      Serial.println(F("Calibrate phycocyanin... "));
    return calViewMenu();
    break; 
  case '5':  
      Serial.println(); 
      Serial.println(F("Calibrate Chlorophyll"));
    return calViewMenu();
    break;  
  case '6': 
      Serial.println(); 
      Serial.println(F("Calibrate PAR"));
    return calViewMenu();
    break;
  case '7':
      Serial.println(); 
      Serial.println(F("Assign temp chain nodes"));
    return calViewMenu();
    break;
  case '*':
    break; //We break here with nothing else and defaults back to main menu
    }
  return calSensorsMenu();
 }

int calViewMenuChoice(){
   if(Serial.available() > 0);            
   Serial.read();
   
   // before waiting for input  
   while(Serial.available() == 0);
    calViewChoice = Serial.read();
    return(calViewChoice);
}

/***********************SD Card Submenu**************************************/
int sdChoice;
void sdMenu(){
  Serial.println();
  Serial.println(F("---SD CARD MENU---"));
  Serial.println(F("(1) Card Info"));
  Serial.println(F("(2) List Files"));
  Serial.println(F("(3) Create Directory"));
  Serial.println(F("(4) Read File"));
  Serial.println(F("(5) Set Data File For Deployment"));
  Serial.println(F("(6) Delete File (Gone Forever!)"));
  Serial.println(F("(7) Read Accel Offset"));
  Serial.println(F("(*) Back to Main Menu"));

  sdMenuChoice();
  switch (sdChoice) {
  case '1':  
      Serial.println(F("********************************************************************************"));  
      SDInfo();
      Serial.println(F("********************************************************************************"));  
    return sdMenu();
    break;
  case '2':
      Serial.println(F("********************************************************************************"));  
      SDList();
      Serial.println(F("********************************************************************************")); 
    return sdMenu();
    break; 
  case '3': 
      Serial.println(F("********************************************************************************"));  
      sdCreateDir();
      Serial.println(F("********************************************************************************")); 
    return sdMenu();
    break; 
  case '4':  
      Serial.println();   
      readSD();
    return sdMenu();
    break;  
  case '5': 
      Serial.println();   
      Serial.println(F("Delete file..."));
    return sdMenu();
    break;  
  case '6': 
      Serial.println();   
      Serial.println(F("Format SD..."));
    return sdMenu();
    break;
   case '7': 
      Serial.println();   
      Serial.println(F("Reading Accel Offsets"));
      readAccelOffset();
    return sdMenu();
    break;
   case '*': 
    break;
    }
  return menu();
 }

int sdMenuChoice(){
   if(Serial.available() > 0);            
   Serial.read();
   
   // before waiting for input  
   while(Serial.available() == 0);
    sdChoice = Serial.read();
    return(sdChoice);
}

/***********************************************End Menu**************************************/
