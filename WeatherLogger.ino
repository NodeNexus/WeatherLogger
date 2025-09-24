#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <SPIFFS.h>

#define SEALEVELPRESSURE_HPA (1013.25)

const char* ssid = "ESP32-AP";
const char* password = "12345678";

Adafruit_BME280 bme; // I2C
AsyncWebServer server(80);

struct WeatherData {
  float temperature;
  float humidity;
  float pressure;
};

WeatherData latestData;

void setup() {
  Serial.begin(115200);

  if(!SPIFFS.begin(true)){
    Serial.println("SPIFFS Mount Failed");
  }

  WiFi.softAP(ssid, password);
  Serial.println("Access Point Started");
  Serial.println(WiFi.softAPIP());

  if (!bme.begin(0x76)) {
    Serial.println("Could not find BME280 sensor!");
    while (1);
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", webpage);
  });

  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    latestData.temperature = bme.readTemperature();
    latestData.humidity = bme.readHumidity();
    latestData.pressure = bme.readPressure() / 100.0F;

    // Save to CSV
    File file = SPIFFS.open("/data.csv", FILE_APPEND);
    if(file){
      file.printf("%.2f,%.2f,%.2f\n", latestData.temperature, latestData.humidity, latestData.pressure);
      file.close();
    }

    StaticJsonDocument<200> doc;
    doc["temperature"] = latestData.temperature;
    doc["humidity"] = latestData.humidity;
    doc["pressure"] = latestData.pressure;

    String json;
    serializeJson(doc, json);
    request->send(200, "application/json", json);
  });

  server.on("/download", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/data.csv", "text/csv");
  });

  server.begin();
}

void loop() {
  // Async server handles all
}

const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Local Weather Logger</title>
<style>
body { font-family: Arial; text-align: center; background-color: #111; color: #0ff; }
h1 { margin-bottom: 20px; }
div.data { font-size: 24px; margin: 10px; }
button { font-size: 18px; margin: 5px; padding: 8px 16px; }
</style>
</head>
<body>
<h1>Weather Logger Dashboard</h1>
<div class="data">Temperature: <span id="temp">0</span> °C</div>
<div class="data">Humidity: <span id="hum">0</span> %</div>
<div class="data">Pressure: <span id="pressure">0</span> hPa</div>
<button onclick="downloadCSV()">Download CSV</button>

<script>
function fetchData() {
  fetch('/data')
  .then(resp => resp.json())
  .then(data => {
    document.getElementById('temp').innerText = data.temperature.toFixed(1);
    document.getElementById('hum').innerText = data.humidity.toFixed(1);
    document.getElementById('pressure').innerText = data.pressure.toFixed(1);
  });
}

function downloadCSV() {
  window.location.href = '/download';
}

setInterval(fetchData, 1000);
fetchData();
</script>
</body>
</html>
)rawliteral";
