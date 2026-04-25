/*

Example of BH1750 library usage.

This example initialises the BH1750 object using the default high resolution
continuous mode and then makes a light level reading every second.

Connections

  - VCC to 3V3 or 5V
  - GND to GND
  - SCL to SCL (A5 on Arduino Uno, Leonardo, etc or 21 on Mega and Due, on
    esp8266 free selectable)
  - SDA to SDA (A4 on Arduino Uno, Leonardo, etc or 20 on Mega and Due, on
    esp8266 free selectable)
  - ADD to (not connected) or GND

ADD pin is used to set sensor I2C address. If it has voltage greater or equal
to 0.7VCC voltage (e.g. you've connected it to VCC) the sensor address will be
0x5C. In other case (if ADD voltage less than 0.7 * VCC) the sensor address
will be 0x23 (by default).

*/
#include <WiFiNINA.h>
#include <Wire.h>
#include <BH1750.h>

// ── WiFi ──────────────────────────────────────────────────────────────────────
char ssid[] = "NOKIA-1FF1";
char pass[] = "k6uL2LaeMw";

// ── IFTTT ─────────────────────────────────────────────────────────────────────
const char* IFTTT_HOST = "maker.ifttt.com";
const int   IFTTT_PORT = 443;
const char* IFTTT_KEY  = "bl2SWLMVGPSPoulJ_wCRF7";

String sunlightOnEvent  = "sunlight_on";
String sunlightOffEvent = "sunlight_off";

// ── Light Threshold ───────────────────────────────────────────────────────────
// Watch Serial Monitor to see your lux readings, then set this value
// between your dark reading and your bright reading.
float lightThreshold = 300.0;  // lux

// ── Globals ───────────────────────────────────────────────────────────────────
WiFiSSLClient client;   // must be SSL for HTTPS on port 443
BH1750 lightMeter;
bool sunlightDetected = false;

// ─────────────────────────────────────────────────────────────────────────────

void connectWiFi() {
  Serial.print("Connecting to WiFi");
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(2000);
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// Sends a GET request to IFTTT Webhooks with value1 as a query parameter.
// URL format: /trigger/{event}/with/key/{key}?value1={value1}
// value1 will appear in your email body as {{Value1}}
void sendIFTTTEvent(String eventName, String value1) {
  Serial.println("Sending IFTTT event: " + eventName);

  if (!client.connect(IFTTT_HOST, IFTTT_PORT)) {
    Serial.println("Connection to IFTTT failed");
    return;
  }

  // Build the URL with value1 as query parameter
  String url = "/trigger/" + eventName + "/with/key/" + String(IFTTT_KEY)
             + "?value1=" + value1;

  // Send HTTP GET request
  client.println("GET " + url + " HTTP/1.1");
  client.println("Host: " + String(IFTTT_HOST));
  client.println("Connection: close");
  client.println();

  // Print full server response
  // "Congratulations! You've fired the sunlight_on event" = success
  unsigned long t = millis();
  while ((client.connected() || client.available()) && millis() - t < 5000) {
    if (client.available()) Serial.print((char)client.read());
  }

  client.stop();
  Serial.println("\nDone.");
}

// ─────────────────────────────────────────────────────────────────────────────

void setup() {
  Serial.begin(9600);
  delay(2000);

  Wire.begin();
  lightMeter.begin();  // không cần kiểm tra bool
  Serial.println("BH1750 ready");

  connectWiFi();
  Serial.println("Monitoring started...");
}

void loop() {
  float lux = lightMeter.readLightLevel();  // dùng hàm đúng

  Serial.print("Light: ");
  Serial.print(lux);
  Serial.print(" lx  |  ");
  Serial.println(sunlightDetected ? "SUNLIGHT ON" : "sunlight off");

  if (lux > lightThreshold && !sunlightDetected) {
    sunlightDetected = true;
    sendIFTTTEvent(sunlightOnEvent, "Lux=" + String(lux));
  }
  else if (lux <= lightThreshold && sunlightDetected) {
    sunlightDetected = false;
    sendIFTTTEvent(sunlightOffEvent, "Lux=" + String(lux));
  }

  delay(3000);
}
