#pragma once
#include <Arduino.h>
#include <vector>
#include "WifiNetwork.h"

class WifiManager {
    public:
        static void connect(String ssid, String password);
        static void init();
        static std::vector<WifiNetwork> getNetworks();

    private:
        
};
