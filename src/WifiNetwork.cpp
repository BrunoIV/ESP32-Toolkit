#include "WifiNetwork.h"


WifiNetwork::WifiNetwork(String name, int channel, const uint8_t* bssid_ptr, String encryptation, int rssi)
: name(name), channel(channel), encryptation(encryptation), rssi(rssi) {
    memcpy(bssid, bssid_ptr, 6);
}

String WifiNetwork::getName() const { return name; }
int WifiNetwork::getChannel() const { return channel; }
const uint8_t* WifiNetwork::getBssid() const { return bssid; }
String WifiNetwork::getEncryptation() const { return encryptation; }
int WifiNetwork::getRssi() const { return rssi; }

String WifiNetwork::toString() {
  return "SSID: " + name + " || Channel: " + String(channel) + " || BSSID: " + getBssidAsString();
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
