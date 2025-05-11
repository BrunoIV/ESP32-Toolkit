#pragma once
#include <Arduino.h>

class StorageStatus {
  public:
    StorageStatus(size_t total, size_t used);
    String toString() const;
    size_t getTotal() const;
    size_t getUsed() const;
    size_t getFree() const;
    int8_t getPercentUsed() const;

  private:
    size_t total;
    size_t used;
};
