#include "WifiManager.h"
#include <WiFi.h>
#include "secrets.h"

void WifiManager::init() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(WIFI_SSID, WIFI_PASS);
}


void WifiManager::connect(const String ssid, const String password) {
  Serial.print("Connecting to " + ssid);
  WiFi.begin(ssid.c_str(), password.c_str());

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  
  Serial.println("\nConnection success");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  delay(200);
}