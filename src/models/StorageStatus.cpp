#include "models/StorageStatus.h"

StorageStatus::StorageStatus(size_t total, size_t used)
: total(total), used(used) {
}

size_t StorageStatus::getTotal() const { return total; }
size_t StorageStatus::getUsed() const { return used; }
size_t StorageStatus::getFree() const { return total - used; }
int8_t StorageStatus::getPercentUsed() const { return ceil((used * 100.0) / total); }

String StorageStatus::toString() const {
    return "Total: " + String(total) +
    ", Used: " + String(used) +
    ", Free: " + String(getFree()) +
    ", Percent: " + String(getPercentUsed()) + "%";
}
