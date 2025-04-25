#include "WifiNetwork.h"


WifiNetwork::WifiNetwork(String name, int channel, String bssid, String encryptation, int rssi)
  : name(name), channel(channel), bssid(bssid), encryptation(encryptation), rssi(rssi) {
}

String WifiNetwork::getName() const { return name; }
int WifiNetwork::getChannel() const { return channel; }
String WifiNetwork::getBssid() const { return bssid; }
String WifiNetwork::getEncryptation() const { return encryptation; }
int WifiNetwork::getRssi() const { return rssi; }

void WifiNetwork::printInfo() {
  Serial.print("SSID: ");
  Serial.print(name);
  Serial.print(" || Channel: ");
  Serial.print(channel);
  Serial.print(" || BSSID: ");
  Serial.print(bssid);
  Serial.print(" || Encryption: ");
  Serial.print(encryptation);
  Serial.print(" || RSSI: ");
  Serial.println(rssi);
}