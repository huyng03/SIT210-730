// TASK2.1P---SIT210
#include <WiFiNINA.h>
#include "ThingSpeak.h"
#include <DHT11.h>
#include "secrets.h"

#include <BH1750.h>
#include <Wire.h>

char ssid[] = "NOKIA-1FF1";   // your network SSID (name) 
char pass[] = "k6uL2LaeMw";   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = 2918208;
const char * myWriteAPIKey = "5A0RLFYA51NH2W9H";

String myStatus = "";

DHT11 dht11(2);
BH1750 lightMeter;  // SCL - A5
                    // SDA - A4

int time = 30; // 30sec
int fieldNumber = 0;

void setup() {
  delay(1000); // Give sensors a second to wake up
  Serial.begin(115200);  // Initialize serial
  Wire.begin();
  lightMeter.begin();
  
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true); // don't continue
  }
  ThingSpeak.begin(client);  //Initialize ThingSpeak

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      for (int i=0; i < 5;i++) {
        Serial.print(".");
        delay(1000); 
      }  
    } 
    Serial.println("\nConnected.");
    delayUpdate(30, false);
  }
}

// ======================================================================

void loop() {

  int temperature = dht11.readTemperature();
  float lux = lightMeter.readLightLevel();

  // display
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");

  // assign the fields with the values
  ThingSpeak.setField(1, temperature); 
  ThingSpeak.setField(2, lux);         

  // checking the connection with ThingSpeaks -> push the package
  checkConnection(myChannelNumber, myWriteAPIKey);  

  for (int i=0; i<30;i++) {
    Serial.print("=");
    delay(1000);
  }
  Serial.println();
}

// ======================================================================


void delayUpdate(int time, bool delayConfirm) {
  for (int i=0; i<time;i++) {
    Serial.print("=");
    if (delayConfirm) {
      delay(1000);
    }
  }
  Serial.println("");
}

void checkConnection(long channelNumber, const char* API) {
  ThingSpeak.setStatus(myStatus); // set the status
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200)
  {
    delayUpdate(time, false);
    Serial.println("Channel update successful.");
  }
  else  
  {
    delayUpdate(time, false);
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
}
