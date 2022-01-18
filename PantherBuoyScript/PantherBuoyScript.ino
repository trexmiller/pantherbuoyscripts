/********************************Panther Buoy Firmware***************************************
This is a script(s) to run the Panther Buoy data logger v20211104B. The script reads sensors, 
saves data to the onboard SD card and sends data with the embedded modem to lakestat.com. A
menu is provided that accepts user input.

Author: Todd Miller, University of Wisconsin - Milwaukee

This is a work in progress!
/*********************************************************************************************/
#define TINY_GSM_MODEM_SIM800
#include <OneWire.h>
#include <Wire.h>
#include <TinyGsmClient.h>
#include <RTCZero.h>
#include <SPI.h>
#include "TinyGPS++.h"
#include "wiring_private.h"
#include "Adafruit_MCP23017.h"
#include "Statistic.h"
#include <U8g2lib.h>
#include <Arduino_LSM9DS1.h>
#include <SDI12.h>
#include "SdFat.h"
#include "sdios.h"
#include "Adafruit_BME680.h"
#include <Adafruit_SleepyDog.h>
/********************************Change options below******************************************/
/***************************These can be changed/overwritten in the user menu*******************/
//Minutes between scans (as milliseconds). 
long Interval = 1000 * 60 * 2;

//Should LCD messenges be turned on? 0 = No, anything larger than 0 = yes.
int doLCD = 1;

//Should verbose reporting to serial monitor be turned on? 0 = No, anything larger than 0 = yes.
int Verbose = 0;

//Should the program wait to get GPS fix? 0 = No, anything larger than 0 = yes.
int GPSHold = 0;

//Change to match deployment location
#define SEALEVELPRESSURE_HPA (1013.25)

/****************************End of buoy specific options*************************************/
//SD Card defines
const uint8_t chipSelect = 4;
SdFat sd;
SdFile root;
File myFile;

/*************Create new serial port on digital pins 10 and 12 of feather***********************/
//Pin 10 and 12 are the common RX and TX for the UART multiplexer (74HCT4052PW)
//The UART multiplexer is used to read GPS and two RS232 ports
#define PIN_SERIAL2_RX       (34ul)               // Pin description number for PIO_SERCOM on D12
#define PIN_SERIAL2_TX       (36ul)               // Pin description number for PIO_SERCOM on D10
#define PAD_SERIAL2_TX       (UART_TX_PAD_2)      // SERCOM pad 2
#define PAD_SERIAL2_RX       (SERCOM_RX_PAD_3)

Uart Serial2(&sercom1, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX);
// Interrupt handler for SERCOM1
void SERCOM1_Handler()
{
  Serial2.IrqHandler();
}

//Use  tinyGPSplus library to read gps
TinyGPSPlus gps;
#define ss Serial2 //define Serial2 as ss for gps communications

//Create instance for GPIO expander
Adafruit_MCP23017 mcp;

//Create instance for BME sensor
Adafruit_BME680 bme;

//Define analog pin for PAR sensor
#define PARpin A2

/****************************************Define LCD**********************************************/
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
#define panther_width 124
#define panther_height 64

//Panther buoy logo image below
static const unsigned char panther_bits[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xf0, 0x07, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0xe0, 0x1f, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0xc0, 0x30, 0x00, 0x00,
  0x00, 0xe0, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x08, 0x00, 0x00,
  0x60, 0x18, 0x00, 0x00, 0x00, 0x60, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x18, 0x08, 0x00, 0x00, 0x60, 0x18, 0x00, 0x00, 0x00, 0x60, 0x60, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x18, 0x88, 0x1b, 0xcc, 0x71, 0x38, 0xc3, 0x0d,
  0x1b, 0x70, 0x30, 0x0c, 0xc3, 0x0c, 0xc1, 0x00, 0x18, 0x0c, 0x30, 0x8c,
  0x71, 0x18, 0x42, 0x18, 0x07, 0x60, 0x18, 0x04, 0x61, 0x18, 0x41, 0x00,
  0xf8, 0x07, 0x30, 0x06, 0x31, 0x18, 0x62, 0x18, 0x03, 0xe0, 0x1f, 0x04,
  0x31, 0x18, 0x21, 0x00, 0x1c, 0x00, 0x38, 0x02, 0x31, 0x0c, 0x62, 0x18,
  0x03, 0x20, 0x30, 0x04, 0x31, 0x38, 0x22, 0x00, 0x0c, 0xc0, 0x18, 0x82,
  0x30, 0x0c, 0x22, 0x80, 0x01, 0x30, 0x30, 0x86, 0x30, 0x18, 0x12, 0x00,
  0x0c, 0x40, 0x18, 0x86, 0x30, 0x0c, 0x23, 0x80, 0x01, 0x10, 0x30, 0x82,
  0x30, 0x18, 0x12, 0x00, 0x0c, 0x40, 0x18, 0x83, 0x30, 0x0c, 0x41, 0x80,
  0x01, 0x10, 0x30, 0xc6, 0x30, 0x0c, 0x0e, 0x00, 0x0e, 0x40, 0x1c, 0xc3,
  0x38, 0x06, 0xc1, 0x8c, 0x01, 0xf0, 0x1d, 0xee, 0x60, 0x06, 0x0e, 0x00,
  0x00, 0x80, 0x00, 0x00, 0x60, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x08,
  0x80, 0x01, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x03, 0x00, 0x00,
  0x00, 0xc0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xfe, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x1f, 0xfe, 0x0f, 0x00, 0x3e,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0xfe,
  0xff, 0xff, 0x07, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20,
  0x00, 0x00, 0x02, 0xc0, 0x0f, 0xff, 0xff, 0x00, 0x0c, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x02, 0x00, 0x3f, 0xc0, 0xff, 0x01,
  0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x02, 0xe0,
  0x7f, 0x00, 0x08, 0xe0, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00,
  0x00, 0x80, 0x07, 0xfc, 0x3f, 0x00, 0x00, 0x70, 0xf8, 0x01, 0x00, 0x00,
  0x00, 0x00, 0x07, 0x00, 0x00, 0xe4, 0x1f, 0xe8, 0x03, 0x38, 0x00, 0x08,
  0x00, 0x0e, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0xf0, 0x7f, 0xe8,
  0x03, 0x70, 0x70, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00,
  0x00, 0xfe, 0xff, 0xfb, 0x9f, 0x7f, 0x70, 0x00, 0x00, 0x60, 0x00, 0x00,
  0x00, 0x08, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x80, 0x7f,
  0x02, 0x40, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x60, 0xe0, 0xff, 0xff, 0xff,
  0xff, 0x01, 0x00, 0xf8, 0x1f, 0x40, 0x00, 0x00, 0x00, 0x03, 0x00, 0x10,
  0xf8, 0xff, 0xff, 0xff, 0xff, 0x03, 0x00, 0xe0, 0x3f, 0x7c, 0x00, 0x00,
  0x80, 0x03, 0x00, 0x04, 0xfc, 0xff, 0xff, 0xfb, 0xff, 0x00, 0x00, 0x80,
  0x3f, 0x70, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x01, 0xfb, 0xff, 0xff, 0xf9,
  0x3f, 0x00, 0x01, 0x00, 0x1e, 0x30, 0x00, 0x00, 0xf0, 0x00, 0xc0, 0x80,
  0xf1, 0xff, 0xff, 0xf9, 0x07, 0x10, 0x10, 0x00, 0x08, 0x3a, 0x00, 0x00,
  0xf0, 0x00, 0x70, 0xe0, 0xf8, 0xff, 0xff, 0x79, 0xc0, 0x01, 0x0e, 0x00,
  0x00, 0xff, 0x00, 0x00, 0x78, 0x00, 0x1c, 0x70, 0xfc, 0xff, 0xff, 0x84,
  0x3c, 0xf0, 0x01, 0x07, 0x00, 0x2f, 0x04, 0x00, 0x78, 0x00, 0x0e, 0x78,
  0xfc, 0xff, 0xff, 0xfc, 0x87, 0x1f, 0xf0, 0x07, 0x80, 0x27, 0x00, 0x00,
  0x7c, 0x00, 0x07, 0x3c, 0xfe, 0xff, 0xff, 0x7c, 0xfc, 0x03, 0xff, 0x03,
  0xc0, 0x73, 0x00, 0x00, 0x7c, 0x80, 0x03, 0x1e, 0xfe, 0xff, 0xff, 0xdc,
  0x7f, 0xf0, 0xff, 0x01, 0xe0, 0x13, 0x02, 0x00, 0x7c, 0xc0, 0x01, 0x0e,
  0xf7, 0xff, 0xff, 0xfc, 0x0f, 0xff, 0xff, 0x8f, 0xff, 0x19, 0x00, 0x00,
  0x7c, 0xe0, 0x00, 0x0f, 0xf7, 0xff, 0xff, 0xfc, 0xfb, 0xbf, 0xff, 0xff,
  0xff, 0x7f, 0x00, 0x00, 0x7c, 0x70, 0x80, 0x87, 0xf7, 0xff, 0xff, 0xfd,
  0xff, 0xcf, 0xff, 0xff, 0xff, 0x03, 0x03, 0x00, 0xf8, 0x38, 0xc0, 0x03,
  0xf3, 0xff, 0xff, 0xfd, 0xff, 0xe7, 0x1f, 0xfc, 0xff, 0x00, 0x00, 0x00,
  0xf8, 0x1f, 0xc0, 0x83, 0xf3, 0xff, 0xff, 0xfb, 0xff, 0xe7, 0x1f, 0x00,
  0x48, 0x00, 0x00, 0x00, 0xf0, 0x1f, 0xe0, 0x83, 0xf3, 0xff, 0xff, 0xf3,
  0xff, 0xe3, 0x7d, 0x00, 0x28, 0x00, 0x00, 0x00, 0xf0, 0x0f, 0xf0, 0x81,
  0xf3, 0xff, 0xff, 0xf7, 0xff, 0xe7, 0xf3, 0x00, 0x10, 0x00, 0x00, 0x00,
  0xe0, 0x87, 0xf0, 0x91, 0xe3, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xf3, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xc0, 0x07, 0xf0, 0x91, 0xe7, 0xff, 0xff, 0xff,
  0xff, 0xcf, 0xe7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfc, 0xc0,
  0xe7, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xe7, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xfd, 0x80, 0xc7, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xc7, 0x03,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xfd, 0xc0, 0xc7, 0xff, 0xff, 0xff,
  0xff, 0xff, 0x07, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0xff, 0x80,
  0x87, 0xff, 0xff, 0xff, 0x1f, 0xff, 0x0f, 0x27, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xfe, 0xc0, 0x0f, 0xff, 0xff, 0xff, 0x03, 0xfc, 0xdf, 0x31,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x80, 0x0f, 0xff, 0xff, 0xff,
  0x01, 0xf8, 0xff, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xcc,
  0x1f, 0xfe, 0xff, 0xff, 0x01, 0xf0, 0xff, 0x1c, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xc8, 0x1f, 0xf8, 0xff, 0xff, 0x01, 0xf0, 0xff, 0xbf,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x3f, 0xf0, 0xff, 0xff,
  0x07, 0xe0, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x7f, 0xc0, 0xff, 0xff, 0x0f, 0xe0, 0x7f, 0x70, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0xff, 0xff, 0x3f, 0xe0, 0x7f, 0x1e,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff,
  0x7f, 0x80, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};

/************************************Define Cell Modem Communications**************************/
//Hardware serial for modem communications
#define SerialAT Serial1
TinyGsm modem(SerialAT);
//TinyGsmClient client(modem);
/****************************************Define Temp String on One Wire *************************/
#define NSENSORS 6 //Number of DS18B temperature sensors to measure
float Temp[NSENSORS], TempF[NSENSORS];
#define ONE_WIRE_BUS 6 //Temp chain on digital pin 6
OneWire oneWire(ONE_WIRE_BUS);
/****************************************Define RDO Pro Sensor ***********************************/
#define RDOpin 11        
SDI12 sdi12(RDOpin);
/****************************************Define Fluorometer Sensor Pins***************************/
const int PhycoPin = A0;
const int ChlPin = A1;
/****************************************Create Data Variables **********************************/
//Variable to hold current millis
unsigned long staticTime;

//Variables needed for saving and sending data with cellular modem (modem response and payload)
char result_char[1000] = {}; //make sufficiently large for holding modem and network response
char result[1000] = {};  //make sufficiently large for holding result of modem data
char url1[1000];
char url2[1000];
char url[4000];

//Variables for saving and writing data to SD card
char SDData[1200];
char TimeDat[100];
char IMUData[400];
char ModemData[100];
char GPSData[500];
char BattData[20];
char FluorData[20];
char PARData[20];
char TempData[100];
char dataFile[] = "Data.csv"; //This can be overwritten by user menu

//Real time clock (RTC) date and time variables
byte year;
byte month;
byte day;
byte hour;
byte minute;
byte second;

//Variables to hold date and time recieved from modem. These are used to set the RTC
char CCLK[100];
char Date[10];
char Time[10];
int Year;
int Month;
int Day;
int Hour;
int Minute;
char Sec[10];
int Second;

//Sensor data variables
float Batv; //Battery voltage
float mvolts; //millivolts read from any analog pin
float Lat;  //GPS latitude
float Lon;  //GPS longitude
float Sat;  //Number of GPS satelites connected
float Alt;  //GPS altitude
float Course; //GPS course over ground
float Speed;  //GPS speed over ground
float Hop;  //GPS horizontal dilution of precision
int GPSYear; //Current year reported by GPS
int GPSMonth; //Current month reported by GPS
int GPSDay;   //Current day reported by GPS
int GPSHour;  //Current hour reported by GPS
int GPSMin;   //Minutes reported by GPS
int GPSSec;   //Seconds reported by GPS
float AccelX;
float AccelY;
float AccelZ;
float MagX;
float MagY;
float MagZ;
unsigned int AccelN = 50;
unsigned int MagN = 50;
float GyroX;
float GyroY;
float GyroZ;
float Roll;
float Pitch;
float Heading;
float fieldStrength;
float inclination;

float GainVal;
float PhycoAvg;
float PhycoStd;
float ChlAvg;
float ChlStd;
float FluorAvg;
float FluorStd;
float PARAvg = 0.000;
float PARStd = 0.000;
float ATemp;
float Pressure;
float Humidity;
float Gas;
float BMEAlt;

float DO;
float DOS;
float RDOTemp;

//Variables to hold statistics calculated by statistic library
Statistic fluorStats;
Statistic PARStats;

//Wind sensor defines and variables
#define WindSensorPin (11) //Wind speed measured on pin D11
volatile unsigned long Rotations; // cup rotation counter used in interrupt routine
volatile unsigned long ContactBounceTime; // Timer to avoid contact bounce in interrupt routine
float WindSpeed; // speed miles per hour
int VaneValue;// raw analog value from wind vane
int WindDir;// translated 0 - 360 direction
int CalDirection;// converted value with offset applied
int LastValue;
#define Offset 0;
long previousMillis = 0;


/*********************************Auxiliary Functions*******************/
//Function to change ports on the I2C multiplexer
#define TCAADDR 0x70
void tcaselect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

//Function to read Battery voltage
void readBat() {
  Batv = readVolts(A4);
}

//Function to read voltage on some analog pin and output in millivolts
float readVolts(int pin) {
  analogReadResolution(12);
  mvolts = analogRead(pin);
  mvolts *= 2;
  mvolts *= (3.3 / 4096);
  mvolts *= 1000;
  return mvolts;
}

//Initiate the RTC
RTCZero rtc;

//Variable to hold watchdog time
int countdownMS;

//Function to set all pins low on GPIO expander and thus all swtiched power rails
void pinsLow(){
  for (int i = 0; i < 15; i++){
    tcaselect(7);
    delay(100);
    mcp.digitalWrite(i,LOW);
    delay(100);
  }
}

unsigned long HoldTime = 1000 * 60 * 10;

/********************************************Main Setup*********************************************/
void setup()
{
  Serial.begin(115200);
  delay(5000);
  Serial.println("Setting things up, please wait. Enter M now to bring up the main menu");
  SerialAT.begin(115200);
  Wire.begin();
  
  tcaselect(5);
  delay(100);
  u8g2.begin();
  //Splash screen
  u8g2.clearBuffer();
  u8g2.drawXBMP(0,0,panther_width, panther_height, panther_bits);
  u8g2.sendBuffer();
  delay(6000);
  
  if (doLCD > 0) {
    tcaselect(5);
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_t0_12_te);
    u8g2.drawStr(0, 12, "Setting up...");
    u8g2.sendBuffer();
    delay(1000);
    u8g2.clearBuffer();
    u8g2.drawXBMP(0,0,panther_width, panther_height, panther_bits);
    u8g2.sendBuffer();
  }

  asm(".global _printf_float"); //for float conversion in sprintf
  pinPeripheral(10, PIO_SERCOM);
  pinPeripheral(12, PIO_SERCOM);

  sd.begin(chipSelect);
  SPI.begin();
  sdi12.begin();
  mcp.begin();
  rtc.begin();
  createSDFile();
  //setupIMU();

  pinMode(9, OUTPUT);
  digitalWrite(9, LOW); //Set ATTiny low
  delay(3000);
  ss.begin(9600);
  u8g2.begin();
  readBat();
  setupBME();

  Serial.println("Close serial monitor before deploying or enter M for the main menu");
  Serial.print("Deployment in 20 seconds");
  staticTime = millis();
  while ((millis() - staticTime) <= 20000) {
    if (Serial.read() == 'M') {
      menu();

    }
      digitalWrite(13, HIGH);
      delay(50);
      digitalWrite(13, LOW);
      delay(50);
      Serial.println(".");
    }
    
    Serial.println("");

  
  if (doLCD > 0) {
    tcaselect(5);
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_t0_12_te);
    char buf[100];
    char buf2[100];
    sprintf(buf2, "20%02u-%02u-%02u %02u:%02u:%02u", rtc.getYear(), rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());
    u8g2.drawStr(0, 12, "Current Time:");
    u8g2.sendBuffer();
    u8g2.drawStr(0, 24, buf2);
    u8g2.sendBuffer();
    u8g2.drawStr(0, 36, "");
    u8g2.sendBuffer();
    u8g2.drawStr(0, 48, "Battery Voltage:");
    u8g2.sendBuffer();
    u8g2.drawStr(0, 60, itoa(Batv, buf, 10)); u8g2.drawStr(30, 60, "mV");
    u8g2.sendBuffer();
    delay(5000);
  }
  char buf[100];
  u8g2.clearBuffer();
  
  GPSON();
  if (millis() < HoldTime) {
    if (GPSHold > 0) {\
      if (doLCD > 0) {
        tcaselect(5);
        u8g2.setFont(u8g2_font_t0_12_te);
        u8g2.drawStr(0, 12, "Waiting for GPS Fix");
        u8g2.sendBuffer();
      }

      int SatSetup = -1;
      while (SatSetup < 5) {
        while (ss.available() > 0)
          gps.encode(ss.read());
        SatSetup = gps.satellites.value();
        Serial.print("Waiting for GPS satellite fix. "); Serial.print("GPS Satellites = "); Serial.println(SatSetup);
        delay(1000);
      }

      float LatSetup = -1;
      while (LatSetup < 0) {
        while (ss.available() > 0)
          gps.encode(ss.read());
          LatSetup = gps.location.lat();
          Serial.print("Waiting for location update. "); Serial.print("Latitude = "); Serial.print(LatSetup); Serial.print(", Longitude = "); Serial.println(gps.location.lng());

        if (doLCD > 0) {
          tcaselect(5);
          u8g2.setFont(u8g2_font_t0_12_te);
          u8g2.drawStr(0, 24, "Waiting for Location");
          u8g2.sendBuffer();
          delay(1000);
        }

      }

      if (doLCD > 0) {
        tcaselect(5);
        u8g2.setFont(u8g2_font_t0_12_te);
        u8g2.drawStr(0, 36, "Satelites: "); u8g2.drawStr(36, 48, itoa(Sat, buf, 10));
        u8g2.sendBuffer();
        u8g2.drawStr(0, 48, "Latitude: "); u8g2.drawStr(48, 60, itoa(Lat, buf, 10));
        u8g2.sendBuffer();
        delay(5000);
      }

    }
  }

  Serial.println("Setup is complete. First round of measurements will start at the next interval. Please wait");
  if (doLCD > 0) {
    tcaselect(5);
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_t0_12_te);
    u8g2.drawStr(0, 12, "Setup complete!");
    u8g2.sendBuffer();
    delay(3000);
    u8g2.drawStr(0, 24, "Waiting for interval");
    u8g2.sendBuffer();
    delay(3000);
    u8g2.drawStr(0, 36, "Waiting for interval");
    u8g2.sendBuffer();
    delay(5000);
  }
  previousMillis = Interval;
}


/************************************************Main Program Loop***********************************************/
void loop()
{
  tcaselect(5);
  u8g2.clearBuffer();
  u8g2.drawXBMP(0, 0, panther_width, panther_height, panther_bits);
  u8g2.sendBuffer();
  delay(2000);
  
  char buf4[50];
  char buf[50];
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > Interval) {
    //Serial.print(F("Free SRAM ")); Serial.println(freeMemory());

    if (doLCD > 0) {
      tcaselect(5);
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_t0_12_te);
      u8g2.drawStr(0, 12, "Reading sensors...");
      u8g2.sendBuffer();
      delay(1500);
    }

    Serial.println("Starting a round of measurements.");
    Serial.println("Reading temp chain....");

    readTemps();
    readBME();

    if (doLCD > 0) {
      tcaselect(5);
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_t0_12_te);
      u8g2.drawStr(0, 12, "WTemp Data (4 nodes)");
      u8g2.sendBuffer();

      sprintf(buf4, "%f", Temp[0]);
      u8g2.drawStr(0, 24, "WaterTemp(0m):"); u8g2.drawStr(85, 24, buf4),
                   u8g2.sendBuffer();

      sprintf(buf4, "%f", Temp[1]);
      u8g2.drawStr(0, 36, "WaterTemp(1m):"); u8g2.drawStr(85, 36, buf4),
                   u8g2.sendBuffer();

      sprintf(buf4, "%f", Temp[2]);
      u8g2.drawStr(0, 48, "WaterTemp(2m):"); u8g2.drawStr(85, 48, buf4),
                   u8g2.sendBuffer();

      sprintf(buf4, "%f", Temp[3]);
      u8g2.drawStr(0, 60, "WaterTemp(3m):"); u8g2.drawStr(85, 60, buf4),
                   u8g2.sendBuffer();
      delay(3000);
    }


    Serial.println("Temp chain data acquired.");
    Serial.println("Reading IMU and Wind...");

    if (doLCD > 0) {
      tcaselect(5);
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_t0_12_te);
      u8g2.drawStr(0, 12, "IMU Data");
      u8g2.sendBuffer();
      u8g2.drawStr(0, 24, "Roll="); u8g2.drawStr(55, 24, itoa(Roll, buf, 10)); u8g2.drawStr(80, 24, "Degrees");
      u8g2.sendBuffer();
      u8g2.drawStr(0, 36, "Pitch="); u8g2.drawStr(55, 36, itoa(Pitch, buf, 10)); u8g2.drawStr(80, 36, "Degrees");
      u8g2.sendBuffer();
      u8g2.drawStr(0, 48, "Heading="); u8g2.drawStr(55, 48, itoa(Heading, buf, 10)); u8g2.drawStr(80, 48, "Degrees");
      u8g2.sendBuffer();
      delay(4000);
    }

    //readWindDir();
    //readWindSpeed();

    if (doLCD > 0) {
      tcaselect(5);
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_t0_12_te);
      u8g2.drawStr(0, 12, "Wind Data:");
      u8g2.sendBuffer();
      u8g2.drawStr(0, 24, "WindDir="); u8g2.drawStr(60, 24, itoa(WindDir, buf, 10)); u8g2.drawStr(85, 24, "Degrees");
      u8g2.sendBuffer();
      u8g2.drawStr(0, 36, "WindSpd="); u8g2.drawStr(60, 36, itoa(WindSpeed, buf, 10)); u8g2.drawStr(85, 36, "MPH");
      u8g2.sendBuffer();
      delay(4000);
    }

    Serial.println("IMU and Wind data acquired.");
    //Serial.print(F("Free SRAM ")); Serial.println(freeMemory());
    Serial.println("Reading GPS");

    readGPS();

    if (doLCD > 0) {
      tcaselect(5);
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_t0_12_te);
      u8g2.drawStr(0, 12, "GPS Data");
      u8g2.sendBuffer();
      u8g2.drawStr(0, 24, "GPS Satelites="); u8g2.drawStr(85, 24, itoa(Sat, buf, 10));
      u8g2.sendBuffer();
      u8g2.drawStr(0, 36, "");
      u8g2.sendBuffer();
      u8g2.drawStr(0, 48, "Lat,Lon:");
      u8g2.sendBuffer();
      char buf3[100];
      sprintf(buf3, "%f,%f",
              Lat,
              Lon
             );
      u8g2.drawStr(0, 60, buf3);
      u8g2.sendBuffer();
      delay(4000);
    }




    //Serial.print(F("Free SRAM ")); Serial.println(freeMemory());
    Serial.println("GPS data acquired. Reading battery voltage");
    delay(1000);
    readBat();
    Serial.print("Battery voltage = "); Serial.print(Batv); Serial.println(" mV");
    Serial.println("Reading fluorometers");
    readChl();

    if (doLCD > 0) {
      tcaselect(5);
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_t0_12_te);
      u8g2.drawStr(0, 12, "Reading Fluorometers");
      u8g2.sendBuffer();
      u8g2.drawStr(0, 24, "Chl Avg="); u8g2.drawStr(70, 24, itoa(ChlAvg, buf, 10)); u8g2.drawStr(100, 24, "mV");
      u8g2.sendBuffer();
      u8g2.drawStr(0, 36, "Chl Std="); u8g2.drawStr(70, 36, itoa(ChlStd, buf, 10)); u8g2.drawStr(100, 36, "mV");
      u8g2.sendBuffer();
    }
    Serial.println("Chlorophyll data acquired");

    readPhyco();

    if (doLCD > 0) {
      tcaselect(5);
      u8g2.drawStr(0, 48, "Phyco Avg="); u8g2.drawStr(70, 48, itoa(PhycoAvg, buf, 10)); u8g2.drawStr(100, 48, "mV");
      u8g2.sendBuffer();
      u8g2.drawStr(0, 60, "Phyco Std="); u8g2.drawStr(70, 60, itoa(PhycoStd, buf, 10)); u8g2.drawStr(100, 60, "mV");
      u8g2.sendBuffer();
      delay(4000);
    }
    Serial.println("Phycocyanin data acquired");

    Serial.println("Reading DO");
    readDO();

    Serial.println("Reading PAR");

    readPAR();

    if (doLCD > 0) {
      tcaselect(5);
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_t0_12_te);
      u8g2.drawStr(0, 12, "PAR Data:");
      u8g2.sendBuffer();
      u8g2.drawStr(0, 24, "PAR="); u8g2.drawStr(24, 24, itoa(PARAvg, buf, 10)); u8g2.drawStr(85, 24, "uE");
      u8g2.sendBuffer();
      delay(4000);
    }

    Serial.println("PAR data acquired. Finished reading sensors");

    Serial.println("Saving data to SD card");
    saveSD();

    if (doLCD > 0) {
      tcaselect(5);
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_t0_12_te);
      u8g2.drawStr(0, 12, "Data saved to SD card");
      u8g2.sendBuffer();
      u8g2.drawStr(0, 24, "Starting modem coms");
      u8g2.sendBuffer();
      delay(1000);
    }
    Serial.println("Sending data....");
    SendData();
    //Serial.print(F("Free SRAM ")); Serial.println(freeMemory());
    pinsLow();
    PrintResults();
    
    Serial.println("***********************************Finished sending data, modem is powered off**************");
    Serial.println("***********************************Waiting for next round****************");
    previousMillis = currentMillis;
  }
}
