#ifndef WIFINETWORK_H
#define WIFINETWORK_H

#include <Arduino.h>

class WifiNetwork {
  public:
    WifiNetwork(String name, int channel, String bssid, String encryptation, int rssi);
    void printInfo();
    String getName() const;
    int getChannel() const;
    String getBssid() const;
    String getEncryptation() const;
    int getRssi() const;

  private:
    String name;
    int channel;
    String bssid;
    String encryptation;
    int rssi; // Signal strength (RSSI)
};

#endif  // WIFINETWORK_H
