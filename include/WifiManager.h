#pragma once
#include <Arduino.h>
#include <vector>
#include "WifiNetwork.h"
const uint8_t MAX_NETWORKS_BEACON_SPAM = 10;

class WifiManager {
    public:
        static void connect(String ssid, String password);
        static void init();
        static void create(String uuid, String password);
        static std::vector<WifiNetwork> getNetworks();
        static void beaconSpam();
        static void runBeaconSpam();
        static void stopBeaconSpam();
        static bool isRunningBeaconSpam();

    private:
        static void sendBeacon(const String ssid);
        static bool run;

};
