#pragma once
#include <Arduino.h>
#include <vector>
#include "WifiNetwork.h"
#include "models/NetworkInfo.h"
#include "models/UsageStats.h"
#include "models/MenuItem.h"

class TemplateManager {
public:
    static String getMainTemplate(const String& title, const String& menu);
    static String getFiles(const std::vector<String>& files);
    static String getBadUsbPayloads(const std::vector<String>& files);
    static String getNetworks(const std::vector<WifiNetwork>& networks);
    static String getSystemStatus(UsageStats& storageStatus, UsageStats& memoryStatus, NetworkInfo networkInfo);
    static String getTextEditor(String path, String content);
    static String getCreateNetwork();
    static String getBluetoothList();
    static String getIndex(const std::vector<MenuItem>& menuItems);
};
