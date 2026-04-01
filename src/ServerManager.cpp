#include "ServerManager.h"
#include "Storage.h"
#include <WebServer.h>
#include <WifiManager.h>
#include <WebServer.h>
#include "secrets.h"
#include "WifiNetwork.h"
#include "BadUSB.h"
#include "Deauther.h"
#include "Bluetooth.h"
#include "Utils.h"
#include "SystemStatus.h"
#include <map>
#include <TemplateManager.h>

//#include <ESPAsyncWebServer.h>

ServerManager::ServerManager() {
  server.on("/", [this]() { handleRoot(); });

  //Files
  server.on("/files", [this]() { handleFiles(); });
  server.on("/edit", [this]() { handleEdit(); });
  server.on("/save", HTTP_POST, [this]() { handleSave(); });

  //WiFi
  server.on("/wifi", [this]() { handleWifiList(); });
  server.on("/doConnectWifi", HTTP_POST, [this]() { handleDoConnectWifi(); });
  server.on("/createNetwork", [this]() { handleCreateNetwork(); });
  server.on("/doCreateNetwork", HTTP_POST, [this]() { handleDoCreateNetwork(); });
  server.on("/doCreateFile", HTTP_POST, [this]() { handleDoCreateFile(); });
  server.on("/stopNetworks", [this]() { handleStopNetworks(); });
  server.on("/doDeauth", [this]() { handleDoDeauth(); });
  server.on("/doStopDeauth", [this]() { handleStopDeauth(); });
  server.on("/runBeaconSpam", [this]() { 
    WifiManager::runBeaconSpam();
    redirect("/"); 
  });

  server.on("/stopBeaconSpam", [this]() { 
    WifiManager::stopBeaconSpam();
    redirect("/"); 
  });


  //Bad USB
  server.on("/badUsb", [this]() { handleBadUsb(); });
  server.on("/badUsbPayload", [this]() { handleBadUsbPayload(); });

  //Bluetooth
  server.on("/bluetooth", [this]() { handleBluetooth(); });
  server.on("/bleStart", [this]() { handleBleStart(); });
  server.on("/bleStop", [this]() { handleBleStop(); });

  //System
  server.on("/status", [this]() { handleStatus(); });
  server.on("/restart", [this]() { SystemStatus::restart(); });
}

void ServerManager::begin() {
  server.begin();
}

void ServerManager::handleRoot() {
  std::vector<MenuItem> menuItems = {
    MenuItem("main", "wifi", "Wifi", "wifi", {}),
    MenuItem("main", "bluetooth", "Bluetooth", "bluetooth",{}),
    MenuItem("main", "files", "Files", "folder",{}),
    MenuItem("main", "badUsb", "Bad USB", "usb",{}),
    MenuItem("main", "status", "Status", "chip",{}),
  };
  sendHtml(TemplateManager::getIndex(menuItems));
}

void ServerManager::sendHtml(String html) {
  server.send(200, "text/html", html);
}

void ServerManager::redirect(String path) {
  server.sendHeader("Location", path);
  server.send(302, "text/plain", "");
}

void ServerManager::handleClient() {
  server.handleClient();
}


void ServerManager::handleEdit() {
  String html = "";
  if (server.hasArg("file")) {
    String filePath = server.arg("file");
    String text = Storage::readFile(filePath);
    sendHtml(TemplateManager::getTextEditor(filePath, text));
  } else {
    Serial.println("Param 'file' not found");
  }

  sendHtml(html);
}



void ServerManager::handleBluetooth(){
  sendHtml(TemplateManager::getBluetoothList());
}

void ServerManager::handleBleStart(){
  Bluetooth::enable();
  redirect("/");
}

void ServerManager::handleBleStop(){
  Bluetooth::disable();
  redirect("/");
}

void ServerManager::handleCreateNetwork() {
  sendHtml(TemplateManager::getCreateNetwork());
}

void ServerManager::handleStopNetworks() {
  WiFi.softAPdisconnect(true);
  redirect("/");
}

void ServerManager::handleStopDeauth() {
  Deauther::stop();
  redirect("/");
}


void ServerManager::handleSave() {
  if (server.hasArg("fileContent")) {
    String text = server.arg("fileContent");
    String name = server.arg("fileName");
    
    Storage::writeFile(name, text);
  } else {
    Serial.println("Param 'fileContent' not found");
  }

  redirect("/files");

}

void ServerManager::handleDoCreateNetwork() {
  if (server.hasArg("network_name") && server.hasArg("network_password")) {
    WifiManager::create(server.arg("network_name"), server.arg("network_password"));
  } else {
    Serial.println("Params not found");
  }

  redirect("/");
}

void ServerManager::handleDoConnectWifi() {
  if (server.hasArg("network_name") && server.hasArg("network_password")) {
    WifiManager::connect(server.arg("network_name"), server.arg("network_password"));
  } else {
    Serial.println(F("Params not found"));
  }

  redirect("/");
}

void ServerManager::handleDoCreateFile() {
  if (server.hasArg("name") && server.hasArg("type")) {
    String name = server.arg("name");

    if(server.arg("type") == "folder") {
      Storage::mkdir(name);
    } else {
      Storage::writeFile(name, "");
    }

  } else {
    Serial.println("Params not found");
  }
}


void ServerManager::handleStatus(){
  UsageStats storageStatus = SystemStatus::getStorageStatus();
  UsageStats memoryStatus = SystemStatus::getMemoryStatus();
  NetworkInfo networkInfo = SystemStatus::getNetworkInfo();
  sendHtml(TemplateManager::getSystemStatus(storageStatus, memoryStatus, networkInfo));
}


void ServerManager::handleFiles(){
  std::vector<String> files = Storage::listDir("/");
  sendHtml(TemplateManager::getFiles(files));
}



void ServerManager::handleWifiList(){
  if (networks.empty()) {
    networks = WifiManager::getNetworks();
  }

  sendHtml(TemplateManager::getNetworks(networks));
}

void ServerManager::handleBadUsb() {
  std::vector<String> files = BadUSB::list();
  sendHtml(TemplateManager::getBadUsbPayloads(files));
}

void ServerManager::handleBadUsbPayload() {

  if(server.hasArg("file")) {
    BadUSB::run(server.arg("file"));
  }
  redirect("/badUsb");
}



void ServerManager::handleDoDeauth() {
  if(server.hasArg("id")) {
    WifiNetwork n = networks[server.arg("id").toInt()];
    Deauther::start(n);
    server.send(200, "text/html", n.toString());
  }

  server.send(200, "text/html", "Err");
}



