#include "WifiManager.h"
#include <WiFi.h>
#include <vector>
#include "secrets.h"
#include "WifiNetwork.h"

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


std::vector<WifiNetwork> WifiManager::getNetworks() {
  std::vector<WifiNetwork> networks;

  Serial.println("Scanning networks...");
  delay(100);

  int size = WiFi.scanNetworks();
  const String encriptationsName[10] = { "OPEN", "WEP", "WPA PSK", "WPA2 PSK", "WPA WPA2 PSK", "WPA2 ENTERPRISE", "WPA3 PSK", "WPA2 WPA3 PSK", "WAPI_PSK", "MAX" };
  for (int i = 0; i < size; ++i) {
    networks.push_back(WifiNetwork(WiFi.SSID(i), WiFi.channel(i), WiFi.BSSID(i), encriptationsName[WiFi.encryptionType(i)], WiFi.RSSI(i)));
  }

  return networks;
}