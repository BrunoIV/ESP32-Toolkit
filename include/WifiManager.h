#pragma once
#include <Arduino.h>

class WifiManager {
    public:
        static void connect(String ssid, String password);
        static void init();

    private:
        
};
