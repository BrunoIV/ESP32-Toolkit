#include <WiFi.h>
#include <vector>

#include "WifiNetwork.h"


std::vector<WifiNetwork> networks;

void setup() {
  Serial.begin(115200);
  getNetworks();
}


void getNetworks() {
  Serial.println("Scanning networks...");
  WiFi.mode(WIFI_STA);
  delay(100);

  int size = WiFi.scanNetworks();
  const String encriptationsName[10] = { "OPEN", "WEP", "WPA PSK", "WPA2 PSK", "WPA WPA2 PSK", "WPA2 ENTERPRISE", "WPA3 PSK", "WPA2 WPA3 PSK", "WAPI_PSK", "MAX" };
  for (int i = 0; i < size; ++i) {
    String name = WiFi.SSID(i);
    networks.push_back(WifiNetwork(name, WiFi.channel(i), getBSSIDString(i), encriptationsName[WiFi.encryptionType(i)], WiFi.RSSI(i)));
    networks[i].printInfo();
  }
}

String getBSSIDString(int index) {
  uint8_t* bssid = WiFi.BSSID(index);  //BSSID as an array of bytes
  String bssidStr = "";
  
  //Convert to format "XX:XX:XX:XX:XX:XX"
  for (int i = 0; i < 6; i++) {
    if (bssid[i] < 0x10) {
      bssidStr += "0";  // Adds a zero to the left if byte < 16
    }
    
    bssidStr += String(bssid[i], HEX);
    if (i < 5) {
      bssidStr += ":";
    }
  }
  return bssidStr;
}

void loop() {
  delay(100);
}
