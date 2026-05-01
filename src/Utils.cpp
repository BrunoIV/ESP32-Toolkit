#include "Utils.h"


std::vector<String> Utils::splitString(const String &str, char delimiter) {
    std::vector<String> result;
    int wordStartIndex = 0;
  
    for (int i = 0; i <= str.length(); i++) {
      //delimiter found or end of string
      if (str.charAt(i) == delimiter || i == str.length()) {

        //From prev word to position of delimiter
        result.push_back(str.substring(wordStartIndex, i));
        wordStartIndex = i + 1;
      }
    }
  
    return result;
}

String Utils::escapeHTML(String input) {
  input.replace("&", "&amp;");
  input.replace("<", "&lt;");
  input.replace(">", "&gt;");
  input.replace("\"", "&quot;");
  input.replace("'", "&#39;");
  return input;
}

String Utils::randomString(int length) {
  String characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  String result = "";

  for (int i = 0; i < length; i++) {
    int index = random(characters.length());
    result += characters[index];
  }

  return result;
}

bool Utils::isValidIPv4(const String& ip) {
  int a, b, c, d;

  if (sscanf(ip.c_str(), "%d.%d.%d.%d", &a, &b, &c, &d) != 4)
    return false;

  return (a >= 0 && a <= 255 &&
          b >= 0 && b <= 255 &&
          c >= 0 && c <= 255 &&
          d >= 0 && d <= 255);
}

uint32_t Utils::ipToInt(int a, int b, int c, int d) {
  return ((uint32_t)a << 24) |
         ((uint32_t)b << 16) |
         ((uint32_t)c << 8)  |
         (uint32_t)d;
}

void Utils::intToIp(uint32_t ip, int &a, int &b, int &c, int &d) {
  a = (ip >> 24) & 0xFF;
  b = (ip >> 16) & 0xFF;
  c = (ip >> 8) & 0xFF;
  d = ip & 0xFF;
}