#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =====================
// PIN DEFINITIONS
// =====================
#define WATER_SENSOR_PIN 33
#define GREEN_LED 25
#define RED_LED 26

#define OLED_SDA 21
#define OLED_SCL 22

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// =====================
// OLED
// =====================
Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

// =====================
// WIFI
// =====================
const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

// =====================
// THINGSPEAK
// =====================
const char* THINGSPEAK_API_KEY = "8UCFHM7M7C7HTXUC";

const char* THINGSPEAK_URL =
  "https://api.thingspeak.com/update";

// =====================
// WEB SERVER
// =====================
WebServer server(80);

// =====================
// AWD THRESHOLD
// =====================
#define AWD_THRESHOLD_CM -20.0

// =====================
// VARIABLES
// =====================
int sensorValue = 0;

float waterLevel = 0;
float waterDepth = 0;

bool thresholdReached = false;
bool sensorValid = true;
bool previousThresholdReached = false;
unsigned long awdEventCount = 0;
// ThingSpeak timing
unsigned long lastThingSpeakUpdate = 0;
const unsigned long THINGSPEAK_INTERVAL = 15000;

// =====================
// WEB PAGE
// =====================
void handleRoot() {

  String html = "";

  html += "<!DOCTYPE html>";
  html += "<html>";
  html += "<head>";

  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";

  html += "<meta http-equiv='refresh' content='2'>";

  html += "<title>Water Level System</title>";

  html += "<style>";

  html += "body{font-family:Arial;text-align:center;background:#111;color:white;}";

  html += ".container{max-width:500px;margin:auto;padding:20px;}";

  html += ".card{background:#222;padding:25px;border-radius:15px;margin-top:20px;}";

  html += ".level{font-size:50px;font-weight:bold;}";

  html += ".safe{color:#00ff66;}";

  html += ".danger{color:#ff3333;}";

  html += ".bar{width:100%;height:30px;background:#444;border-radius:15px;overflow:hidden;}";

  html += ".fill{height:100%;background:#00aaff;}";

  html += "</style>";

  html += "</head>";

  html += "<body>";

  html += "<div class='container'>";

  html += "<h1>WATER LEVEL SYSTEM</h1>";

  html += "<div class='card'>";

  html += "<h2>Water Level</h2>";

  html += "<div class='level'>";

  html += String(waterLevel, 1);

  html += "%</div>";

  html += "<div class='bar'>";

  html += "<div class='fill' style='width:";

  html += String(waterLevel, 1);

  html += "%'></div>";

  html += "</div>";

  html += "<h2>Depth: ";

  html += String(waterDepth, 1);

  html += " cm</h2>";

  if (thresholdReached) {

    html += "<h2 class='danger'>AWD THRESHOLD REACHED</h2>";

    html += "<p>RED LED: ON</p>";

    html += "<p>GREEN LED: OFF</p>";

  } else {

    html += "<h2 class='safe'>SAFE</h2>";

    html += "<p>GREEN LED: ON</p>";

    html += "<p>RED LED: OFF</p>";
  }

  html += "<p>Sensor Value: ";

  html += String(sensorValue);

  html += "</p>";

  html += "</div>";

  html += "</div>";

  html += "</body>";

  html += "</html>";

  server.send(200, "text/html", html);
}

// =====================
// SEND DATA TO THINGSPEAK
// =====================
void sendToThingSpeak() {

  if (WiFi.status() != WL_CONNECTED) {

    Serial.println("WiFi disconnected!");

    return;
  }

  WiFiClientSecure client;

  // Wokwi simulation:
  client.setInsecure();

  HTTPClient http;

  String url = String(THINGSPEAK_URL);

  url += "?api_key=";
  url += THINGSPEAK_API_KEY;

  // Field 1 = Water Level
  url += "&field1=";
  url += String(waterLevel, 1);

  // Field 2 = Water Depth
  url += "&field2=";
  url += String(waterDepth, 1);

  // Field 3 = Sensor Value
  url += "&field3=";
  url += String(sensorValue);

  // Field 4 = AWD Status
  // 1 = AWD
  // 0 = SAFE
  url += "&field4=";
  url += String(thresholdReached ? 1 : 0);
// field 5 = AWD Event Count
  url += "&field5=";
url += String(awdEventCount);

  Serial.println("Sending data to ThingSpeak...");

  if (http.begin(client, url)) {

    int httpCode = http.GET();

    Serial.print("ThingSpeak HTTP code: ");
    Serial.println(httpCode);

    if (httpCode > 0) {

      String response = http.getString();

      Serial.print("ThingSpeak response: ");
      Serial.println(response);

      if (response != "0") {

        Serial.println("ThingSpeak update successful!");

      } else {

        Serial.println("ThingSpeak update failed!");
      }

    } else {

      Serial.print("HTTP request failed: ");
      Serial.println(http.errorToString(httpCode));
    }

    http.end();

  } else {

    Serial.println("Unable to connect to ThingSpeak");
  }
}
int readStableSensor() {
  long total = 0;

  for (int i = 0; i < 10; i++) {
    total += analogRead(WATER_SENSOR_PIN);
    delay(5);
  }

  return total / 10;
}

bool isValidSensorReading(int value) {
  return (value >= 0 && value <= 4095);
}
// =====================
// SETUP
// =====================
void setup() {

  Serial.begin(115200);

  // LEDs
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  // OLED
  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C)) {

    Serial.println("OLED initialization failed!");

    while (true) {
      delay(1000);
    }
  }

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);

  display.setCursor(10, 10);

  display.println("WATER LEVEL SYSTEM");

  display.setCursor(20, 30);

  display.println("Connecting WiFi...");

  display.display();

  // =====================
  // WIFI CONNECTION
  // =====================

  Serial.println("Connecting to WiFi...");

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();

  Serial.println("WiFi connected!");

  Serial.print("ESP32 IP address: ");

  Serial.println(WiFi.localIP());

  // =====================
  // WEB SERVER
  // =====================

  server.on("/", handleRoot);

  server.begin();

  Serial.println("Web server started!");

  // OLED ONLINE
  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(15, 15);

  display.println("WATER LEVEL SYSTEM");

  display.setCursor(30, 35);

  display.println("ONLINE");

  display.display();

  delay(2000);
}

// =====================
// LOOP
// =====================
void loop() {

  // =====================
  // READ SENSOR
  // =====================

  sensorValue = readStableSensor();


// Validate sensor reading
sensorValid = (sensorValue >= 0 && sensorValue <= 4095);

if (!sensorValid) {
  Serial.println("ERROR: Invalid sensor reading!");
  delay(1000);
  return;
}

  // Convert ADC to percentage

  waterLevel =
    (sensorValue / 4095.0) * 100.0;

  waterLevel =
    constrain(
      waterLevel,
      0.0,
      100.0
    );

  // Convert percentage to depth

  waterDepth =
    -(waterLevel / 100.0) * 30.0;

  // AWD threshold

  thresholdReached =
    (waterDepth <= AWD_THRESHOLD_CM);

  if (thresholdReached && !previousThresholdReached) {
  awdEventCount++;

  Serial.print("AWD EVENT: Threshold reached! Event count = ");
  Serial.println(awdEventCount);
}

if (!thresholdReached && previousThresholdReached) {
    Serial.println("AWD EVENT: Water level returned to SAFE.");
}

previousThresholdReached = thresholdReached;

  // =====================
  // LED CONTROL
  // =====================

  if (thresholdReached) {

    digitalWrite(RED_LED, HIGH);

    digitalWrite(GREEN_LED, LOW);

  } else {

    digitalWrite(RED_LED, LOW);

    digitalWrite(GREEN_LED, HIGH);
  }

  // =====================
  // SERIAL MONITOR
  // =====================

  Serial.print("Sensor value: ");

  Serial.print(sensorValue);

  Serial.print(" | Water level: ");

  Serial.print(waterLevel, 1);

  Serial.print("% | Water depth: ");

  Serial.print(waterDepth, 1);

  Serial.println(" cm");

  // =====================
  // OLED DISPLAY
  // =====================

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);

  display.setCursor(5, 2);

  display.println("WATER LEVEL SYSTEM");

  display.setTextSize(2);

  display.setCursor(5, 15);

  display.print((int)waterLevel);

  display.println("%");

  // Progress bar

  display.drawRect(
    5,
    35,
    118,
    8,
    SSD1306_WHITE
  );

  int barWidth =
    (int)((waterLevel / 100.0) * 118);

  barWidth =
    constrain(
      barWidth,
      0,
      118
    );

  if (barWidth > 0) {

    display.fillRect(
      5,
      35,
      barWidth,
      8,
      SSD1306_WHITE
    );
  }

  display.setTextSize(1);

  display.setCursor(5, 47);

  display.print("Depth: ");

  display.print(waterDepth, 1);

  display.println(" cm");

  display.setCursor(5, 56);

  if (thresholdReached) {

    display.print("STATUS: AWD!");

  } else {

    display.print("STATUS: SAFE");
  }

  display.display();

  // =====================
  // LOCAL WEB SERVER
  // =====================

  server.handleClient();

  // =====================
  // THINGSPEAK UPDATE
  // =====================

  if (millis() - lastThingSpeakUpdate >=
      THINGSPEAK_INTERVAL) {

    sendToThingSpeak();

    lastThingSpeakUpdate = millis();
  }

  delay(100);
}