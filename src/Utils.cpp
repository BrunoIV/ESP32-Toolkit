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