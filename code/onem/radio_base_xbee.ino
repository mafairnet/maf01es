/*
   Arduino Base Station Server
*/

#include <SPI.h>
#include <Ethernet.h>
#include <SoftwareSerial.h>
String inData;
String finalData;
String finalPage;

int indicatorOne = 9;
int indicatorTwo = 8;

// software serial #1: RX = digital pin 2, TX = digital pin 3
//Serial port comms (xBee)
SoftwareSerial XBee(2, 3);

String readString;
bool alarmStatus = false;

//MAC Adress
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

//Arduino network settings
IPAddress ip(192,168,0,2);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

//Main App Server
IPAddress server(192,168,0,3);

//Change to your domain name for virtual servers
char serverName[] = "192.168.0.3";

// change to your server's port
int serverPort = 88;

EthernetClient client;
// set this to the number of milliseconds delay
// this is 30 seconds
#define delayMillis 30000UL

unsigned long thisMillis = 0;
unsigned long lastMillis = 0;

char pageAdd[64];

void setup() {
  Serial.begin(9600);

  pinMode(indicatorOne, OUTPUT);
  pinMode(indicatorTwo, OUTPUT);
  
  //Xbee Coms begin
  XBee.begin(9600);
  
  // disable SD SPI
  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);

  // Start ethernet
  Serial.println(F("Starting ethernet..."));
  Ethernet.begin(mac, ip, gateway, gateway, subnet);
  Serial.println(Ethernet.localIP());

  delay(2000);
  Serial.println(F("Ready"));
  ledStatus(false);
}

void loop()
{
  inData = "";
  
  if (Serial.available())
  { // If data comes in from serial monitor, send it out to XBee
    XBee.write(Serial.read());
  }

  while (XBee.available() > 0)
  {
    char recieved = XBee.read();
    
    if (recieved == '#')
    {
      inData = "";
    }
    else
    {
      inData += recieved; 
    }

    // Process message when new line character is recieved
    if (recieved == '*')
    {
      Serial.println(inData);
      if(inData == "NOK*"){
        alarmStatus = true;
      }
      else
      {
        alarmStatus = false;
      }      
    }
    if (recieved == '\n')
    {
      finalData = inData;
      inData = "";
    }
  }

  Serial.print("Arduino Received: ");
  Serial.print(finalData);
  Serial.println("");
  
  if(alarmStatus)
  {
    XBee.println("ACK ALARLM");
    ledStatus(true);
    //sprintf(pageAdd,"/index.php?test=%s",finalData);
    finalPage = "/cti/devs/pci/event.php?data=" + finalData;
    finalPage.toCharArray(pageAdd, 64);
    if(!getPage(server,serverPort,pageAdd))
    {
      Serial.print(F("Fail "));
    }
    else
    {
      Serial.print(F("Pass "));
    }
  }
  else
  {
    ledStatus(false);
    XBee.println("ADIP");
  }

  inData = ""; // Clear recieved buffer
  
  delay(1000);
}

void ledStatus(bool on)
{
  if(on == true)
  {
    digitalWrite(indicatorOne, HIGH);
    digitalWrite(indicatorTwo, HIGH);
  }
  else
  {
    digitalWrite(indicatorOne, LOW);
    digitalWrite(indicatorTwo, LOW);
  }
}

byte getPage(IPAddress ipBuf,int thisPort, char *page)
{
  int inChar;
  char outBuf[128];

  Serial.print(F("connecting..."));

  if(client.connect(ipBuf,thisPort) == 1)
  {
    Serial.println(F("connected"));

    sprintf(outBuf,"GET %s HTTP/1.1",page);
    client.println(outBuf);
    sprintf(outBuf,"Host: %s",serverName);
    client.println(outBuf);
    client.println(F("Connection: close\r\n"));
  } 
  else
  {
    Serial.println(F("failed"));
    return 0;
  }

  // connectLoop controls the hardware fail timeout
  int connectLoop = 0;

  while(client.connected())
  {
    while(client.available())
    {
      inChar = client.read();
      Serial.write(inChar);
      // set connectLoop to zero if a packet arrives
      connectLoop = 0;
    }

    connectLoop++;

    // if more than 10000 milliseconds since the last packet
    if(connectLoop > 10000)
    {
      // then close the connection from this end.
      Serial.println();
      Serial.println(F("Timeout"));
      client.stop();
    }
    // this is a delay for the connectLoop timing
    delay(1);
  }

  Serial.println();

  Serial.println(F("disconnecting."));
  // close client end
  client.stop();

  return 1;
}
