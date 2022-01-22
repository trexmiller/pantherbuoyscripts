//The function below sets the date and time of the RTC using time reported by the modem
//This happens one time during setup.
//Before deployment the buoy operator should check that the correct time has been set either in serial monitor or on the LCD
void SetTime() {
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  delay(1000);
  pinMode(5, INPUT);
  delay(100);

  sendReceiveATCommand("AT", "\r\nOK", "\r\nERROR", 3000);
  sendReceiveATCommand("AT+CCLK?", "\r\nOK", "\r\nERROR", 3000);
  char* strtokIndx;

  strtokIndx = strtok(result_char, "\"");
  strcpy(CCLK, strtokIndx);


  //Date message
  strtokIndx = strtok(NULL, ",");
  strcpy(Date, strtokIndx);
  Serial.print("Date = "); Serial.println(Date);

  //Time message
  strtokIndx = strtok(NULL, ",");
  strcpy(Time, strtokIndx);
  Serial.print("Time = "); Serial.println(Time);

  //Get year from date message
  strtokIndx = strtok(Date, "/");
  Year = atoi(strtokIndx);
  rtc.setYear(Year);


  //Get month from date message
  strtokIndx = strtok(NULL, "/");
  Month = atoi(strtokIndx);
  rtc.setMonth(Month);


  //Get day from date message
  strtokIndx = strtok(NULL, "/");
  Day = atoi(strtokIndx);
  rtc.setDay(Day);


  //Get hour from time message
  strtokIndx = strtok(Time, ":");
  Hour = atoi(strtokIndx);
  rtc.setHours(Hour);


  //Get minute from time message
  strtokIndx = strtok(NULL, ":");
  Minute = atoi(strtokIndx);
  rtc.setMinutes(Minute);


  //Get second from time message
  strtokIndx = strtok(NULL, ":");
  strcpy(Sec, strtokIndx);


  //Get second from time message
  strtokIndx = strtok(Sec, "-");
  Second = atoi(strtokIndx);
  rtc.setSeconds(Second);

}

//Function to read the cellular signal strength
char* CSQ1;
char* CSQ2;
int RSI;
void GetCSQ() {
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  delay(100);
  pinMode(5, INPUT);
  modem.init();
  sendReceiveATCommand("AT+CSQ", "\r\nOK", "\r\nERROR", 3000);
  CSQ1 = strtok(result_char, ":,");
  CSQ2 = strtok(NULL, ",");
  RSI = atoi(CSQ2);
}

//Function to read SIM card number
char* SIM1;
char* SIM;
void GetSIM() {
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  delay(100);
  pinMode(5, INPUT);
  modem.init();
  sendReceiveATCommand("AT+CCID", "\r\nOK", "\r\nERROR", 3000);
  SIM1 = strtok(result_char, ":,");
  SIM = strtok(NULL, "\n\r");
}

void RestartModem(){
  sendReceiveATCommand("AT+CFUN=1,1", "\r\nOK", "\r\nERROR", 3000);
}

void CellEnv(){
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  delay(100);
  pinMode(5, INPUT);
  modem.init();
  sendReceiveATCommand("AT+KCELL", "\r\nOK", "\r\nERROR", 3000);
}
 


//Function to read SIM card number
void ModemInfo() {
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  delay(100);
  pinMode(5, INPUT);
  modem.init();
  sendReceiveATCommand2("AT+GMI", "\r\nOK", "\r\nERROR", 3000);
  sendReceiveATCommand2("AT+GMM", "\r\nOK", "\r\nERROR", 3000);
  sendReceiveATCommand2("AT+CGMR", "\r\nOK", "\r\nERROR", 3000);
}

//Function to read modem temperature
char* ModemTemp1;
char* ModemTemp2;
char* ModemTemp3;
int ModemTemp;
void GetModemTemp() {
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  delay(100);
  pinMode(5, INPUT);
  modem.init();
  sendReceiveATCommand("AT+KTEMPMON=1", "\r\nOK", "\r\nERROR", 3000);
  ModemTemp1 = strtok(result_char, ":,");
  ModemTemp2 = strtok(NULL, ",");
  ModemTemp3 = strtok(NULL, ",");
  ModemTemp = atoi(ModemTemp3);
}

//Function to read registration status on the cellular network
char* RegStatus1;
char* RegStatus2;
char* RegStatus3;
char* RegStatus;
void GetReg() {
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  delay(100);
  pinMode(5, INPUT);
  modem.init();
  sendReceiveATCommand("AT+CREG?", "\r\nOK", "\r\nERROR", 3000);
  RegStatus1 = strtok(result_char, ":,");
  RegStatus2 = strtok(NULL, ",");
  RegStatus = strtok(NULL, ",");
  //RegStatus = atoi(RegStatus2);
  Serial.println(RegStatus1);
  Serial.println(RegStatus2);
  Serial.println(RegStatus3);
}


//Function to send most buoy sensor data with the cellular modem
//Modem specific data is also acquired here
void SendData() {
  //Turn on modem with pin D5
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  delay(1000);
  pinMode(5, INPUT);
  delay(100);
  modem.init();
  sendReceiveATCommand("AT+CFUN=1,1", "\r\n+KSUP: 0", "\r\nERROR", 150000); //Restart modem
  sendReceiveATCommand("AT+CSQ", "\r\nOK", "\r\nERROR", 3000); //Get signal strength for debugging
  String Signal = result_char;
  String Signal2 = Signal.substring(17, 25);
  GetCSQ();

  GetModemTemp();
  GetReg();
  SetTime();

  sprintf(url1, "AT+KHTTPSGET=1,\"lakestat/BuoyData/NCL/GetData.php?batv=%f&roll=%f&pitch=%f&heading=%f&lat=%f&lon=%f&course=%f&speed=%f&alt=%f&rssi=%02d&modemtemp=%02d&regstat=%02d&timestamper=20%d-%02d-%02d+%02d:%02d:%02d",
          Batv,

          //IMU data
          Roll,
          Pitch,
          Heading,

          //GPS data
          Lat,
          Lon,
          Course,
          Speed,
          Alt,

          //Modem data
          RSI,
          ModemTemp,
          RegStatus,
          Year,
          Month,
          Day,
          Hour,
          Minute,
          Second
         );

  sprintf(url2, "&temp0=%f&temp1=%f&temp2=%f&temp3=%f&temp4=%f&temp5=%f&temp6=%f&temp7=%f&temp8=%f&wd=%f&cd=%f&ws=%f&phyavg=%f&phystd=%f&chlavg=%f&chlstd=%f&paravg=%f&parstd=%f&domgl=%f&dotemp=%f&dosat=%f\"",
          Temp[0],
          Temp[1],
          Temp[2],
          Temp[3],
          Temp[4],
          Temp[5],
          Temp[6],
          Temp[7],
          Temp[8],

          //Wind data
          WindDir,
          WindDir,
          WindSpeed,

          //Fluorometer data
          PhycoAvg,
          PhycoStd,
          ChlAvg,
          ChlStd,

          //PAR data
          PARAvg,
          PARStd,

          //RDO data
          DO,
          DOS,
          RDOTemp
         );

  strcpy(url, url1);
  strcat(url, url2);
  Serial.println(url);

  if (doLCD > 0) {
    u8g2.clearBuffer();
    u8g2.drawStr(0, 12, "Cell Signal ="); u8g2.drawStr(80, 24, Signal2.c_str());
    u8g2.sendBuffer();
    u8g2.drawStr(0, 24, "Sending data...");
    u8g2.sendBuffer();
  }

  sendReceiveATCommand("AT&K3", "\r\nOK", "\r\nERROR", 10000); //set flow control
  //sendReceiveATCommand("AT+KCNXCFG=1,\"GPRS\",\"soracom.io\"", "\r\nOK", "\r\nERROR", 10000); //Setup GPRS communication
  //sendReceiveATCommand("AT+KCNXTIMER=1,10,3,30", "\r\nOK", "\r\nERROR", 10000); //Set a timer for retries
  //sendReceiveATCommand("AT+KHTTPSCFG=1,\"web.uwm.edu\",443,1", "\r\n+KHTTPS_IND: 1,1", "\r\nERROR", 30000);


  //Uncomment when ready to send data
  //sendReceiveATCommand(url, "\r\nOK", "\r\nERROR", 10000); //Send data using HTTP GET
  char* Result1;
  char* Result2;
  char* Result3;
  Result1 = strtok(result_char, " ");
  Result2 = strtok(NULL, " ");
  Result3 = strtok(NULL, " ");

  Serial.println(Result3);

  sendReceiveATCommand("AT", "\r\nOK", "\r\nERROR", 1000); //Seems to be needed to get modem's attention (??)
  sendReceiveATCommand("AT", "\r\nOK", "\r\nERROR", 1000);
  sendReceiveATCommand("AT", "\r\nOK", "\r\nERROR", 1000);
  sendReceiveATCommand("AT+KHTTPCLOSE=1,1", "OK", "\r\nERROR", 3000); //Close the http connection
  sendReceiveATCommand("AT+KHTTPDEL=1", "OK", "\r\nERROR", 3000); //delete the http connection
  sendReceiveATCommand("AT+CFUN=0", "\r\nOK", "\r\nERROR", 2000); //Power down modem
}

void ModemOff(){
  sendReceiveATCommand("AT", "\r\nOK", "\r\nERROR", 1000);
  sendReceiveATCommand("AT", "\r\nOK", "\r\nERROR", 1000);
  sendReceiveATCommand("AT+CFUN=0", "\r\nOK", "\r\nERROR", 2000); 
}

//Function to send AT commands to modem and listen for expected response, error code, or timeout after so many seconds
String sendReceiveATCommand2(const char *toSend, const char *toCheck1, const char *toCheck2, unsigned long milliseconds) {
  String result;
  Serial.print("Sending: ");
  Serial.println(toSend);
  SerialAT.println(toSend);
  unsigned long startTime = millis();
  Serial.print("Received: ");
  while (millis() - startTime < milliseconds) {
    SerialAT.flush();
    Serial.flush();
    if (SerialAT.available()) {
      char c = SerialAT.read();
      Serial.write(c);
      result += c;  // append to the result string
      result.toCharArray(result_char, 1000);
      if (strstr(result_char, toCheck1) || strstr(result_char, toCheck2))
      {
        milliseconds = 0;
      }
    }
  }
  Serial.println();
  return result;
  Serial.print(result);
}

//Function to send AT commands to modem and listen for expected response, error code, or timeout after so many seconds
String sendReceiveATCommand(const char *toSend, const char *toCheck1, const char *toCheck2, unsigned long milliseconds) {
  String result;
  //Serial.print("Sending: ");
  //Serial.println(toSend);
  SerialAT.println(toSend);
  unsigned long startTime = millis();
  //Serial.print("Received: ");
  while (millis() - startTime < milliseconds) {
    SerialAT.flush();
    Serial.flush();
    if (SerialAT.available()) {
      char c = SerialAT.read();
      //Serial.write(c);
      result += c;  // append to the result string
      result.toCharArray(result_char, 1000);
      if (strstr(result_char, toCheck1) || strstr(result_char, toCheck2))
      {
        milliseconds = 0;
      }
    }
  }
  //Serial.println();  // new line after timeout.
  return result;
  //Serial.print(result);
}
