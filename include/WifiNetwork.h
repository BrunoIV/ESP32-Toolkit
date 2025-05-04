#ifndef WIFINETWORK_H
#define WIFINETWORK_H

#include <Arduino.h>

class WifiNetwork {
  public:
    WifiNetwork(String name, int channel, const uint8_t* bssid_ptr, String encryptation, int rssi);

    String toString();
    String getName() const;
    int getChannel() const;
    const uint8_t* getBssid() const;
    String getEncryptation() const;
    int getRssi() const;
    String getBssidAsString();

  private:
    String name;
    int channel;
    uint8_t bssid[6];
    String encryptation;
    int rssi;
};

#endif  // WIFINETWORK_H
