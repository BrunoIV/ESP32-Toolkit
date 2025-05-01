#include "WifiNetwork.h"


WifiNetwork::WifiNetwork(String name, int channel, uint8_t* bssid, String encryptation, int rssi)
  : name(name), channel(channel), bssid(bssid), encryptation(encryptation), rssi(rssi) {
}

String WifiNetwork::getName() const { return name; }
int WifiNetwork::getChannel() const { return channel; }
uint8_t* WifiNetwork::getBssid() const { return bssid; }
String WifiNetwork::getEncryptation() const { return encryptation; }
int WifiNetwork::getRssi() const { return rssi; }

void WifiNetwork::printInfo() {
  Serial.print("SSID: ");
  Serial.print(name);
  Serial.print(" || Channel: ");
  Serial.print(channel);
  Serial.print(" || BSSID: ");
  Serial.print(getBssidAsString());
  Serial.print(" || Encryption: ");
  Serial.print(encryptation);
  Serial.print(" || RSSI: ");
  Serial.println(rssi);
}

String WifiNetwork::getBssidAsString() {
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
