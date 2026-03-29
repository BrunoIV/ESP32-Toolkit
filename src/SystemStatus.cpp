#include "SystemStatus.h"
#include <LittleFS.h>

UsageStats SystemStatus::getStorageStatus() {
    return UsageStats(LittleFS.totalBytes(), LittleFS.usedBytes());
}

UsageStats SystemStatus::getMemoryStatus() {
    size_t total = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
    size_t free = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    size_t used = total - free;

    return UsageStats(total, used);
}

void SystemStatus::restart() {
  ESP.restart();
}

String SystemStatus::uptime() {
  unsigned long currentMillis = millis();
  unsigned long seconds = currentMillis / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  unsigned long days = hours / 24;
  currentMillis %= 1000;
  seconds %= 60;
  minutes %= 60;
  hours %= 24;

  return String(hours < 10 ? "0" : "") + String(hours) + ":" + String(minutes < 10 ? "0" : "") + String(minutes) + ":" + String(seconds < 10 ? "0" : "") + String(seconds);
}