#include "Storage.h"
#include "Bluetooth.h"
#include "WifiManager.h"
#include "ServerManager.h"
ServerManager serverManager;

void setup() {
  Serial.begin(115200);

  Serial.println("Welcome to ESP32-Toolkit");
  Storage::init();
  WifiManager::init();

  delay(100);

  serverManager.begin();
}

void loop() {
  if (Bluetooth::isRunning()) {
    Bluetooth::spamBLE();
  }

  if (WifiManager::isRunningBeaconSpam()) {
    WifiManager::beaconSpam();
  }

  serverManager.readAsyncReport();
  
  delay(100);
}
