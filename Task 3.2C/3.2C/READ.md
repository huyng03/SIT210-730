#include <WiFiNINA.h>
#include <PubSubClient.h>

const char* ssid = "NOKIA-1FF1";
const char* password = "k6uL2LaeMw";
const char* broker = "broker.emqx.io";
const int port = 1883;


const int echoPin = 10;
const int trigPin = 9;
const int hall = 6;
const int wc = 7;

const int wave_max = 50;
const int pat_max = 20;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Message arrived ["); 
  Serial.print(topic); 
  Serial.print("]: "); 
  Serial.println(message);

  if (String(topic) == "ES/Wave") {
    digitalWrite(hall, HIGH); 
    digitalWrite(wc, HIGH);
  } 
  else if (String(topic) == "ES/Pat") {
    digitalWrite(hall, LOW); 
    digitalWrite(wc, LOW);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("Linda_House_System_01")) {
      Serial.println("connected");
      client.subscribe("ES/Wave"); 
      client.subscribe("ES/Pat");  
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

int getDistance() {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2; 
}

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(hall, OUTPUT);
  pinMode(wc, OUTPUT);

  Serial.println("Attempting to connect to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");

  client.setServer(broker, port); 
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int d = getDistance();

  if (d >= 5 && d <= 50) {
    if (d <= pat_max) {
      Serial.println("Action: Pat detected -> Sending OFF");
      client.publish("ES/Pat", "Linda"); 
    } else if (d <= wave_max) {
      Serial.println("Action: Wave detected -> Sending ON");
      client.publish("ES/Wave", "Linda"); 
    }
    delay(2000); 
  }
  
  delay(100); 
}
