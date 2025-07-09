#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

const char* ssid = "TP-LINK_050102";
const char* password = "13842005m";

// 🔄 لینک فایل .bin فریمور که روی سرور آپلود کردی
String firmware_url = "http://github.com/HeliaTJB/OTA.git";  // <-- اینو تغییر بده
WiFiClient client;


const int ESP_BUILTIN_LED = 2;
unsigned long lastUpdateCheck = 0;
const unsigned long updateInterval = 1 * 60 * 1000; // هر 5 دقیقه


void checkForUpdates() {
  Serial.println("🔄 Checking for firmware update...");
 t_httpUpdate_return ret = ESPhttpUpdate.update(client, firmware_url);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("❌ Update failed. Error (%d): %s\n", 
                    ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("ℹ️ No updates available.");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("✅ Update successful.");
      break;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.setPort(8266);
  ArduinoOTA.setHostname("myesp8266");
  ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(BUILTIN_LED, OUTPUT);
}

void loop() {
  ArduinoOTA.handle();



  // Check for update every X milliseconds
  if (millis() - lastUpdateCheck > updateInterval) {
    lastUpdateCheck = millis();
    checkForUpdates();
  }
}
