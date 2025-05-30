#include "WifiManager.h"
#include <WiFi.h>
#include <esp_wifi.h>
#include <vector>
#include "secrets.h"
#include "Utils.h"
#include "WifiNetwork.h"


bool WifiManager::run = false;


uint8_t beaconPacket[] = {
  0x80, 0x00, // Frame control (Beacon frame)
  0x00, 0x00, // Duration
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Dest: broadcast
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Source MAC (se llenará luego)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // BSSID MAC (se llenará luego)
  0x00, 0x00, // Seq
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x64, 0x00, // Interval
  0x01, 0x04  // Capabilities
};



void WifiManager::runBeaconSpam() {
  run = true;
}

void WifiManager::stopBeaconSpam() {
  run = false;
}

bool WifiManager::isRunningBeaconSpam() {
  return run;
}

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

void WifiManager::create(const String ssid, const String password) {
  WiFi.softAP(ssid, password);
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

void WifiManager::beaconSpam() {
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
  delay(100);

  for (int i = 0; i < MAX_NETWORKS_BEACON_SPAM; i++) {
    sendBeacon(Utils::randomString(20));
    delay(100);
  }
}

void WifiManager::sendBeacon(const String ssid) {
  uint8_t packet[200];
  int ssid_len = strlen(ssid.c_str());
  int packetLen = sizeof(beaconPacket) + 2 + ssid_len;

  memcpy(packet, beaconPacket, sizeof(beaconPacket));

  // Generate a random mac
  for (int i = 10; i < 16; i++) {
    uint8_t randByte = random(256);
    packet[i] = randByte;
    packet[i + 6] = randByte;
  }

  // Info of SSID
  packet[sizeof(beaconPacket)] = 0x00;
  packet[sizeof(beaconPacket) + 1] = ssid_len;
  memcpy(&packet[sizeof(beaconPacket) + 2], ssid.c_str(), ssid_len);

  // Send the fake beacon
  esp_wifi_80211_tx(WIFI_IF_AP, packet, packetLen, false);
}
