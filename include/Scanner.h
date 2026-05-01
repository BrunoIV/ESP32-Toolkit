#ifndef SCANNER_H
#define SCANNER_H

#include <Arduino.h>
#include <vector>
#include <functional>
#include "models/MenuItem.h"

class Scanner {
public:
    typedef std::function<void(const String&)> ScanCallback;
    static void scanCommonPorts(ScanCallback);
    static void startScanPort(String ip);
    static void startScanIp(String ipStart, String ipEnd);
};

#endif
