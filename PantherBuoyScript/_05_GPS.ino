//Function to turn on GPS
void GPSON() {
  tcaselect(7);
  //Set UART MUX for GPS
  mcp.pinMode(4, OUTPUT);
  mcp.digitalWrite(4, LOW);
  delay(1000);

  mcp.pinMode(14, OUTPUT);
  mcp.digitalWrite(14, LOW);
  delay(1000);

  mcp.pinMode(15, OUTPUT);
  mcp.digitalWrite(15, LOW);
  delay(1000);

  //Turn on MUX, 5VS1 already on for wave/wind sensors
  mcp.pinMode(8, OUTPUT);
  delay(100);
  mcp.digitalWrite(8, HIGH);
  delay(1000);

  //Turn on GPS
  mcp.pinMode(13, OUTPUT);
  delay(100);
  mcp.digitalWrite(13, HIGH);
  delay(1000);
}

//Function to read GPS data
void readGPS() {
  GPSON();
  delay(1000);
  unsigned long GPSStartTime = millis();
  unsigned long GPSHoldTime = 10000;

  Serial.println("Starting GPS measurement");
  while (GPSStartTime + GPSHoldTime > millis()){
    while (ss.available() > 0)
      gps.encode(ss.read());
      if (gps.location.isUpdated()){ 
        Lat = gps.location.lat();
        Lon = gps.location.lng();
      }
      else if (gps.date.isUpdated()){
        GPSYear = gps.date.year();
        GPSMonth = gps.date.month();
        GPSDay = gps.date.day();
      }
      else if (gps.time.isUpdated()){
        GPSHour = gps.time.hour();
        GPSMin = gps.time.minute();
        GPSSec = gps.time.second();
      }
      else if (gps.speed.isUpdated()){ 
        Speed = gps.speed.mps();
      }
      else if (gps.course.isUpdated()){ 
        Course = gps.course.deg();
      }
      else if (gps.altitude.isUpdated()){ 
        Alt = gps.altitude.meters();
      }
      else if (gps.satellites.isUpdated()){ 
        Sat = gps.satellites.value();
      }
      else if (gps.hdop.isUpdated()){
        Hop = gps.hdop.value();
      }
  }
  Serial.println("Finished GPS measurement");
  mcp.pinMode(13,OUTPUT);
  delay(100);
  mcp.digitalWrite(13,LOW);
  delay(100);
}
