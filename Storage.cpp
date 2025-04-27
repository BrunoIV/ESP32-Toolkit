#include "Storage.h"
#include "SPIFFS.h"

#define FORMAT_SPIFFS_IF_FAILED true
#define FILEBUFSIZ 4096

void Storage::init() {
    if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
        Serial.println("SPIFFS Mount Failed");
    }
}

std::vector<String> Storage::listDir() {
    File root = SPIFFS.open("/");
    std::vector<String> files;

    if (!root) {
        Serial.println("Error getting the list of files");
        return files;
    }

    File file = root.openNextFile();
    while (file) {
        files.push_back(String(file.path()));
        file = root.openNextFile();
    }

    return files;
}

void Storage::writeFile(String path, String content) {
    File file = SPIFFS.open(path.c_str(), FILE_WRITE);
    if (!file) {
        Serial.println("Error opening the file '" + path + "'");
        return;
    }

    if (!file.print(content.c_str())) {
        Serial.println("Error writing the file");
    }

    file.close();
}

boolean Storage::deleteFile(String path) {
  return SPIFFS.remove(path.c_str());
}

String Storage::readFile(String path) {
  File file = SPIFFS.open(path.c_str());
  if (!file || file.isDirectory()) {
    Serial.println("Error opening the file '" + path + "'");
    return "";
  }

  String output = "";
  while (file.available()) {
    char c = file.read();
    output += c;
  }
  file.close();
  return output;
}