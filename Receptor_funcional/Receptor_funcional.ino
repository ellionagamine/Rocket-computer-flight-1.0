// SimpleRx - the slave or the receiver
//Radio
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//not all #defines used but here for documentation
#define CE_PIN   7
#define CSN_PIN 8
#define MOSI_PIN 11
#define MISO_PIN 12
#define SCK_PIN 13

const byte thisSlaveAddress[5] = {'R', 'x', 'A', 'A', 'A'};

RF24 radio(CE_PIN, CSN_PIN);

struct data
{
  float height;
  float latitude;
  float longitude;
} receivedData;

bool newData = false;

void setup()
{
  Serial.begin(9600);
  Serial.println("SimpleRx Starting");
  radio.begin();
  radio.setDataRate( RF24_2MBPS );
  radio.openReadingPipe(1, thisSlaveAddress);
  radio.startListening();
}

void loop()
{
  getData();
  showData();
}

void getData()
{
  if ( radio.available() )
  {
    radio.read( &receivedData, sizeof(receivedData) );
    newData = true;
  }
}

void showData()
{
  if (newData == true)
  {
    Serial.print("Latitude : ");
    Serial.print(receivedData.latitude, 6);
    Serial.print("\t");
    Serial.print("Longitude ");
    Serial.print(receivedData.longitude, 6);
    Serial.print("\t");
    Serial.print("Altura : ");
    Serial.println(receivedData.height);
    newData = false;
  }
}
