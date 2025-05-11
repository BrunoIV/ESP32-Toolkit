#include <WiFi.h>
#include <WebServer.h>
#include <vector>

#include "secrets.h"
#include "WifiNetwork.h"
#include "MenuItem.h"
#include "BadUSB.h"
#include "Storage.h"
#include "SPIFFS.h"
#include "Deauther.h"
#include "Bluetooth.h"
#include "Utils.h"
#include "SystemStatus.h"
#include "models/UsageStats.h"

std::vector<WifiNetwork> networks;
std::vector<MenuItem> menus;
WebServer server(80);

void setup() {
  Serial.begin(115200);

  Serial.println("Welcome to ESP32-Toolkit");
  Storage::init();

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(WIFI_SSID, WIFI_PASS);
  delay(100);

  

  server.on("/", handleRoot);
  server.on("/files", handleFiles);
  server.on("/edit", handleEdit);
  server.on("/save", HTTP_POST, handleSave);
  server.on("/status", handleStatus);

  server.on("/connectWifi", handleConnectWifi);
  server.on("/doConnectWifi", HTTP_POST, handleDoConnectWifi);


  server.on("/createNetwork", handleCreateNetwork);
  server.on("/doCreateNetwork", HTTP_POST, handleDoCreateNetwork);
  server.on("/stopNetworks", handleStopNetworks);
  server.on("/deauther", handleDeauther);
  server.on("/doDeauth", handleDoDeauth);
  server.on("/doStopDeauth", handleStopDeauth);


  server.on("/badUsb", handleBadUsb);
  server.on("/badUsbPayload", handleBadUsbPayload);

  server.on("/bleStart", handleBleStart);
  server.on("/bleStop", handleBleStop);
  

  server.begin();
}

void handleEdit() {
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

void handleBleStart(){
  Bluetooth::enable();
  redirect("/");
}

void handleBleStop(){
  Bluetooth::disable();
  redirect("/");
}

void handleCreateNetwork() {
  sendHtml(getMainTemplate("Create Network", Storage::readFile("/create_network.html")));
}

void handleStopNetworks() {
  WiFi.softAPdisconnect(true);
  redirect("/");
}

void handleStopDeauth() {
  Deauther::stop();
  redirect("/");
}


void handleSave() {
  if (server.hasArg("fileContent")) {
    String text = server.arg("fileContent");
    String name = server.arg("fileName");
    
    Storage::writeFile(name, text);
  } else {
    Serial.println("Param 'fileContent' not found");
  }

  redirect("/files");

}

void handleDoCreateNetwork() {
  if (server.hasArg("network_name") && server.hasArg("network_password")) {
    WiFi.softAP(server.arg("network_name"), server.arg("network_password"));
  } else {
    Serial.println("Params not found");
  }

  redirect("/");
}

void handleDoConnectWifi() {
  if (server.hasArg("network_name") && server.hasArg("network_password")) {
    connectWifi(server.arg("network_name").c_str(), server.arg("network_password").c_str());
  } else {
    Serial.println("Params not found");
  }

  redirect("/");
}

String getMainTemplate(String title, String menu) {
  String html = Storage::readFile("/index.html");
  String stylesText = Storage::readFile("/styles.css");
  html.replace("{{TITLE}}", title);
  html.replace("{{MENU}}", menu);
  html.replace("<link rel='stylesheet' href='styles.css' />", "<style>" + stylesText + "</style>");
  return html;
}


void handleRoot(){
  sendHtml(getMainTemplate("ESP32 Toolkit", getMenu("main")));
}


void handleStatus(){
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

  sendHtml(getMainTemplate("Status", returnHtml));
}


void handleFiles(){

  String folder = "/";
  if(server.hasArg("folder")) {
    folder = server.arg("folder");
  }

  std::vector<MenuItem> files = Storage::listDir(folder, "/edit");
  
  String stringFiles = "";
  for (const auto& file : files) {
    stringFiles+= file.toString();
  }

  sendHtml(getMainTemplate("Files", stringFiles));
}


void handleConnectWifi(){
  getNetworks();
  String html = Storage::readFile("/templates/wifi/connect.html");

  String wifiString = "";
  for (const auto& network : networks) {
    MenuItem menu = MenuItem("", "#" + network.getName(), network.getName(), "fa fa-wifi", std::vector<MenuItem>());
    wifiString+= menu.toString();
  }
  
  html.replace("{{NETWORKS}}", wifiString);
  sendHtml(getMainTemplate("WiFi Networks", html));
}


void handleDeauther(){
  getNetworks();

  String wifiString = "";
  MenuItem menuAll = MenuItem("", "/doDeauth?id=ALL", "All networks", "fa fa-wifi", std::vector<MenuItem>());
  wifiString+= menuAll.toString();

  int i=0;
  for (const auto& network : networks) {
    MenuItem menu = MenuItem("", "/doDeauth?id=" + String(i), network.getName(), "fa fa-wifi", std::vector<MenuItem>());
    wifiString+= menu.toString();
    i++;
  }
  
  sendHtml(getMainTemplate("Deauther", wifiString));
}

void handleBadUsb() {
  std::vector<MenuItem> files = BadUSB::list();
  
  String stringFiles = "";
  for (const auto& file : files) {
    stringFiles+= file.toString();
  }

  sendHtml(getMainTemplate("Bad USB Payloads", stringFiles));
}

void handleBadUsbPayload() {

  if(server.hasArg("file")) {
    BadUSB::run(server.arg("file"));
  }
  redirect("/badUsb");
}



void handleDoDeauth() {
  if(server.hasArg("id")) {
    WifiNetwork n = networks[server.arg("id").toInt()];
    Deauther::start(n);
    server.send(200, "text/html", n.toString());
  }

  server.send(200, "text/html", "Err");
}

void sendHtml(String html) {
  server.send(200, "text/html", html);
}

void redirect(String path) {
  server.sendHeader("Location", path);
  server.send(302, "text/plain", "");
}



String getMenu(const String& menuName) {
    std::vector<MenuItem> menuItems = {
        MenuItem("main", "", "Wifi", "fa fa-wifi", {
          MenuItem("wifi", "connectWifi", "Connect", "fa-solid fa-binoculars", std::vector<MenuItem>()),
          MenuItem("wifi", "createNetwork", "Create network", "fa-solid fa-circle-plus", std::vector<MenuItem>()),
          MenuItem("wifi", "stopNetworks", "Stop networks", "fa-solid fa-stop", std::vector<MenuItem>()),
          MenuItem("wifi", "deauther", "Deauther", "fa-solid fa-skull", std::vector<MenuItem>()),
          MenuItem("wifi", "", "Beacon Spam", "fa-solid fa-house-flood-water", {
            MenuItem("beaconSpam", "rickRoll", "Rick Roll", "fa-solid fa-house-flood-water", std::vector<MenuItem>()),
            MenuItem("beaconSpam", "randomNetworks", "Random", "fa-solid fa-house-flood-water", std::vector<MenuItem>()),
            MenuItem("beaconSpam", "cloneExistant", "Clone Existant", "fa-solid fa-house-flood-water", std::vector<MenuItem>())
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

    
    String menu = "<ul>";
    for (const auto& item : menuItems) {
      menu += item.toString();
    }
    
    return menu + "</ul>";
}


void connectWifi(const char* ssid, const char* password) {
  Serial.print("Connecting to " + String(ssid) + "...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  
  Serial.println("\nConnection success");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  delay(200);
}

void getNetworks() {
  Serial.println("Scanning networks...");
  delay(100);

  int size = WiFi.scanNetworks();
  const String encriptationsName[10] = { "OPEN", "WEP", "WPA PSK", "WPA2 PSK", "WPA WPA2 PSK", "WPA2 ENTERPRISE", "WPA3 PSK", "WPA2 WPA3 PSK", "WAPI_PSK", "MAX" };
  for (int i = 0; i < size; ++i) {
    networks.push_back(WifiNetwork(WiFi.SSID(i), WiFi.channel(i), WiFi.BSSID(i), encriptationsName[WiFi.encryptionType(i)], WiFi.RSSI(i)));
  }
}

void loop() {
  server.handleClient();

  if (Bluetooth::isRunning()) {
    Bluetooth::spamBLE();
  }
  
  delay(100);
}
