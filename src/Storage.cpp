#include "Storage.h"
#include "models/MenuItem.h"
#include <LittleFS.h>

#define FORMAT_LittleFS_IF_FAILED true
#define FILEBUFSIZ 4096

void Storage::init() {
  if (!LittleFS.begin(true)) { //format if fails
    Serial.println("LittleFS Mount Failed");
  }
}

/**
 * @brief Lists the files inside a directory in LittleFS
 * 
 * @param folder Directory path to filter
 * @return std::vector<String> List of file paths found
 */
std::vector<String> Storage::listDir(String folder) {
  std::vector<String> files;

  File root = LittleFS.open(folder);
  if (!root || !root.isDirectory()) {
    Serial.println("Error opening directory");
    return files;
  }

  File file = root.openNextFile();

  while (file) {
    if (file.isDirectory()) {
      std::vector<String> subFiles = listDir(String(file.path()));
      files.insert(files.end(), subFiles.begin(), subFiles.end());
    } else {
      files.push_back(String(file.path()));
    }

    file = root.openNextFile();
  }

  return files;
}

boolean Storage::mkdir(String path) {
  return LittleFS.mkdir(path);
}


void Storage::writeFile(String path, String content) {
    File file = LittleFS.open(path.c_str(), FILE_WRITE);
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
  return LittleFS.remove(path.c_str());
}

String Storage::readFile(String path) {
  File file = LittleFS.open(path.c_str());
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