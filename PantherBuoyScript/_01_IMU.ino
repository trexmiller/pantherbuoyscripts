

void SetupLIS(){
  mcp.pinMode(10,OUTPUT);
  delay(1000);
  mcp.digitalWrite(10,HIGH);
  delay(1000);
    
  tcaselect(1);
  delay(1000);
  
  lis3mdl.begin_I2C();
  lis3mdl.setOperationMode(LIS3MDL_CONTINUOUSMODE);
  lis3mdl.setDataRate(LIS3MDL_DATARATE_155_HZ);
  lis3mdl.setRange(LIS3MDL_RANGE_4_GAUSS);

  lis3mdl.setIntThreshold(500);
  lis3mdl.configInterrupt(false, false, true, // enable z axis
                          true, // polarity
                          false, // don't latch
                          true); // enabled!
}

void readLIS(){
  tcaselect(7);
  mcp.pinMode(10,OUTPUT);
  delay(1000);
  mcp.digitalWrite(10,HIGH);
  delay(1000);
    
  tcaselect(1);
  delay(1000);
  
  lis3mdl.read();
  MagX = lis3mdl.x; 
  MagY = lis3mdl.y; 
  MagZ = lis3mdl.z; 

  Heading = atan2(MagX, MagY) * 180 / M_PI;
}

void SetupMPU(){
  tcaselect(7);
  mcp.pinMode(10,OUTPUT);
  delay(1000);
  mcp.digitalWrite(10,HIGH);
  delay(1000);
    
  tcaselect(1);
  delay(1000);
  mpu.begin();

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void readMPU(){
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  //Board is mounted on its side so X becomes Z and Z becomes X
  AccelZ = a.acceleration.x;
  AccelY = a.acceleration.y;
  AccelX = a.acceleration.z;

  //Board is mounted on its side so X becomes Z and Z becomes X
  GyroZ = g.gyro.x; 
  GyroY = g.gyro.y;
  GyroX = g.gyro.z;
  
  PTemp = temp.temperature;
}

void SetupIMU(){
  SetupLIS();
  SetupMPU();
}

void readIMU(){
  readLIS();
  readMPU();
}
