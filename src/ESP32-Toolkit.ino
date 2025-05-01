#include <WiFi.h>
#include <WebServer.h>
#include <vector>

#include "secrets.h"
#include "WifiNetwork.h"
#include "MenuItem.h"
#include "Storage.h"
#include "SPIFFS.h"

std::vector<WifiNetwork> networks;
std::vector<MenuItem> menus;
WebServer server(80);

void setup() {
  Serial.begin(115200);
  Storage::init();
  connectWifi(WIFI_SSID, WIFI_PASS);

  server.on("/", handleRoot);
  server.on("/files", handleFiles);
  server.on("/edit", handleEdit);
  server.on("/save", HTTP_POST, handleSave);
  server.on("/status", handleStatus);
  server.on("/scan", handleScanNetworks);
  server.on("/createNetwork", handleCreateNetwork);
  server.on("/doCreateNetwork", HTTP_POST, handleDoCreateNetwork);
  server.on("/stopNetworks", handleStopNetworks);
  server.on("/deauther", handleDeauther);

  server.begin();
}

void handleEdit() {
  String html = "";
  if (server.hasArg("file")) {
    String filePath = server.arg("file");
    String text = Storage::readFile(filePath);

    
    html = Storage::readFile("/editor.html");
    html.replace("{{FILE_NAME}}", filePath);
    html.replace("{{TEXT}}", escapeHTML(text));
    
  } else {
    Serial.println("Param 'file' not found");
  }

  sendHtml(html);
}

void handleCreateNetwork() {
  sendHtml(getMainTemplate("Create Network", Storage::readFile("/create_network.html")));
}




void handleStopNetworks() {
  WiFi.softAPdisconnect(true);
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

String escapeHTML(String input) {
  input.replace("&", "&amp;");
  input.replace("<", "&lt;");
  input.replace(">", "&gt;");
  input.replace("\"", "&quot;");
  input.replace("'", "&#39;");
  return input;
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
  size_t totalKb = SPIFFS.totalBytes() / 1024.0;
  size_t usedKb = SPIFFS.usedBytes() / 1024.0;
  int8_t percentageUsed = ceil((usedKb * 100.0) / totalKb);  

  String returnHtml = "";
  
  String html = Storage::readFile("/chart.html");
  html.replace("{{CHART_TITLE}}", "Storage");
  html.replace("{{FREE_STORAGE}}", String(totalKb - usedKb));
  html.replace("{{VAL1}}", String(percentageUsed));
  html.replace("{{VAL2}}", String(percentageUsed));
  html.replace("{{1}}", String(usedKb));
  html.replace("{{2}}", String(totalKb));
  returnHtml += html;

  
  size_t ram_total = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
  size_t ram_libre = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
  size_t ram_usada = ram_total - ram_libre;
  int8_t percentageUsedRam = ceil((ram_usada * 100.0) / ram_total);  

  
  html = Storage::readFile("/chart.html");
  html.replace("{{CHART_TITLE}}", "RAM");
  html.replace("{{FREE_STORAGE}}", String(ram_libre / 1024.0));
  html.replace("{{VAL1}}", String(percentageUsedRam));
  html.replace("{{VAL2}}", String(percentageUsedRam));
  html.replace("{{1}}", String(ram_usada / 1024.0));
  html.replace("{{2}}", String(ram_total / 1024.0));
  returnHtml += html;

  sendHtml(getMainTemplate("Status", returnHtml));
}


void handleFiles(){
  std::vector<String> files = Storage::listDir();
  
  String stringFiles = "";
  for (const String& file : files) {
    MenuItem menu = MenuItem("", "/edit?file=" + file, file, "fa fa-file", std::vector<MenuItem>());
    stringFiles+= menu.toString();
  }

  sendHtml(getMainTemplate("Files", stringFiles));
}


void handleScanNetworks(){
  getNetworks();

  String wifiString = "";
  for (const auto& network : networks) {
    MenuItem menu = MenuItem("", "/wifiDetails?id=" + network.getName(), network.getName(), "fa fa-wifi", std::vector<MenuItem>());
    wifiString+= menu.toString();
  }
  
  sendHtml(getMainTemplate("WiFi Networks", wifiString));
}


void handleDeauther(){
  getNetworks();

  String wifiString = "";
  MenuItem menuAll = MenuItem("", "/death?id=ALL", "All networks", "fa fa-wifi", std::vector<MenuItem>());
  wifiString+= menuAll.toString();

  for (const auto& network : networks) {
    MenuItem menu = MenuItem("", "/death?id=" + network.getName(), network.getName(), "fa fa-wifi", std::vector<MenuItem>());
    wifiString+= menu.toString();
  }
  
  sendHtml(getMainTemplate("Deauther", wifiString));
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
          MenuItem("wifi", "scan", "Scan", "fa-solid fa-binoculars", std::vector<MenuItem>()),
          MenuItem("wifi", "createNetwork", "Create network", "fa-solid fa-circle-plus", std::vector<MenuItem>()),
          MenuItem("wifi", "stopNetworks", "Stop networks", "fa-solid fa-stop", std::vector<MenuItem>()),
          MenuItem("wifi", "deauther", "Deauther", "fa-solid fa-skull", std::vector<MenuItem>()),
          MenuItem("wifi", "", "Beacon Spam", "fa-solid fa-house-flood-water", {
            MenuItem("beaconSpam", "rickRoll", "Rick Roll", "fa-solid fa-house-flood-water", std::vector<MenuItem>()),
            MenuItem("beaconSpam", "randomNetworks", "Random", "fa-solid fa-house-flood-water", std::vector<MenuItem>()),
            MenuItem("beaconSpam", "cloneExistant", "Clone Existant", "fa-solid fa-house-flood-water", std::vector<MenuItem>())
          })
        }),       
        
        MenuItem("main", "bluetooth", "Bluetooth", "fa-brands fa-bluetooth", std::vector<MenuItem>()),
        MenuItem("main", "files", "Files", "fa fa-folder", std::vector<MenuItem>()),
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
  WiFi.mode(WIFI_STA);
  delay(100);

  int size = WiFi.scanNetworks();
  const String encriptationsName[10] = { "OPEN", "WEP", "WPA PSK", "WPA2 PSK", "WPA WPA2 PSK", "WPA2 ENTERPRISE", "WPA3 PSK", "WPA2 WPA3 PSK", "WAPI_PSK", "MAX" };
  for (int i = 0; i < size; ++i) {
    networks.push_back(WifiNetwork(WiFi.SSID(i), WiFi.channel(i), WiFi.BSSID(i), encriptationsName[WiFi.encryptionType(i)], WiFi.RSSI(i)));
  }
}

String getBSSIDString(int index) {
  uint8_t* bssid = WiFi.BSSID(index);  //BSSID as an array of bytes
  String bssidStr = "";
  
  //Convert to format "XX:XX:XX:XX:XX:XX"
  for (int i = 0; i < 6; i++) {
    if (bssid[i] < 0x10) {
      bssidStr += "0";  // Adds a zero to the left if byte < 16
    }
    
    bssidStr += String(bssid[i], HEX);
    if (i < 5) {
      bssidStr += ":";
    }
  }
  return bssidStr;
}

void loop() {
  server.handleClient();
  delay(100);
}
