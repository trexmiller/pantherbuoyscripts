/*****************************************BME680 Functions**************************************
The functions below setup and read the BME680 sensor for measuring air temperature, humidity, and 
atmospheric pressure. 
*************************************************************************************************/
//Function to setup BME680
void setupBME(){
   tcaselect(7);
   mcp.pinMode(11,OUTPUT);
   mcp.digitalWrite(11,HIGH);
   delay(100);
   bme.begin();
   bme.setTemperatureOversampling(BME680_OS_8X);
   bme.setHumidityOversampling(BME680_OS_2X);
   bme.setPressureOversampling(BME680_OS_4X);
   bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
   bme.setGasHeater(320, 150); // 320*C for 150 ms
   mcp.digitalWrite(11,LOW);
}
  
//Function to read BME680
void readBME(){
  tcaselect(7);
  mcp.pinMode(11,OUTPUT);
  mcp.digitalWrite(11,HIGH);
  delay(1000);
  tcaselect(4);
  delay(1000);
  bme.begin();
  unsigned int BMENowTime = millis();
  unsigned int BMEInterval = 10000; //Give sensor 10 seconds to send good data
  Gas = 0;
  while (BMENowTime + BMEInterval > millis()){
    if (Gas==0){
      Serial.print(".");
      bme.performReading();
      ATemp = bme.temperature;
      Pressure = bme.pressure / 100.0;
      Humidity = bme.humidity;
      Gas = bme.gas_resistance / 1000.0;
      BMEAlt = bme.readAltitude(SEALEVELPRESSURE_HPA);
    }
  }
  Serial.println("");
  tcaselect(7);
  mcp.digitalWrite(11,LOW);
}
