#pragma once
#include <Arduino.h>

class UsageStats {
  public:
    UsageStats(size_t total, size_t used);
    String toString() const;
    size_t getTotal() const;
    size_t getUsed() const;
    size_t getFree() const;
    size_t getTotalAsKb() const;
    size_t getUsedAsKb() const;
    size_t getFreeAsKb() const;
    int8_t getPercentUsed() const;

  private:
    size_t total;
    size_t used;
};
