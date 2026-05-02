#include "ServerManager.h"
#include "Storage.h"
#include <WifiManager.h>
#include "secrets.h"
#include "WifiNetwork.h"
#include "BadUSB.h"
#include "Deauther.h"
#include "Bluetooth.h"
#include "Utils.h"
#include "SystemStatus.h"
#include <map>
#include <TemplateManager.h>
#include <ESPAsyncWebServer.h>
#include <Scanner.h>
#include <LittleFS.h>
String scanResult = "";

AsyncEventSource events("/scanPortsReport");

void ServerManager::setUpRoutes() {

  server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) { handleRoot(request); });
  server.serveStatic("/styles.css", LittleFS, "/styles.css").setCacheControl("public,max-age=31536000, inmutable");
  server.serveStatic("/sprites.svg", LittleFS, "/templates/cmn/icons.svg").setCacheControl("public,max-age=31536000, inmutable");

  // Files
  server.on("/files", HTTP_GET, [this](AsyncWebServerRequest *request) { handleFiles(request); });
  server.on("/edit", HTTP_GET, [this](AsyncWebServerRequest *request) { handleEdit(request); });
  server.on("/save", HTTP_POST, [this](AsyncWebServerRequest *request) { handleSave(request); });

  // WiFi
  server.on("/wifi", HTTP_GET, [this](AsyncWebServerRequest *request) { handleWifiList(request); });
  server.on("/doConnectWifi", HTTP_POST, [this](AsyncWebServerRequest *request) { handleDoConnectWifi(request); });
  server.on("/createNetwork", HTTP_GET, [this](AsyncWebServerRequest *request) { handleCreateNetwork(request); });
  server.on("/doCreateNetwork", HTTP_POST, [this](AsyncWebServerRequest *request) { handleDoCreateNetwork(request); });
  server.on("/doCreateFile", HTTP_POST, [this](AsyncWebServerRequest *request) { handleDoCreateFile(request); });
  server.on("/stopNetworks", HTTP_GET, [this](AsyncWebServerRequest *request) { handleStopNetworks(request); });
  server.on("/doDeauth", HTTP_GET, [this](AsyncWebServerRequest *request) { handleDoDeauth(request); });
  server.on("/doStopDeauth", HTTP_GET, [this](AsyncWebServerRequest *request) { handleStopDeauth(request); });

  server.on("/runBeaconSpam", HTTP_GET, [this](AsyncWebServerRequest *request) { 
    WifiManager::runBeaconSpam();
    redirect(request, "/"); 
  });

  server.on("/stopBeaconSpam", HTTP_GET, [this](AsyncWebServerRequest *request) { 
    WifiManager::stopBeaconSpam();
    redirect(request, "/"); 
  });

  // Bad USB
  server.on("/badUsb", HTTP_GET, [this](AsyncWebServerRequest *request) { handleBadUsb(request); });
  server.on("/badUsbPayload", HTTP_GET, [this](AsyncWebServerRequest *request) { handleBadUsbPayload(request); });

  // Bluetooth
  server.on("/bluetooth", HTTP_GET, [this](AsyncWebServerRequest *request) { handleBluetooth(request); });
  server.on("/bleStart", HTTP_GET, [this](AsyncWebServerRequest *request) { handleBleStart(request); });
  server.on("/bleStop", HTTP_GET, [this](AsyncWebServerRequest *request) { handleBleStop(request); });


  //Network
  server.on("/network", HTTP_GET, [this](AsyncWebServerRequest *request) { handleNetwork(request); });
  server.on("/network/scanPorts", HTTP_POST, [this](AsyncWebServerRequest *request) { handleScanPorts(request); });
  server.on("/network/scanIps", HTTP_POST, [this](AsyncWebServerRequest *request) { handleScanIps(request); });
  
  //System
  server.on("/status", HTTP_GET, [this](AsyncWebServerRequest *request) {
    handleStatus(request);
  });

  server.on("/restart", HTTP_GET, [this](AsyncWebServerRequest *request) {
    SystemStatus::restart();
  });

}

void ServerManager::begin() {
  ServerManager::setUpRoutes();
  server.addHandler(&events);
  server.begin();
}

void ServerManager::readAsyncReport() {

  Scanner::scanCommonPorts([&](const String& msg){
    if (msg == "DONE") {
      events.send("ok", "done");
    } else {
      Serial.println(msg.c_str());
      events.send(msg.c_str(), "message");
    }
  });

}

void ServerManager::handleRoot(AsyncWebServerRequest *request) {
  std::vector<MenuItem> menuItems = {
    MenuItem("wifi", "Wifi", "wifi", {}),
    MenuItem("bluetooth", "Bluetooth", "bluetooth",{}),
    MenuItem("network", "Network", "network",{}),
    MenuItem("files", "Files", "folder",{}),
    MenuItem("badUsb", "Bad USB", "usb",{}),
    MenuItem("status", "Status", "chip",{}),
  };
  sendHtml(request, TemplateManager::getIndex(menuItems));
}

void ServerManager::sendHtml(AsyncWebServerRequest *request, String html) {
    request->send(200, "text/html", html);
}

void ServerManager::redirect(AsyncWebServerRequest *request, String path) {
  request->redirect(path);
}

void ServerManager::handleEdit(AsyncWebServerRequest *request) {
  String html = "";
  if (request->hasArg("file")) {
    String filePath = request->arg("file");
    String text = Storage::readFile(filePath);

    Serial.println(TemplateManager::getTextEditor(filePath, text));
    sendHtml(request, TemplateManager::getTextEditor(filePath, text));
  } else {
    sendHtml(request, "Param 'file' not found");
  }
}


void ServerManager::handleNetwork(AsyncWebServerRequest *request){
  NetworkInfo networkInfo = SystemStatus::getNetworkInfo();
  sendHtml(request, TemplateManager::getNetworkMenu(networkInfo));
}


void ServerManager::handleScanPorts(AsyncWebServerRequest *request){
  String ip = request->arg("ip");
  Scanner::startScanPort(ip);
  request->send(200, "text/plain", "ok");
}

void ServerManager::handleScanIps(AsyncWebServerRequest *request){
  String ipStart = request->arg("ip_start");
  String ipEnd = request->arg("ip_end");
  if(Utils::isValidIPv4(ipStart) && Utils::isValidIPv4(ipEnd)) {
    Scanner::startScanIp(ipStart, ipEnd);
  }
  request->send(200, "text/plain", "ok");
}


void ServerManager::handleBluetooth(AsyncWebServerRequest *request){
  sendHtml(request, TemplateManager::getBluetoothList());
}

void ServerManager::handleBleStart(AsyncWebServerRequest *request){
  Bluetooth::enable();
  redirect(request, "/");
}

void ServerManager::handleBleStop(AsyncWebServerRequest *request){
  Bluetooth::disable();
  redirect(request, "/");
}

void ServerManager::handleCreateNetwork(AsyncWebServerRequest *request) {
  sendHtml(request, TemplateManager::getCreateNetwork());
}

void ServerManager::handleStopNetworks(AsyncWebServerRequest *request) {
  //WiFi.softAPdisconnect(true);
  redirect(request, "/");
}

void ServerManager::handleStopDeauth(AsyncWebServerRequest *request) {
  Deauther::stop();
  redirect(request, "/");
}


void ServerManager::handleSave(AsyncWebServerRequest *request) {
  if (request->hasArg("fileContent")) {
    String text = request->arg("fileContent");
    String name = request->arg("fileName");
    
    Storage::writeFile(name, text);
  } else {
    Serial.println("Param 'fileContent' not found");
  }

  redirect(request, "/files");
}

void ServerManager::handleDoCreateNetwork(AsyncWebServerRequest *request) {
  if (request->hasArg("network_name") && request->hasArg("network_password")) {
    WifiManager::create(request->arg("network_name"), request->arg("network_password"));
  } else {
    Serial.println("Params not found");
  }

  redirect(request, "/");
}

void ServerManager::handleDoConnectWifi(AsyncWebServerRequest *request) {
  if (request->hasArg("network_name") && request->hasArg("network_password")) {
    WifiManager::connect(request->arg("network_name"), request->arg("network_password"));
  } else {
    Serial.println(F("Params not found"));
  }

  redirect(request, "/");
}

void ServerManager::handleDoCreateFile(AsyncWebServerRequest *request) {
  if (request->hasArg("name") && request->hasArg("type")) {
    String name = request->arg("name");

    if(request->arg("type") == "folder") {
      Storage::mkdir(name);
    } else {
      Storage::writeFile(name, "");
    }

  } else {
    Serial.println("Params not found");
  }
}


void ServerManager::handleStatus(AsyncWebServerRequest *request){
  UsageStats storageStatus = SystemStatus::getStorageStatus();
  UsageStats memoryStatus = SystemStatus::getMemoryStatus();
  NetworkInfo networkInfo = SystemStatus::getNetworkInfo();
  sendHtml(request, TemplateManager::getSystemStatus(storageStatus, memoryStatus, networkInfo));
}


void ServerManager::handleFiles(AsyncWebServerRequest *request){
  std::vector<String> files = Storage::listDir("/");
  sendHtml(request, TemplateManager::getFiles(files));
}



void ServerManager::handleWifiList(AsyncWebServerRequest *request){
  if (networks.empty()) {
    networks = WifiManager::getNetworks();
  }

  sendHtml(request, TemplateManager::getNetworks(networks));
}

void ServerManager::handleBadUsb(AsyncWebServerRequest *request) {
  std::vector<String> files = BadUSB::list();
  sendHtml(request, TemplateManager::getBadUsbPayloads(files));
}

void ServerManager::handleBadUsbPayload(AsyncWebServerRequest *request) {

  if(request->hasArg("file")) {
    BadUSB::run(request->arg("file"));
  }
  redirect(request, "/badUsb");
}



void ServerManager::handleDoDeauth(AsyncWebServerRequest *request) {
  if(request->hasArg("id")) {
    WifiNetwork n = networks[request->arg("id").toInt()];
    Deauther::start(n);
    sendHtml(request, n.toString());
  } else {
    sendHtml(request, "Param 'id' not found");
  }
}



