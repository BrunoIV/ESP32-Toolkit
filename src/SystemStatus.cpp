#include "SystemStatus.h"
#include "SPIFFS.h"

UsageStats SystemStatus::getStorageStatus() {
    return UsageStats(SPIFFS.totalBytes(), SPIFFS.usedBytes());
}

UsageStats SystemStatus::getMemoryStatus() {
    size_t total = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
    size_t free = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    size_t used = total - free;

    return UsageStats(total, used);
}