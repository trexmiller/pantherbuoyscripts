

const int line_buffer_size = 50;
char Offsets[line_buffer_size];
//------------------------------------------------------------------------------
char* GetAccelOffsetline() {
  ifstream sdin("AccelOffsets.csv");
  int line_number = 0;

  while (sdin.getline(Offsets, line_buffer_size, '\n') || sdin.gcount()) {
    int count = sdin.gcount();
    if (sdin.fail()) {
      sdin.clear(sdin.rdstate() & ~ios_base::failbit);
    } else if (sdin.eof()) {
    } else {
      count--;  // Don’t include newline in count
    }
    return Offsets;
  }
}

void readAccelOffset(){
  char AccelOffsetFile[40] = "AccelOffsets.csv";
  sd.begin(chipSelect, SD_SCK_MHZ(10)); 
  myFile = sd.open(AccelOffsetFile);
  
  GetAccelOffsetline();
  int i = 0;
  char *p = strtok (Offsets, ",");
  char *AccelOffsets[3];

  while (p != NULL)
    {
        AccelOffsets[i++] = p;
        p = strtok (NULL, ",");
    }

  Serial.print("AccelOffset X = "); Serial.println(AccelOffsets[0]);
  Serial.print("AccelOffset Y = "); Serial.println(AccelOffsets[1]);
  Serial.print("AccelOffset Z = "); Serial.println(AccelOffsets[2]);

  myFile.close();
}

/*********************************************************Accelerometer Calibration Functions****************************************************/
void printParam(char txt[], float param[3])
{   for (int i= 0; i<=2 ; i++) 
    {  Serial.print(txt);Serial.print("[");
       Serial.print(i);Serial.print("] = "); 
       Serial.print(param[i],6);Serial.print(";");
    }
}
void printSetParam(char txt[], float param[3])
{   Serial.print(txt);Serial.print("(");
    Serial.print(param[0],6);Serial.print(", ");
    Serial.print(param[1],6);Serial.print(", ");
    Serial.print(param[2],6);Serial.print(");");
}

void saveAccelOffsets() {
  char AccelOffsetFile[40] = "AccelOffsets.csv";
  sd.begin(chipSelect, SD_SCK_MHZ(10)); 
  myFile = sd.open(AccelOffsetFile,FILE_WRITE);
  
  char AccelOffsetsSD[40];
  Serial.println(IMU.accelOffset[1]);
  
  sprintf(AccelOffsetsSD, "%f,%f,%f",
    IMU.accelOffset[0],
    IMU.accelOffset[1],
    IMU.accelOffset[2]
    );
    
  strcat(AccelOffsetsSD, "\r\n");
  Serial.println(AccelOffsetsSD);
  myFile.print(AccelOffsetsSD);
  myFile.close();
}

void saveAccelSlopes() {
  if (!sd.begin(chipSelect, SD_SCK_MHZ(10))) {
    sd.initErrorHalt();
  }
  File logfile = sd.open("AccelSlopes.csv", FILE_WRITE);
  char AccelSlopeSD[40];
  
  sprintf(AccelSlopeSD, "%f,%f,%f",
    IMU.accelSlope[1],
    IMU.accelSlope[2],
    IMU.accelSlope[3]
    );
  logfile.print(AccelSlopeSD);
}

const float accelCriterion = 0.1;
char xyz[3]= {'X','Y','Z'};
float maxAX = 1, maxAY=1, maxAZ=1, minAX=-1, minAY=-1, minAZ=-1; // Accel Slope
float zeroAX1 =0,zeroAX2 =0,zeroAY1 =0,zeroAY2 =0,zeroAZ1 =0,zeroAZ2 =0;  //Accel Offset
boolean accelOK=false;
uint8_t acceMMlOK=0; // bit 0..2 maxXYZ bit 3..5 minXYZ
uint8_t accelODRindex=5; // Sample Rate 0:off, 1:10Hz, 2:50Hz, 3:119Hz, 4:238Hz, 5:476Hz, (6:952Hz=na) 
uint8_t accelFSindex=3;   // Full Scale// 0: ±2g ; 1: ±24g ; 2: ±4g ; 3: ±8g
uint16_t NofCalibrationSamples = 1000;

void CalAccelPanther(){
  pinMode(LED_BUILTIN,OUTPUT); 
  delay(10);
  calibrateAccelMenu(); 
}

void calibrateAccelMenu(){
 char incomingByte = 0;
 byte b;
 uint16_t NofCalibrationSamples = 1000;
 //while (1)  //(incomingByte!='X') 
    Serial.println(F("\n\n")); 
    Serial.println(F(" Calibrate Accelerometer Offset and Slope"));
    Serial.println(F(" You will be asked to enter the Full Scale (FS) setting and an output data rate (ODR) before a calibration measurement is made"));
    Serial.println(F(" During each calibration measurement the Panther Buoy vetical electronics shelf will need to be held on a flat surface with one of its axes pointing up."));
    Serial.println(F(" Each of the axes must be measured pointing up and pointing down (6 measurements). The FS and ODR will be asked for each time and should be entered the same each time"));
    Serial.println(F(" The program recognises which axis is vertical and shows which were measured successfully. If the angle is too far oblique the measurement is not valid."));
    Serial.println(F(" Calibration data will be automatically saved to the SD card and initialized after the system is restarted from the main menu."));
    Serial.println(F(" Enter the FS, then the ODR at the prompts and a measurement will be made. Repeat for each axis."));
    
    Serial.print(F(" Measured status of axis \n "));
    for (int i=0;i<=2;i++){  Serial.print(xyz[i]); if (bitRead(acceMMlOK,i)==1)Serial.print("+ = ( -OK- ) "); else Serial.print("+ = not done "); }
    Serial.print("\n ");
    for (int i=0;i<=2;i++){  Serial.print(xyz[i]); if (bitRead(acceMMlOK,i+3)==1)Serial.print("- = ( -OK- ) "); else Serial.print("- = not done "); }
 
    Serial.println(F("\n\n   // Accelerometer code"));
    Serial.print(F("   IMU.setAccelFS(")); Serial.print(accelFSindex);
    Serial.print(F(");\n   IMU.setAccelODR("));Serial.print(accelODRindex);Serial.println(");");

    printSetParam("   IMU.setAccelOffset",IMU.accelOffset);
    Serial.println();
    printSetParam ("   IMU.setAccelSlope ",IMU.accelSlope); 
    Serial.println("\n\n");

    saveAccelOffsets();
    saveAccelSlopes();
    
    FSMenu();
    IMU.setAccelFS(accelFSindex); 
    ODRMenu();
}

int FSChoice;
void FSMenu(){
  Serial.println(F("Choose the Full Scale (FS) setting")); 
  Serial.println(F("(0) ±2g"));
  Serial.println(F("(1) ±24g"));
  Serial.println(F("(2) ±4g"));
  Serial.println(F("(3) ±8g"));

  FShoiceMenu();
  switch (accelFSindex){
    case '0': 
     Serial.print("±2g");
    break; 
    case '1': 
      Serial.print("±24g");
    break;
    case '2': 
      Serial.print("±4g");
    break;
    case '3': 
      Serial.print("±8g");
    break;
  }
}
     
int FShoiceMenu(){
   if(Serial.available() > 0);            
   Serial.read();
   
   // before waiting for input  
   while(Serial.available() == 0);
    accelFSindex = Serial.read();
    return(accelFSindex);
}

int ODRChoice;
void ODRMenu(){
  Serial.println("Select an output data rate(ODR). After hitting enter a calibration measurement will begin.");
  Serial.println(F("(0) 10 Hz"));
  Serial.println(F("(1) 50 Hz"));
  Serial.println(F("(2) 119 Hz"));
  Serial.println(F("(3) 238 Hz"));
  Serial.println(F("(4) 476 Hz"));

  ODRhoiceMenu();
  switch (accelODRindex){
    case '0': 
     Serial.print("10 Hz");
    return calibrateAccel(NofCalibrationSamples);
    break; 
    case '1': 
      Serial.print("50 Hz");
    return calibrateAccel(NofCalibrationSamples);
    break;
    case '2': 
      Serial.print("119 Hz");
    return calibrateAccel(NofCalibrationSamples);
    break;
    case '3': 
      Serial.print("238 Hz");
    return calibrateAccel(NofCalibrationSamples);
    break;
    case '4': 
      Serial.print("476 Hz");
    return calibrateAccel(NofCalibrationSamples);
    break;
  }
  return ODRMenu();
}
        
int ODRhoiceMenu(){
   if(Serial.available() > 0);            
   Serial.read();
   
   // before waiting for input  
   while(Serial.available() == 0);
    accelODRindex = Serial.read();
    return(accelODRindex);
} 

void calibrateAccel(uint16_t NofSamples){
  IMU.setAccelODR(accelODRindex);
  tcaselect(7);
  mcp.pinMode(11, OUTPUT); //Controls relay to turn on chl fluorometer
  mcp.digitalWrite(11,HIGH); 
  delay(1000);
  tcaselect(2);
  delay(100);  
  IMU.begin();
  boolean validMmt=false;
  float x,y,z;
  Serial.println(F("\n\n\n\n\n\n\n\n\n\n\n")); 
  Serial.println(F("measuring \n")); 
//   IMU.setAccelODR(5);  //476 Hz
   raw_N_Accel(NofSamples,x,y,z);
   if (abs(x)>max(abs(y),abs(z)))
   {    Serial.println(F("X detected"));  
       if (sqrt(y*y+z*z)/x<accelCriterion)
       {  validMmt= true;
          if (x>0) {maxAX=x; 
                    acceMMlOK=acceMMlOK | 0b00000001 ;}
          else     {minAX=x; 
                    acceMMlOK=acceMMlOK | 0b00001000 ; }
       }
   }
   if (abs(y)>max(abs(x),abs(z)))
   {  Serial.println(F("Y detected"));  
      if (sqrt(x*x+z*z)/y<accelCriterion)
       {  validMmt= true;
          if (y>0) {maxAY=y; 
                    acceMMlOK=acceMMlOK | 0b00000010 ; }
          else     {minAY=y; 
                    acceMMlOK=acceMMlOK | 0b00010000 ; }
       }  
   }
   if (abs(z)>max(abs(x),abs(y)))
   {  Serial.println(F("Z detected"));  
      if ( sqrt(x*x+y*y)/z<accelCriterion)
       {  validMmt= true;
          if (z>0) {maxAZ=z; 
                    acceMMlOK=acceMMlOK | 0b00000100 ; }
          else     {minAZ=z;
                    acceMMlOK=acceMMlOK | 0b00100000 ; }
       }  
   }
   IMU.setAccelOffset((maxAX+minAX)/2,(maxAY+minAY)/2,(maxAZ+minAZ)/2);
   IMU.setAccelSlope ((maxAX-minAX)/2,(maxAY-minAY)/2,(maxAZ-minAZ)/2);
   if (acceMMlOK==0b00111111) accelOK = true;
   
   if ( !validMmt ) 
    { Serial.println(F("\n\n\nNot a valid measurement!  "));
      Serial.print("x= ");Serial.println(x);
      Serial.print("y= ");Serial.println(y);
      Serial.print("z= ");Serial.println(z);
    }
  Serial.println("");
  Serial.println("");   
  calibrateAccelMenu();
}


char readChar()
{  char ch;
   while (!Serial.available()) ;             // wait for character to be entered
   ch= toupper(Serial.read());   
   delay(10);
   while (Serial.available()){Serial.read();delay(1);} // empty readbuffer 
   return ch;
}


void readAnswer(char msg[], uint16_t& param)
{ char ch=0;
  byte count=0;
  const byte NofChars = 8;
  char ans[NofChars];
  while (Serial.available()){Serial.read();}  //empty read buffer
  Serial.print(msg); 
  Serial.print(param); 
  Serial.print(F(" Enter new value ")); 
  while (byte(ch)!=10 && byte(ch)!=13 && count<(NofChars-1)   )
  {  if (Serial.available())
     {  ch= Serial.read();
        ans[count]=ch;
        count++;
     }
  }      
  ans[count]=0;
  Serial.println(ans);
  if (count>1) param= atoi(ans);
  while (Serial.available()){Serial.read();}
     Serial.println(F("\n\n\n\n\n\n\n")); 
}

void raw_N_Accel(uint16_t N, float& averX, float& averY, float& averZ) 
{    float x, y, z;
     averX=0; averY =0;averZ =0;
     for (int i=1;i<=N;i++)
     {  while (!IMU.accelAvailable());
        IMU.readRawAccel(x, y, z);
        averX += x;    averY += y;     averZ += z;
        digitalWrite(LED_BUILTIN, (millis()/125)%2);       // blink onboard led every 250ms
        if ((i%30)==0)Serial.print('.'); 
     } 
     averX /= N;    averY /= N;     averZ /= N;
     digitalWrite(LED_BUILTIN,0);                          // led off
}


uint8_t findDevices(int pin)
{
  OneWire ow(pin);

  uint8_t address[8];
  uint8_t count = 0;


  if (ow.search(address))
  {
    Serial.print("\nuint8_t pin");
    Serial.print(pin, DEC);
    Serial.println("[][8] = {");
    do {
      count++;
      Serial.println("  {");
      for (uint8_t i = 0; i < 8; i++)
      {
        Serial.print("0x");
        if (address[i] < 0x10) Serial.print("0");
        Serial.print(address[i], HEX);
        if (i < 7) Serial.print(", ");
      }
      Serial.println("  },");
    } while (ow.search(address));

    Serial.println("};");
    Serial.print("// nr devices found: ");
    Serial.println(count);
  }

  return count;
}


void ReadTempNodeInfo(){
  tcaselect(7);
  mcp.pinMode(11, OUTPUT);
  mcp.digitalWrite(11, HIGH);
  delay(1000);
  mcp.pinMode(10, OUTPUT);
  mcp.digitalWrite(10, HIGH);
  delay(1000);
  
  Serial.println("Temp chain nodes will be read for 30 seconds");
  unsigned int TempInfoNowTime = millis();
  unsigned int TempInfoInterval = 30000; //Read sensors info for 30 seconds
  while (TempInfoNowTime + TempInfoInterval > millis()){
    byte i;
    byte present = 0;
    byte type_s;
    byte data[12];
    byte addr[8];
    float celsius, fahrenheit;
  
    if ( !oneWire.search(addr)) {
        Serial.println("No more addresses.");
        Serial.println();
        oneWire.reset_search();
        delay(250);
      }
  
      Serial.print("ROM =");
      for( i = 0; i < 8; i++) {
        Serial.write(' ');
        Serial.print(addr[i], HEX);
      }

    type_s = 0;

    oneWire.reset();
    oneWire.select(addr);
    oneWire.write(0x44, 1);        // start conversion, with parasite power on at the end
  
    delay(1000);              
    present = oneWire.reset();
    oneWire.select(addr);    
    oneWire.write(0xBE);         // Read Scratchpad

    Serial.print("  Data = ");
    Serial.print(present, HEX);
    Serial.print(" ");
    for ( i = 0; i < 9; i++) {           // we need 9 bytes
      data[i] = oneWire.read();
      Serial.print(data[i], HEX);
      Serial.print(" ");
    }
    Serial.print(" CRC=");
    Serial.print(OneWire::crc8(data, 8), HEX);
    Serial.println();

  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  Serial.print("  Temperature = ");
  Serial.print(celsius);
  Serial.print(" Celsius, ");
  Serial.print(fahrenheit);
  Serial.println(" Fahrenheit");
  } 
}

char ChlCalArray[4];
int ChlCalPointsChoice;
int TotalCalPoints;
void CalChl(){  
  Serial.println(F("Enter the total number of calibration points that will be measured not including the blank"));
  
  readChlCalInput();
  TotalCalPoints = atoi(ChlCalArray);
  Serial.print(F("Total calibration points = ")); Serial.println(TotalCalPoints);
  
  float CalChlConc[TotalCalPoints];
  float CalChlResult[TotalCalPoints];

  
  Serial.println(F(""));
  Serial.println(F("Place sensor in the blank solution lacking chlorophyll"));
  Serial.println(F("Hit any key, enter and background fluorescence will be measured"));
  readChlCalInput();
  CalChlConc[0] = 0.00;
  Serial.println(F("Got it. Measuring fluorescence in the blank (please wait)"));
  Serial.println(F(""));
  readChl();
  CalChlResult[0] = ChlAvg;

  /*
  Serial.println(F(""));
  Serial.println(F("Place sensor in the first standard solution containing chlorophyll"));
  Serial.println(F("Enter the concentration of chlorophyll in ug/L, hit enter and fluorescence will be measured"));
  readChlCalInput();
  CalChlConc[1] = atof(ChlCalArray);;
  Serial.println(F("Got it. Measuring fluorescence (please wait)"));
  Serial.println(F(""));
  readChl();
  CalChlResult[1] = ChlAvg;
  */
  
  for (int i = 1; i<TotalCalPoints; i++){
    Serial.println(F(""));
    Serial.print(F("Place sensor in standard solution "));Serial.println(i);
    Serial.println(F("Enter the concentration of the standard solution, hit enter and fluorescence will be measured"));
    readChlCalInput();
    CalChlConc[i] = atof(ChlCalArray);
    Serial.println(F("Got it. Measuring fluorescence (please wait)"));
    Serial.println(F(""));
    readChl();
    CalChlResult[i] = ChlAvg;
    }

  /*
  Serial.println(F(""));
  Serial.println(F("Place sensor in the **last** standard solution"));
  Serial.println(F("Enter the concentration of the last standard, hit enter and fluorescence will be measured"));
  readChlCalInput();
  CalChlConc[TotalCalPoints] = atof(ChlCalArray);
  Serial.println(F("Got it. Measuring fluorescence (please wait)"));
  Serial.println(F(""));
  readChl();
  CalChlResult[TotalCalPoints] = ChlAvg;
  */
  
  Serial.println(F("Here are the results:")); 
    Serial.print(F("Blank = ")); 
    Serial.print(CalChlResult[0]); Serial.print(F(" mV")); 
    Serial.print(F(" | ")); 
    Serial.print(CalChlConc[0]); Serial.println(F(" ug/L")); 
  for (int i = 1; i<TotalCalPoints; i++){
    Serial.print(F("Standard ")); Serial.print(i); Serial.print(F(" = "));
    Serial.print(CalChlResult[i]); Serial.print(F(" mV")); 
    Serial.print(F(" | ")); 
    Serial.print(CalChlConc[i]); Serial.println(F(" ug/L")); 
   }
  Serial.println(F(""));


 float Ex;
  float Ey;
  float Exy;
  float Ex2;
  float Ey2;

  for(int i = 0; i<TotalCalPoints; i++){
    Ey += CalChlResult[i];
  }

  for(int i = 0; i<TotalCalPoints; i++){
    Ex += CalChlConc[i];
  }

  for(int i = 0; i<TotalCalPoints; i++){
    Exy += CalChlConc[i] * CalChlResult[i];
  }

  for(int i = 0; i<TotalCalPoints; i++){
    Exy += CalChlConc[i] * CalChlResult[i];
  }

  for(int i = 0; i<TotalCalPoints; i++){
    Ex2 += CalChlConc[i] * CalChlConc[i];
  }

  for(int i = 0; i<TotalCalPoints; i++){
    Ey2 += CalChlResult[i] * CalChlResult[i];
  }

  float ChlSlope = ((TotalCalPoints*Exy) - (Ex * Ey))/((TotalCalPoints*Ex2)-(Ex * Ex));
  float ChlOffset = (Ey - (ChlSlope * Ex))/TotalCalPoints;
  float ChlR = ((TotalCalPoints * Exy) - Ex * Ey) / (sqrt((TotalCalPoints * Ex2) - (Ex * Ex)) * sqrt((TotalCalPoints * Ey2) - (Ey * Ey)));

  Serial.print ("Chlorophyll calibration Slope = "); Serial.println(ChlSlope);
  Serial.print ("Chlorophyll calibration Offset = "); Serial.println(ChlOffset);
  Serial.print ("Chlorophyll calibration Regression R = "); Serial.println(ChlR);
  Serial.println("");
  Serial.println("");
}






  




void readChlCalInput() {
  while (Serial.available() == 0);       // wait for user input
  for (int i = 0; i <= 4; i++) {
    ChlCalArray[i] = Serial.read();       // read incoming data
    delay(10);
  }
}
