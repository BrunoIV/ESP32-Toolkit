#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <vector>

class Utils {
public:
    static std::vector<String> splitString(const String &str, char delimiter);
    static String escapeHTML(String input);
    static String randomString(int length);
    static bool isValidIPv4(const String& ip);
    static uint32_t ipToInt(int a, int b, int c, int d);
    static void intToIp(uint32_t ip, int &a, int &b, int &c, int &d);
};

#endif
