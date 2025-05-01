#ifndef WIFINETWORK_H
#define WIFINETWORK_H

#include <Arduino.h>

class WifiNetwork {
  public:
    WifiNetwork(String name, int channel, uint8_t* bssid, String encryptation, int rssi);
    void printInfo();
    String getName() const;
    int getChannel() const;
    uint8_t* getBssid() const;
    String getEncryptation() const;
    int getRssi() const;
    String getBssidAsString();

  private:
    String name;
    int channel;
    uint8_t* bssid;
    String encryptation;
    int rssi; // Signal strength (RSSI)
};

#endif  // WIFINETWORK_H
