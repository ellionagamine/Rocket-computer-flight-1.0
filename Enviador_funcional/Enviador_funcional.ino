// SimpleTx - the master or the transmitter
//Send
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
//BMP 180 stuff
#include <Wire.h>
#include <Adafruit_BMP085.h>
//GPS  stuff
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
//not all #defines used but here for documentation
#define CE_PIN   7
#define CSN_PIN 8
#define MOSI_PIN 11
#define MISO_PIN 12
#define SCK_PIN 13
#define seaLevelPressure_hPa 1013.25
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;
const byte slaveAddress[5] = {'R', 'x', 'A', 'A', 'A'};

Adafruit_BMP085 bmp;
TinyGPSPlus gps;
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

char txNum = '0';
SoftwareSerial ss(RXPin, TXPin);

struct data
{
  float altitude;
  float latitude;
  float longitude;
} sentData;

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 1000;

void setup()
{
  Serial.begin(9600);
  Serial.println("SimpleTx Starting");
  ss.begin(GPSBaud);
  radio.begin();
  radio.setDataRate( RF24_2MBPS );
  radio.setRetries(3, 5); // delay, count
  radio.openWritingPipe(slaveAddress);
  if (!bmp.begin()){
    Serial.print("bmp180 fail");
  }
}

void loop()
{
  currentMillis = millis();
  if (currentMillis - prevMillis >= txIntervalMillis)
  {
    getData();
    send();
    prevMillis = millis();
  }
}

void send()
{
  bool rslt;
  rslt = radio.write( &sentData, sizeof(sentData) );
  if (rslt)
  {
    Serial.println("  Acknowledge received");
  }
  else
  {
    Serial.println("  Tx failed");
  }
}

void getData(){
  sentData.altitude = bmp.readAltitude(seaLevelPressure_hPa * 100);
  while (ss.available() > 0){
    gps.encode(ss.read());
    if (gps.location.isUpdated()){
      sentData.longitude = gps.location.lat();
      sentData.latitude = gps.location.lng();
    }
  }
}