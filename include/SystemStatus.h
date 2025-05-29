#pragma once

#include <models/UsageStats.h>

class SystemStatus {
public:
    static UsageStats getStorageStatus();
    static UsageStats getMemoryStatus();
    static void restart();
    static String uptime();
};
