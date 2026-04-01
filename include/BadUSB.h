#ifndef BADUSB_H
#define BADUSB_H

#include <Arduino.h>
#include <vector>
#include "models/MenuItem.h"

class BadUSB {
public:
    static std::vector<String> list();
    static void run(String file);

private:
    static void parseLine(String lineCode);
};

#endif
