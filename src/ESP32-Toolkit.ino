#include "Storage.h"
#include "Bluetooth.h"
#include "WifiManager.h"
#include "ServerManager.h"

ServerManager server;

void setup() {
  Serial.begin(115200);

  Serial.println("Welcome to ESP32-Toolkit");
  Storage::init();
  WifiManager::init();

  delay(100);

  server.begin();
}

void loop() {
  server.handleClient();

  if (Bluetooth::isRunning()) {
    Bluetooth::spamBLE();
  }
  
  delay(100);
}
