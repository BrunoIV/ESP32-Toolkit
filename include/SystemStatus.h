#pragma once
#include "models/UsageStats.h"
#include "models/NetworkInfo.h"

class SystemStatus {
public:
    static UsageStats getStorageStatus();
    static UsageStats getMemoryStatus();
    static void restart();
    static NetworkInfo getNetworkInfo();
    static String uptime();
};
