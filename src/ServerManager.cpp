#include "ServerManager.h"
#include "Storage.h"

#include <WifiManager.h>
#include <WebServer.h>
#include "secrets.h"
#include "WifiNetwork.h"
#include "MenuItem.h"
#include "BadUSB.h"
#include "SPIFFS.h"
#include "Deauther.h"
#include "Bluetooth.h"
#include "Utils.h"
#include "SystemStatus.h"
#include "models/UsageStats.h"


ServerManager::ServerManager() {
  server.on("/", [this]() { handleRoot(); });

  //Files
  server.on("/files", [this]() { handleFiles(); });
  server.on("/edit", [this]() { handleEdit(); });
  server.on("/save", HTTP_POST, [this]() { handleSave(); });

  //WiFi
  server.on("/connectWifi", [this]() { handleConnectWifi(); });
  server.on("/doConnectWifi", HTTP_POST, [this]() { handleDoConnectWifi(); });
  server.on("/createNetwork", [this]() { handleCreateNetwork(); });
  server.on("/doCreateNetwork", HTTP_POST, [this]() { handleDoCreateNetwork(); });
  server.on("/stopNetworks", [this]() { handleStopNetworks(); });
  server.on("/deauther", [this]() { handleDeauther(); });
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
  server.on("/bleStart", [this]() { handleBleStart(); });
  server.on("/bleStop", [this]() { handleBleStop(); });

  //System
  server.on("/status", [this]() { handleStatus(); });
  server.on("/restart", [this]() { SystemStatus::restart(); });
}

void ServerManager::begin() {
  server.begin();
}

String ServerManager::getMainTemplate(String title, String menu) {
  String html = Storage::readFile("/index.html");
  String stylesText = Storage::readFile("/styles.css");
  html.replace("{{TITLE}}", title);
  html.replace("{{MENU}}", menu);
  html.replace("<link rel='stylesheet' href='styles.css' />", "<style>" + stylesText + "</style>");
  return html;
}

void ServerManager::handleRoot() {
  sendHtml(getMainTemplate("ESP32 Toolkit", getMenu("main")));
}


String ServerManager::getMenu(const String& menuName) {
    std::vector<MenuItem> menuItems = {
        MenuItem("main", "", "Wifi", "fa fa-wifi", {
          MenuItem("wifi", "connectWifi", "Connect", "fa-solid fa-binoculars", std::vector<MenuItem>()),
          MenuItem("wifi", "createNetwork", "Create network", "fa-solid fa-circle-plus", std::vector<MenuItem>()),
          MenuItem("wifi", "stopNetworks", "Stop networks", "fa-solid fa-stop", std::vector<MenuItem>()),
          MenuItem("wifi", "deauther", "Deauther", "fa-solid fa-skull", std::vector<MenuItem>()),
          MenuItem("wifi", "", "Beacon Spam", "fa-solid fa-house-flood-water", {
            MenuItem("beaconSpam", "runBeaconSpam", "Run", "fa-solid fa-house-flood-water", std::vector<MenuItem>()),
            MenuItem("beaconSpam", "stopBeaconSpam", "Stop", "fa-solid fa-house-flood-water", std::vector<MenuItem>()),
          })
        }),       
        
        MenuItem("main", "bluetooth", "Bluetooth", "fa-brands fa-bluetooth", {
          MenuItem("bluethooth", "bleStart", "Start BLE Spam", "fa-brands fa-bluetooth", std::vector<MenuItem>()),
          MenuItem("bluethooth", "bleStop", "Stop BLE Spam", "fa-brands fa-bluetooth", std::vector<MenuItem>()),
        }),
        
        MenuItem("main", "files", "Files", "fa fa-folder", std::vector<MenuItem>()),
        MenuItem("main", "badUsb", "Bad USB", "fa-brands fa-usb", std::vector<MenuItem>()),
        MenuItem("main", "status", "Status", "fa-solid fa-microchip", std::vector<MenuItem>()),
    };

    
    String menu = "<div class='list'>";
    for (const auto& item : menuItems) {
      menu += item.toString();
    }
    
    return menu + "</div>";
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

    
    html = Storage::readFile("/editor.html");
    html.replace("{{FILE_NAME}}", filePath);
    html.replace("{{TEXT}}", Utils::escapeHTML(text));
    
  } else {
    Serial.println("Param 'file' not found");
  }

  sendHtml(html);
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
  sendHtml(getMainTemplate("Create Network", Storage::readFile("/create_network.html")));
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
    Serial.println("Params not found");
  }

  redirect("/");
}

void ServerManager::handleDoCreateFile() {
  if (server.hasArg("name") && server.hasArg("type")) {
    String name = server.arg("name");

    if(server.arg("type") == "folder") {
      name += "/.keep";
    }

    Storage::writeFile(name, "");
  } else {
    Serial.println("Params not found");
  }

  redirect("/files");
}


void ServerManager::handleStatus(){
  String returnHtml = "";
  
  UsageStats storageStatus = SystemStatus::getStorageStatus();
  String html = Storage::readFile("/chart.html");
  html.replace("{{CHART_TITLE}}", "Storage");
  html.replace("{{FREE_STORAGE}}", String(storageStatus.getFreeAsKb()));
  html.replace("{{VAL1}}", String(storageStatus.getPercentUsed()));
  html.replace("{{VAL2}}", String(storageStatus.getPercentUsed()));
  html.replace("{{1}}", String(storageStatus.getUsedAsKb()));
  html.replace("{{2}}", String(storageStatus.getTotalAsKb()));
  returnHtml += html;


  UsageStats memoryStatus = SystemStatus::getMemoryStatus();
  html = Storage::readFile("/chart.html");
  html.replace("{{CHART_TITLE}}", "RAM");
  html.replace("{{FREE_STORAGE}}", String(memoryStatus.getFreeAsKb()));
  html.replace("{{VAL1}}", String(memoryStatus.getPercentUsed()));
  html.replace("{{VAL2}}", String(memoryStatus.getPercentUsed()));
  html.replace("{{1}}", String(memoryStatus.getUsedAsKb()));
  html.replace("{{2}}", String(memoryStatus.getTotalAsKb()));
  returnHtml += html;


  html = Storage::readFile("/templates/status/network.html");
  html.replace("{{IP}}", WiFi.localIP().toString());
  html.replace("{{MASK}}", WiFi.subnetMask().toString());
  html.replace("{{GATEWAY}}", WiFi.gatewayIP().toString());
  html.replace("{{DNS}}", WiFi.dnsIP().toString());
  html.replace("{{MAC}}", WiFi.macAddress());
  html.replace("{{NETWORK}}", WiFi.SSID());
  html.replace("{{SIGNAL}}", String(WiFi.RSSI()));
  returnHtml += html;

  sendHtml(getMainTemplate("Status", returnHtml));
}


void ServerManager::handleFiles(){

  String folder = "/";
  if(server.hasArg("folder")) {
    folder = server.arg("folder");
  }

  std::vector<MenuItem> files = Storage::listDir(folder, "/edit");
  
  String stringFiles = "";
  for (const auto& file : files) {
    stringFiles+= file.getAsDropdownMenu();
  }

  String tpl = getMainTemplate("Files", "<div class='list'>" + stringFiles + "</div>");

  String floatingMenu = Storage::readFile("/templates/cmn/float_menu.html");
  tpl.replace("<!-- floating_menu -->", floatingMenu);
  sendHtml(tpl);
}


void ServerManager::handleConnectWifi(){
  networks = WifiManager::getNetworks();
  String html = Storage::readFile("/templates/wifi/connect.html");

  String wifiString = "<div class='list'>";
  for (const auto& network : networks) {
    MenuItem menu = MenuItem("", "#" + network.getName(), network.getName(), "fa fa-wifi", std::vector<MenuItem>());
    wifiString+= menu.toString();
  }
  wifiString += "</div>";
  
  html.replace("{{NETWORKS}}", wifiString);
  sendHtml(getMainTemplate("WiFi Networks", html));
}


void ServerManager::handleDeauther(){
  networks = WifiManager::getNetworks();

  String wifiString = "<div class='list'>";
  MenuItem menuAll = MenuItem("", "/doDeauth?id=ALL", "All networks", "fa fa-wifi", std::vector<MenuItem>());
  wifiString+= menuAll.toString();

  int i=0;
  for (const auto& network : networks) {
    MenuItem menu = MenuItem("", "/doDeauth?id=" + String(i), network.getName(), "fa fa-wifi", std::vector<MenuItem>());
    wifiString+= menu.toString();
    i++;
  }
  
  wifiString += "</div>";
  sendHtml(getMainTemplate("Deauther", wifiString));
}

void ServerManager::handleBadUsb() {
  std::vector<MenuItem> files = BadUSB::list();
  
  String stringFiles = "<div class='list'>";
  for (const auto& file : files) {
    stringFiles+= file.toString();
  }
  stringFiles += "</div>";

  sendHtml(getMainTemplate("Bad USB Payloads", stringFiles));
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



