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

std::vector<MenuItem> Storage::listDir(String folder) {
    File root = SPIFFS.open("/");
    std::vector<MenuItem> files;

    if (!root) {
        Serial.println("Error getting the list of files");
        return files;
    }

    File file = root.openNextFile();

    while (file) {
      String filePath = String(file.path());

      if(filePath.startsWith(folder)) {

        String filePathWithoutFolder = filePath.substring(folder.length());
        bool isFile = filePathWithoutFolder.indexOf("/") <= 0;
        filePathWithoutFolder = filePathWithoutFolder.substring(0, filePathWithoutFolder.indexOf("/"));

        if(isFile) {
          MenuItem menu = MenuItem("", "/edit?file=" + String(file.path()), file.name(), "fa fa-file", std::vector<MenuItem>());
          files.push_back(menu);
        } else {

          bool exists = false;
          for (const MenuItem& item : files) {
              if (item.getName() == filePathWithoutFolder) {
                  exists = true;
                  break;
              }
          }

          if(!exists) {
            MenuItem menu = MenuItem("", "/files?folder=" + folder + filePathWithoutFolder + "/", filePathWithoutFolder, "fa fa-folder", std::vector<MenuItem>());
            files.push_back(menu);
          }
       
        }
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