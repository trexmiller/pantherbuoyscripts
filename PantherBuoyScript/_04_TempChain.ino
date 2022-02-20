//Temp string one wire sensor addresses below need to be changed for each panther buoy tempchain
//Run the DS18B20_PantherBuoy script to find temp strng addresses.
//Then put addresses in order from top to bottom below
//In addition, change the number of temp chain nodes NSENSORS
byte Address[NSENSORS][8] = {
  {0x28, 0xF0, 0x92, 0xAC, 0x0D, 0x00, 0x00, 0xF3},
  {0x28, 0x58, 0xBB, 0xAC, 0x0D, 0x00, 0x00, 0x05},
  {0x28, 0x92, 0x9C, 0xAC, 0x0D, 0x00, 0x00, 0xFC},
  {0x28, 0xC1, 0x6F, 0xAC, 0x0D, 0x00, 0x00, 0x63},
  {0x28, 0x69, 0xB5, 0xAC, 0x0D, 0x00, 0x00, 0x7D},
  {0x28, 0x8D, 0xB5, 0xAC, 0x0D, 0x00, 0x00, 0x88}
};

 byte present = 0;

//Function to read temp chain, code adapted from OneWire DS18B20 library example
void readTemps() {

  unsigned int TempsNowTime = millis();
  unsigned int TempsInterval = 10000; //Give sensor 10 seconds to send good data
  Temp[0] = -995;
  
  tcaselect(7);
  delay(100);
  mcp.pinMode(11, OUTPUT);
  delay(100);
  mcp.digitalWrite(11, HIGH);
  delay(1000);
  mcp.pinMode(10, OUTPUT);
  delay(100);
  mcp.digitalWrite(10, HIGH);
  delay(1000);
  
  
  while(TempsNowTime + TempsInterval > millis()){
     if(Temp[0]==-995 || Temp[0] == -127){
      for (int i = 0; i < NSENSORS; i++) {
        byte type_s;
        byte rawtemp[12];
        oneWire.reset();
        oneWire.select(Address[i]);
        oneWire.write(0x44, 1);
        delay(1200);     // required delay
        present = oneWire.reset();
        oneWire.select(Address[i]);
        oneWire.write(0xBE);         // Read Scratchpad
        for (int j = 0; j < 9; j++) {           // we need 9 bytes
          rawtemp[j] = oneWire.read();
        }
        int16_t raw = (rawtemp[1] << 8) | rawtemp[0];
        if (type_s) {
          raw = raw << 3;
          if (rawtemp[7] == 0x10) {
            raw = (raw & 0xFFF0) + 12 - rawtemp[6];
          }
        } else {
          byte cfg = (rawtemp[4] & 0x60);
          if (cfg == 0x00) raw = raw & ~7;
          else if (cfg == 0x20) raw = raw & ~3;
          else if (cfg == 0x40) raw = raw & ~1;
        }
        Temp[i] = (float)raw / 16.0;
        TempF[i] = Temp[i] * 1.8 + 32.0;
      }
    }
  }
}
