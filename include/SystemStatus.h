#pragma once

#include <models/UsageStats.h>

class SystemStatus {
public:
    static UsageStats getStorageStatus();
    static UsageStats getMemoryStatus();
};
