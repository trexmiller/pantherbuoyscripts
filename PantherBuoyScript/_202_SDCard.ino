
/*******************************SD card defines and functions**************************************/
//Name the data file on SD card. New data will be appended

// serial output steam
ArduinoOutStream cout(Serial);

// global for card size
uint32_t cardSize;

// global for card erase size
uint32_t eraseSize;

// Buffer for Serial input.
char cinBuf[40];

// Create a serial input stream.
ArduinoInStream cin(Serial, cinBuf, sizeof(cinBuf));

//------------------------------------------------------------------------------
// store error strings in flash
#define sdErrorMsg(msg) sd.errorPrint(F(msg));
#define error(msg) sd.errorHalt(F(msg))
//------------------------------------------------------------------------------
uint8_t cidDmp() {
  cid_t cid;
  if (!sd.card()->readCID(&cid)) {
    sdErrorMsg("readCID failed");
    return false;
  }
  cout << F("\nManufacturer ID: ");
  cout << hex << int(cid.mid) << dec << endl;
  cout << F("OEM ID: ") << cid.oid[0] << cid.oid[1] << endl;
  cout << F("Product: ");
  for (uint8_t i = 0; i < 5; i++) {
    cout << cid.pnm[i];
  }
  cout << F("\nVersion: ");
  cout << int(cid.prv_n) << '.' << int(cid.prv_m) << endl;
  cout << F("Serial number: ") << hex << cid.psn << dec << endl;
  cout << F("Manufacturing date: ");
  cout << int(cid.mdt_month) << '/';
  cout << (2000 + cid.mdt_year_low + 10 * cid.mdt_year_high) << endl;
  cout << endl;
  return true;
}

uint8_t csdDmp() {
  csd_t csd;
  uint8_t eraseSingleBlock;
  if (!sd.card()->readCSD(&csd)) {
    sdErrorMsg("readCSD failed");
    return false;
  }
  if (csd.v1.csd_ver == 0) {
    eraseSingleBlock = csd.v1.erase_blk_en;
    eraseSize = (csd.v1.sector_size_high << 1) | csd.v1.sector_size_low;
  } else if (csd.v2.csd_ver == 1) {
    eraseSingleBlock = csd.v2.erase_blk_en;
    eraseSize = (csd.v2.sector_size_high << 1) | csd.v2.sector_size_low;
  } else {
    cout << F("csd version error\n");
    return false;
  }
  eraseSize++;
  cout << F("cardSize: ") << 0.000512*cardSize;
  cout << F(" MB\n");

  cout << F("flashEraseSize: ") << int(eraseSize) << F(" blocks\n");
  cout << F("eraseSingleBlock: ");
  if (eraseSingleBlock) {
    cout << F("true\n");
  } else {
    cout << F("false\n");
  }
  return true;
}

uint8_t partDmp() {
  mbr_t mbr;
  if (!sd.card()->readBlock(0, (uint8_t*)&mbr)) {
    sdErrorMsg("read MBR failed");
    return false;
  }
  for (uint8_t ip = 1; ip < 5; ip++) {
    part_t *pt = &mbr.part[ip - 1];
    if ((pt->boot & 0X7F) != 0 || pt->firstSector > cardSize) {
      cout << F("\nNo MBR. Assuming Super Floppy format.\n");
      return true;
    }
  }
  cout << F("\nSD Partition Table\n");
  cout << F("part,boot,type,start,length\n");
  for (uint8_t ip = 1; ip < 5; ip++) {
    part_t *pt = &mbr.part[ip - 1];
    cout << int(ip) << ',' << hex << int(pt->boot) << ',' << int(pt->type);
    cout << dec << ',' << pt->firstSector <<',' << pt->totalSectors << endl;
  }
  return true;
}

void volDmp() {
  cout << F("\nVolume is FAT") << int(sd.vol()->fatType()) << endl;
  cout << F("blocksPerCluster: ") << int(sd.vol()->blocksPerCluster()) << endl;
  cout << F("clusterCount: ") << sd.vol()->clusterCount() << endl;
  cout << F("freeClusters(Please Wait....):");
  uint32_t volFree = sd.vol()->freeClusterCount();
  cout <<  volFree << endl;
  float fs = 0.000512*volFree*sd.vol()->blocksPerCluster();
  cout << F("freeSpace: ") << fs << endl;
  cout << F("fatStartBlock: ") << sd.vol()->fatStartBlock() << endl;
  cout << F("fatCount: ") << int(sd.vol()->fatCount()) << endl;
  cout << F("blocksPerFat: ") << sd.vol()->blocksPerFat() << endl;
  cout << F("rootDirStart: ") << sd.vol()->rootDirStart() << endl;
  cout << F("dataStartBlock: ") << sd.vol()->dataStartBlock() << endl;
  if (sd.vol()->dataStartBlock() % eraseSize) {
    cout << F("Data area is not aligned on flash erase boundaries!\n");
    cout << F("Download and use formatter from www.sdcard.org!\n");
  }
}

void SDInfo() {
  if (!sd.cardBegin(chipSelect, SD_SCK_MHZ(10))) {
    sdErrorMsg("cardBegin failed");
   }
 
  cardSize = sd.card()->cardSize();
  if (cardSize == 0) {
    sdErrorMsg("cardSize failed");
  }
  
  switch (sd.card()->type()) {
  case SD_CARD_TYPE_SD1:
    cout << F("SD1\n");
    break;

  case SD_CARD_TYPE_SD2:
    cout << F("SD2\n");
    break;

  case SD_CARD_TYPE_SDHC:
    if (cardSize < 70000000) {
      cout << F("SDHC\n");
    } else {
      cout << F("SDXC\n");
    }
    break;

  default:
    cout << F("Unknown\n");
  }
  if (!cidDmp()) {
    return;
  }
  if (!csdDmp()) {
    return;
  }
  uint32_t ocr;
  if (!sd.card()->readOCR(&ocr)) {
    sdErrorMsg("\nreadOCR failed");
    return;
  }
  cout << F("OCR: ") << hex << ocr << dec << endl;
  if (!partDmp()) {
    return;
  }
  if (!sd.fsBegin()) {
    sdErrorMsg("\nFile System initialization failed.\n");
    return;
  }
  volDmp();
}

void SDList() {
  if (!sd.begin(chipSelect, SD_SCK_MHZ(10))) {
    sd.initErrorHalt();
  }
  
  int rootFileCount = 0;
  root.open("/");
  
  while (root.openNext(&root, O_RDONLY)) {
    if (!root.isHidden()) {
      rootFileCount++;
    }
    root.close();
  }
  cout << F("\nList of files on the SD Card:\n");
  sd.ls("/", LS_R);
}

void sdCreateDir(){
  cin.clear();
  cout << F("Type the name of the new directory to create\n");
  cin.readline();
  cout << endl;
  Serial.println("Got it!");
  if (!sd.begin(chipSelect, SD_SCK_MHZ(10))) {
    sd.initErrorHalt();
  }

  int rootFileCount = 0;
  if (!root.open("/")) {
    error("open root failed");
  }
  while (root.openNext(&root, O_RDONLY)) {
    if (!root.isHidden()) {
      rootFileCount++;
    }
    root.close();
  }
  if (!sd.mkdir(cinBuf)) {
    error("Create directory failed");
  }
  cout << F("Created directory\n");

  cout << F("\nList of files on the SD.\n");
  sd.ls("/", LS_R);
}

void readSD(){
  SDList();
  // Wait for USB Serial 
  while (!Serial) {
    SysCall::yield();
  }
  cout << F("============================================================================. \n");
  cout << F("Type the name of the file to read from. \n");
  cout << F("Files on the SD card have been listed above. \n");
  cout << F("The file chosen will be read completely or timeout after 10 seconds. \n");
  cout << F("============================================================================. \n");
  
  cin.readline();
  cout << endl;
  Serial.flush();
  Serial.println("Got it!");
  char ReadFile[40];
  strcpy(ReadFile,cinBuf);

 sd.begin(chipSelect, SD_SCK_MHZ(10)); 
 
 myFile = sd.open(ReadFile);
  if (myFile) {
    unsigned long sdNowTime = millis();
    while (sdNowTime+10000>millis()){
        Serial.write(myFile.read());
      }

    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  cout << F("\n");
  cout << F("\n");
}

//Function to check if the SD card is present. Not currently used.
void checkSD() {
  sd.begin();
  if (!sd.begin(chipSelect)) {
    delay(1);
    if (Serial) {
      Serial.println("Card init. failed!");
    }
    else {
      Serial.println("  SD... OK");
    }
  }
}

//Function to create the file on the SD card where data strings will be saved
void createSDFile() {
  File logfile = sd.open("STRG2022.csv", FILE_WRITE);
  //Timedat
  logfile.print("YY"); logfile.print(",");
  logfile.print("MM"); logfile.print(",");
  logfile.print("DD"); logfile.print(",");
  logfile.print("HH"); logfile.print(",");
  logfile.print("MM"); logfile.print(",");
  logfile.print("SS"); logfile.print(",");
  
  //Modem data
  logfile.print("rsi"); logfile.print(",");
  logfile.print("ModemTemp"); logfile.print(",");
  logfile.print("RegStatus"); logfile.print(",");

  //GPS data
  logfile.print("gpsyear"); logfile.print(",");
  logfile.print("gpsmonth"); logfile.print(",");
  logfile.print("gpsday"); logfile.print(",");
  logfile.print("gpshour"); logfile.print(",");
  logfile.print("gpsmin"); logfile.print(",");
  logfile.print("gpssec"); logfile.print(",");
  logfile.print("latitude"); logfile.print(",");
  logfile.print("longitude"); logfile.print(",");
  logfile.print("course"); logfile.print(",");
  logfile.print("speed"); logfile.print(",");
  logfile.print("alt"); logfile.print(",");
  logfile.print("sat"); logfile.print(",");

  //Battery data
  logfile.print("Batv"); logfile.print(",");

  //Fluorometer data
  logfile.print("ChlAvg"); logfile.print(",");
  logfile.print("ChlStd"); logfile.print(",");
  logfile.print("PhyAvg"); logfile.print(",");
  logfile.print("PhyStd"); logfile.print(",");

  //PAR data
  logfile.print("PARAvg"); logfile.print(",");
  logfile.print("PARStd"); logfile.print(",");

  //temp data
  logfile.print("Temp0"); logfile.print(",");
  logfile.print("Temp1"); logfile.print(",");
  logfile.print("Temp2"); logfile.print(",");
  logfile.print("Temp3"); logfile.print(",");
  logfile.print("Temp4"); logfile.print(",");

  //RDO Data
  logfile.print("DOmgL"); logfile.print(",");
  logfile.print("DOTemp"); logfile.print(",");
  logfile.print("DOSat"); logfile.print(",");

  //BME Data
  logfile.print("ATemp"); logfile.print(",");
  logfile.print("Pressure"); logfile.print(",");
  logfile.print("Humidity"); logfile.print(",");
  logfile.print("Gas"); logfile.print(",");
  logfile.print("BMEAlt"); logfile.print(",");

  //Wind Data
  logfile.print("WindSpeed"); logfile.print(",");
  logfile.print("WindDir"); logfile.print(",");
  logfile.print("CalDirection"); logfile.print(",");
  logfile.print("Gust"); logfile.print(",");
  logfile.print("ATMOAirT"); logfile.print(",");
  logfile.print("ATMOX"); logfile.print(",");
  logfile.print("ATMOY"); logfile.print(",");

  //Wave Data
  logfile.print("WVHeading"); logfile.print(",");
  logfile.print("WVDir"); logfile.print(",");
  logfile.print("WVPeriod"); logfile.print(",");
  logfile.print("WVHT"); logfile.print(",");
  logfile.print("MeanWVDir"); logfile.print(",");
  logfile.print("GN"); logfile.print(",");
  logfile.print("DP1"); logfile.print(",");
  logfile.print("DP2");

  //logfile.sync();
  logfile.close();
}

//Function to save data to SD card
void saveSD() {
  GetCSQ();
  GetModemTemp();
  GetReg();
  File logfile = sd.open("STRG2022.csv", FILE_WRITE);
  sprintf(TimeDat, "%d,%d,%d,%d,%d,%d,",
          rtc.getYear(),
          rtc.getMonth(),
          rtc.getDay(),
          rtc.getHours(),
          rtc.getMinutes(),
          rtc.getSeconds()
         );
  //Serial.println("TimeDat:");
  //Serial.println(TimeDat);

  sprintf(ModemData, "%d,%d,%d,",
          RSI,
          ModemTemp,
          RegStatus
         );
         
  //Serial.println("ModemData:");
  //Serial.println(ModemData);

  sprintf(GPSData, "%d,%d,%d,%d,%d,%d,%.5f,%.5f,%.2f,%.2f,%.2f,%f,",
          GPSYear,
          GPSMonth,
          GPSDay,
          GPSHour,
          GPSMin,
          GPSSec,
          Lat,
          Lon,
          Course,
          Speed,
          Alt,
          Sat
         );

  //Serial.println("GPSData:");
  //Serial.println(GPSData);

  sprintf(BattData, "%.2f,",
          Batv
         );
  //Serial.println("BattData:");
  //Serial.println(BattData);

  sprintf(FluorData, "%.2f,%.2f,%.2f,%.2f,",
          ChlAvg,
          ChlStd,
          PhycoAvg,
          PhycoStd
         );
  //Serial.println("FluorData:");
  //Serial.println(FluorData);
  
  sprintf(PARData, "%.2f,%.2f,,%.2f,,%.2f,,%.2f,,%.2f,",
          paraAvg,
          paraStd,
          IRAvg,
          IRStd,
          UVAvg,
          UVStd
         );
  //Serial.println("PARData:");
  //Serial.println(PARData);
  
  sprintf(TempData, "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,",
          Temp[0],
          Temp[1],
          Temp[2],
          Temp[3],
          Temp[4],
          DO,
          DOS,
          RDOTemp,
          ATemp,
          Humidity,
          Pressure,
          Gas,
          BMEAlt
         );

  Serial.println("TempData:");
  Serial.println(TempData);
  
  sprintf(WindData, "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,",
          WindSpeed,
          WindDir,
          TrueWindDir,
          Gust,
          ATMOAirT,
          ATMOX,
          ATMOY
         );
  
  Serial.println("WindData:");
  Serial.println(WindData);
  

  sprintf(WaveData, "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f", //Last set of data so no ending comma
          WVHeading,
          WVDir,
          WVPeriod,
          WVHT,
          MeanWVDir,
          GN,
          DP1,
          DP2
        );
        
  Serial.println("WaveData:");
  Serial.println(WaveData);

  strcpy(SDData, TimeDat);
  strcat(SDData, ModemData);
  strcat(SDData, GPSData);
  strcat(SDData, BattData );
  strcat(SDData, FluorData );
  strcat(SDData, PARData );
  strcat(SDData, TempData );
  strcat(SDData, WindData );
  strcat(SDData, WaveData );

  strcat(SDData, "\r\n");
  Serial.println("The data string below was saved to SD card...");
  Serial.print("YY"); Serial.print(",");
  Serial.print("MM"); Serial.print(",");
  Serial.print("DD"); Serial.print(",");
  Serial.print("HH"); Serial.print(",");
  Serial.print("MM"); Serial.print(",");
  Serial.print("SS"); Serial.print(",");
  Serial.print("rssi"); Serial.print(",");
  Serial.print("modemtemp"); Serial.print(",");
  Serial.print("RegStatus"); Serial.print(",");
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
  Serial.print("Batv"); Serial.print(",");
  Serial.print("ChlAvg"); Serial.print(",");
  Serial.print("ChlStd"); Serial.print(",");
  Serial.print("PhyAvg"); Serial.print(",");
  Serial.print("PhyStd"); Serial.print(",");
  Serial.print("PARAvg"); Serial.print(",");
  Serial.print("PARStd"); Serial.print(",");
  Serial.print("Temp0"); Serial.print(",");
  Serial.print("Temp1"); Serial.print(",");
  Serial.print("Temp2"); Serial.print(",");
  Serial.print("Temp3"); Serial.print(",");
  Serial.print("Temp4"); Serial.print(",");
  Serial.print("DOmgL"); Serial.print(",");
  Serial.print("DOSat"); Serial.print(",");
  Serial.print("DOTemp"); Serial.print(",");
  Serial.print("ATemp"); Serial.print(",");
  Serial.print("Humidity"); Serial.print(",");
  Serial.print("Pressure"); Serial.print(",");
  Serial.print("Gas"); Serial.print(",");
  Serial.print("BMEAlt"); Serial.print(",");
  Serial.print("WindSpeed"); Serial.print(",");
  Serial.print("WindDir"); Serial.print(",");
  Serial.print("TrueWindDir"); Serial.print(",");
  Serial.print("Gust"); Serial.print(",");
  Serial.print("ATMOAirT"); Serial.print(",");
  Serial.print("ATMOX"); Serial.print(",");
  Serial.print("ATMOY"); Serial.print(",");
  Serial.print("WVHeading"); Serial.print(",");
  Serial.print("WVDir"); Serial.print(",");
  Serial.print("WVPeriod"); Serial.print(",");
  Serial.print("WVHT"); Serial.print(",");
  Serial.print("MeanWVDir"); Serial.print(",");
  Serial.print("GN"); Serial.print(",");
  Serial.print("DP1"); Serial.print(",");
  Serial.println("DP2");
  Serial.println(SDData);
  logfile.print(SDData);
  logfile.sync();
  logfile.close();
}
