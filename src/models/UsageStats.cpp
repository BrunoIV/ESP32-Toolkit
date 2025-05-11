#include "models/UsageStats.h"

UsageStats::UsageStats(size_t total, size_t used)
: total(total), used(used) {
}

size_t UsageStats::getTotal() const { return total; }
size_t UsageStats::getUsed() const { return used; }
size_t UsageStats::getFree() const { return total - used; }
size_t UsageStats::getTotalAsKb() const { return total / 1024.0; }
size_t UsageStats::getUsedAsKb() const { return used / 1024.0; }
size_t UsageStats::getFreeAsKb() const { return getFree() / 1024.0; }
int8_t UsageStats::getPercentUsed() const { return ceil((used * 100.0) / total); }

String UsageStats::toString() const {
    return "Total: " + String(total) +
    ", Used: " + String(used) +
    ", Free: " + String(getFree()) +
    ", Percent: " + String(getPercentUsed()) + "%";
}
