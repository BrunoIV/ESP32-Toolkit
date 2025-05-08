#include "BadUSB.h"
#include <Arduino.h>
#include "Storage.h"
#include "Utils.h"
#include "USB.h"
#include "USBHIDKeyboard.h"
#include "USBHIDMouse.h"
#include "MenuItem.h"
#include <map>


USBHIDKeyboard Keyboard;
USBHIDMouse Mouse;

std::vector<MenuItem> BadUSB::list() {
    return Storage::listDir("/bad_usb/payloads", "/badUsbPayload");
}

void BadUSB::run(String file) {
    USB.begin();
    Keyboard.begin();
    Mouse.begin();
    delay(100);

    String text = Storage::readFile(file);

    std::vector<String> parts = Utils::splitString(text, '\n');
    for (int i = 0; i < parts.size(); i++) {
        parseLine(parts[i]);
    }
}

void BadUSB::parseLine(String line) {
    Serial.println(line);

    //Remove trailing/duplicated spaces
    line.trim();

    if (line.startsWith("REM ")) {
        return;
    } else if (line.startsWith("DELAY ")) {
        int delayTime = line.substring(6).toInt();
        delay(delayTime);
    } else if (line.startsWith("STRING ")) {
        Keyboard.releaseAll();
        String text = line.substring(7);
        Keyboard.print(text);
    } else {
        std::map<String, int> keys;
        keys["ENTER"] = KEY_RETURN;
        keys["CONTROL"] = KEY_LEFT_CTRL;
        keys["ALT"] = KEY_LEFT_ALT;
        keys["DELETE"] = KEY_DELETE;
        keys["GUI"] = KEY_LEFT_GUI;


        std::vector<String> parts = Utils::splitString(line, ' ');
        for (int i = 0; i < parts.size(); i++) {

            if (keys.count(parts[i]) > 0) {
                Keyboard.press(keys[parts[i]]);
            } else if(parts[i].length() == 1) {
                Keyboard.press(parts[i][0]);
            }
            delay(100);
        }
    } 
    Keyboard.releaseAll();

}
