//Function to turn on GPS
void GPSON() {
  tcaselect(7);
  //Set UART MUX for GPS
  mcp.pinMode(4, OUTPUT);
  mcp.digitalWrite(4, LOW);

  mcp.pinMode(14, OUTPUT);
  mcp.digitalWrite(14, LOW);

  mcp.pinMode(15, OUTPUT);
  mcp.digitalWrite(15, LOW);

  //Turn on MUX
  mcp.pinMode(8, OUTPUT);
  delay(100);
  mcp.digitalWrite(8, HIGH);
  delay(100);

  //Turn on GPS
  mcp.pinMode(13, OUTPUT);
  delay(100);
  mcp.digitalWrite(13, HIGH);
  delay(100);


  tcaselect(7);
  mcp.pinMode(12, OUTPUT);
  mcp.digitalWrite(12, LOW);
  delay(100);
  mcp.pinMode(12, INPUT);
}

//Function to turn off GPS
void GPSOFF() {
  tcaselect(7);
  //Turn off MUX
  mcp.pinMode(8, OUTPUT);
  delay(100);
  mcp.digitalWrite(8, LOW);

  //Turn off GPS
  delay(100);
  mcp.pinMode(13, OUTPUT);
  delay(100);
  mcp.digitalWrite(13, LOW);
  delay(100);
}

//Function to read GPS data
void readGPS() {
  GPSON(); 
  ss.end();
  ss.begin(9600);
  staticTime = millis();
  unsigned long GPSHoldTime = 10000;
  int SatSetup = -1;
    while (staticTime + GPSHoldTime > millis()){
      gps.encode(ss.read());
      SatSetup = gps.satellites.value();
      Serial.print("Waiting for GPS satellite fix. "); Serial.print("GPS Satellites = "); Serial.println(SatSetup);
      delay(500);
    }
    while (ss.available() > 0)
      gps.encode(ss.read());
      if (gps.location.isUpdated()){
        Lat = gps.location.lat();
        Lon = gps.location.lng();
        Alt = gps.altitude.meters();
        Course = gps.course.deg();
        Sat = gps.satellites.value();
        Speed = gps.speed.knots();
        Hop = gps.hdop.value();
        GPSYear = gps.date.year();
        GPSMonth = gps.date.month();
        GPSDay = gps.date.day();
        GPSHour = gps.time.hour();
        GPSMin = gps.time.minute();
        GPSSec = gps.time.second();
      }
    ss.end();
    GPSOFF();
}
