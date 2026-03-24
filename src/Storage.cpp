#include "Storage.h"
#include "MenuItem.h"
#include "SPIFFS.h"

#define FORMAT_SPIFFS_IF_FAILED true
#define FILEBUFSIZ 4096

void Storage::init() {
    if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
        Serial.println("SPIFFS Mount Failed");
    }
}

/**
 * @brief Lists the files inside a directory in SPIFFS
 * 
 * @param folder Directory path to filter
 * @return std::vector<String> List of file paths found
 */
std::vector<String> Storage::listDir(String folder) {
    File root = SPIFFS.open("/");
    std::vector<String> files;

    if (!root) {
        Serial.println("Error getting the list of files");
        return files;
    }

    File file = root.openNextFile();

    while (file) {
      String filePath = String(file.path());

      if(filePath.startsWith(folder) && String(file.name()) != ".keep") {
        files.push_back(String(file.path()));
      }

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