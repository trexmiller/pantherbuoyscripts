//Function to set up IMU with calibration values

//This equation converts mag to heading... float Heading = 180*atan2( mag.m.y, mag.m.z)/PI; 
//But IMU is not flat, it is standing up so need to use X and Y axes instead of y and z.
//Maybe use adafruit library instead for LIS3MDL... #include <Adafruit_LIS3MDL.h>

//Maybe use MPU6050_light library instead for MPU6050 accel/gyro or adafruit library

void setupIMU()
{
  tcaselect(7);
  mcp.pinMode(11, OUTPUT); //Controls relay to turn on chl fluorometer
  mcp.digitalWrite(11,HIGH); 
  delay(1000);
  tcaselect(2);
  delay(100);
  IMU.begin();
  //IMU calibration values determined by DIY_Calibration scripts
  IMU.setAccelFS(3);
  IMU.setAccelODR(5);
  IMU.setAccelOffset(-0.015717, -0.010586, -0.036296);
  IMU.setAccelSlope (0.993018, 0.993259, 1.002441);
  IMU.setMagnetFS(0);
  IMU.setMagnetODR(8);
  IMU.setMagnetOffset(14.396362, 34.963989, 0.334473);
  IMU.setMagnetSlope (1.102481, 1.079620, 1.370087);
  IMU.setGyroFS(2);
  IMU.setGyroODR(5);
  IMU.setGyroOffset (2.125702, 1.891205, 0.438385);
  IMU.setGyroSlope (0.964258, 0.918662, 1.034757);
  IMU.magnetUnit = MICROTESLA;
}

//Function to read IMU
void readIMU() {
  tcaselect(7);
  mcp.pinMode(11, OUTPUT); //Controls relay to turn on chl fluorometer
  mcp.digitalWrite(11,HIGH); 
  delay(1000);
  tcaselect(2);
  delay(100);

  //Turn on magnetometer
  IMU.setMagnetODR(2);
  delay(100);
  float x, y, z;
  //Get and average magnetometer readings
  MagX = 0; MagY = 0; MagZ = 0;
  for (int i = 1; i <= MagN; i++)
  {
    IMU.readMagnet(x, y, z);
    MagX += x; MagY += y;  MagZ += z;
  }
  MagX /= MagN;    MagY /= MagN;  MagZ /= MagN;

  //Turn off magnetometer
  IMU.setMagnetODR(0);

  //Calculate heading, field strength and inclination
  Heading = atan2(MagY, MagX) * 180 / PI + 180;
  fieldStrength = sqrt(MagX * MagX + MagY * MagY + MagZ * MagZ);
  inclination = atan(-MagZ / sqrt(MagX * MagX + MagY * MagY)) * 180 / PI; 

  //Get Roll, Pitch, and raw accelerometer data
  //Turn on accelerometer (and gyro)
  IMU.setAccelODR(2);
  delay(100);

  AccelX = 0; AccelY = 0; AccelZ = 0;
  for (int i = 1; i <= AccelN; i++)
  {
    IMU.readAccel(x, y, z);
    AccelX += x;    AccelY += y;     AccelZ += z;
  }
  AccelX /= float(AccelN); AccelY /= float(AccelN); AccelZ /= float(AccelN);

  //Turn off accelerometer and gyro
  IMU.setAccelODR(0);

  //Calculate roll and pitch
  if (abs(AccelX) > 0.1 || abs(AccelZ) > 0.1) Roll = atan2(AccelX, AccelZ) * 180 / PI;
  else Roll = 0;
  if (abs(AccelY) > 0.1 || abs(AccelZ) > 0.1) Pitch = atan2(AccelY, AccelZ) * 180 / PI;
  else Pitch = 0;
  //Read raw gyro values
  IMU.readGyro(GyroX, GyroY, GyroZ);
}
