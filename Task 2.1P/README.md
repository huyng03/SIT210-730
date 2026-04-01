// TASK2.1P---SIT210
#include <WiFiNINA.h>
#include "ThingSpeak.h"

char ssid[] = "NOKIA-1FF1";
char pass[] = "k6uL2LaeMw";
WiFiClient client;

unsigned long myChannelNumber = 2918327;
const char *myWriteAPIKey = "ZSIT6P3CCZE391BP";

const int trigPin = 3;
const int echoPin = 2;

long duration;
float distance;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi.");
  ThingSpeak.begin(client);
}

void loop() {
  // Trigger ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read echo time
  duration = pulseIn(echoPin, HIGH);

  // Convert to distance in cm
  distance = duration * 0.034 / 2.0;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Send to ThingSpeak
  ThingSpeak.setField(1, distance);
  int status = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (status == 200) {
    Serial.println("Data sent to ThingSpeak successfully.");
  } else {
    Serial.print("ThingSpeak error code: ");
    Serial.println(status);
  }

  delay(30000); // wait 30 seconds
}
