#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include <vector>

class Storage {
public:
    static void init();
    static std::vector<String> listDir();
    static void writeFile(String path, String content);
    static boolean deleteFile(String path);
    static String readFile(String path);
};

#endif
