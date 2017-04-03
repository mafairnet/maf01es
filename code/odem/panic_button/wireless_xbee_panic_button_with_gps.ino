#include <SoftwareSerial.h>
#include <TinyGPS++.h>

static const int RXPin = 4, TXPin = 3; //(4, 5);// RX, TX
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

// The serial connection to the XBee
SoftwareSerial Xbee(5, 6);

int alarmButton = 0;
int alarmValue = 0;           // variable to store the value read

bool gpsData = false;

void setup()
{
  pinMode(13, OUTPUT);
  
  Serial.begin(9600); 
  Serial.println("Booting...");
  
  Xbee.begin(9600);
  Serial.println("XBee Radio: OK");

  ss.begin(GPSBaud);
  Serial.println("GPS       : OK");

  Serial.print(F("Based on TinyGPS++ library v."));
  Serial.println(TinyGPSPlus::libraryVersion());
  
}

void loop()
{
  /*alarmValue = analogRead(alarmButton);

  if(alarmValue >= 1000)
  {
    Xbee.print("NOK:");
    digitalWrite(13, HIGH);
  }
  else
  {
    Xbee.print("OK:");
    digitalWrite(13, LOW);
  }*/

  /*while (ss.available() > 0)
  {
    gpsData = true;
    if (gps.encode(ss.read()))
    {
      if (gps.location.isValid())
      {
        Xbee.print(gps.location.lat(), 6);
        Xbee.print(F(","));
        Xbee.print(gps.location.lng(), 6);
      }
      else
      {
        Xbee.print(F("I"));
        Xbee.print(F(","));
        Xbee.print(F("I"));
      }
      if (gps.date.isValid())
      {
        Xbee.print(gps.date.month());
        Xbee.print(F("/"));
        Xbee.print(gps.date.day());
        Xbee.print(F("/"));
        Xbee.print(gps.date.year());
      }
      else
      {
        Xbee.print(F("I"));
      }
      if (gps.time.isValid())
      {
        if (gps.time.hour() < 10) Serial.print(F("0"));
        Xbee.print(gps.time.hour());
        Xbee.print(F(":"));
        if (gps.time.minute() < 10) Serial.print(F("0"));
        Xbee.print(gps.time.minute());
        Xbee.print(F(":"));
        if (gps.time.second() < 10) Serial.print(F("0"));
        Xbee.print(gps.time.second());
        Xbee.print(F("."));
        if (gps.time.centisecond() < 10) Serial.print(F("0"));
        Xbee.print(gps.time.centisecond());
      }
      else
      {
        Xbee.print(F("I"));
      }
    }
    else
    {
      Xbee.print(F("*"));
      Xbee.print(F(","));
      Xbee.print(F("*"));
    }
  }

  if(!gpsData)
  {
    Xbee.print(F("N"));
    Xbee.print(F(","));
    Xbee.print(F("N"));
  }
  
  Xbee.println();

  Serial.println(alarmValue);
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }

  gpsData = false;
  //delay(500);
  smartDelay(1000);
  */

  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

void displayInfo()
{
  alarmValue = analogRead(alarmButton);

  if(alarmValue >= 1000)
  {
    Xbee.print("#NOK*");
    digitalWrite(13, HIGH);
  }
  else
  {
    Xbee.print("#OK*");
    digitalWrite(13, LOW);
  }
  
  //Xbee.print(F(""));
  if (gps.location.isValid())
  {
    Xbee.print(gps.location.lat(), 6);
    Xbee.print(F(","));
    Xbee.print(gps.location.lng(), 6);
  }
  else
  {
    Xbee.print(F("INVALID"));
  }
  Xbee.print(F(";"));
  if (gps.date.isValid())
  {
    Xbee.print(gps.date.month());
    Xbee.print(F("/"));
    Xbee.print(gps.date.day());
    Xbee.print(F("/"));
    Xbee.print(gps.date.year());
  }
  else
  {
    Xbee.print(F("INVALID"));
  }

  Xbee.print(F(";"));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Xbee.print(gps.time.hour());
    Xbee.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Xbee.print(gps.time.minute());
    Xbee.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Xbee.print(gps.time.second());
    Xbee.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Xbee.print(gps.time.centisecond());
  }
  else
  {
    Xbee.print(F("INVALID"));
  }

  Xbee.print("\n");
}


