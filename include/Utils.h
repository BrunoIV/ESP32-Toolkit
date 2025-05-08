#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <vector>

class Utils {
public:
    static std::vector<String> splitString(const String &str, char delimiter);
};

#endif
