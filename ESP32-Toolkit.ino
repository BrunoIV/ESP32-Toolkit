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
  server.on("/wifi", handleWifi);
  server.on("/files", handleFiles);
  server.on("/edit", handleEdit);
  server.on("/save", HTTP_POST, handleSave);
  server.on("/status", handleStatus);

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

  server.send(200, "text/html", html);
}

void handleSave() {
  if (server.hasArg("fileContent")) {
    String text = server.arg("fileContent");
    String name = server.arg("fileName");
    
    Storage::writeFile(name, text);
  } else {
    Serial.println("Param 'fileContent' not found");
  }

  server.sendHeader("Location", "/files");
  server.send(302, "text/plain", "");
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
  server.send(200, "text/html", getMainTemplate("ESP32 Toolkit", getMenu("main")));
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

  server.send(200, "text/html", getMainTemplate("Status", returnHtml));
}


void handleFiles(){
  std::vector<String> files = Storage::listDir();
  
  String stringFiles = "";
  for (const String& file : files) {
    String rowTemplateString = Storage::readFile("/list_item.html");
    rowTemplateString.replace("{{FILE}}", file);
    rowTemplateString.replace("{{URL}}", "/edit?file=" + file);
    rowTemplateString.replace("{{ICON}}", "fa fa-folder");

    stringFiles += rowTemplateString;
  }

  server.send(200, "text/html", getMainTemplate("Files", stringFiles));
}


void handleWifi(){
  getNetworks();

  String wifiString = "";
  for (const auto& network : networks) {
    String rowTemplateString = Storage::readFile("/list_item.html");
    rowTemplateString.replace("{{FILE}}", network.getName());
    rowTemplateString.replace("{{URL}}", "/wifiDetails?id=" + network.getName());
    rowTemplateString.replace("{{ICON}}", "fa fa-wifi");

    wifiString += rowTemplateString;
  }
  
  server.send(200, "text/html", getMainTemplate("WiFi Networks", wifiString));
}

String getMenu(const String& menuName) {
    std::vector<MenuItem> menuItems = {
        MenuItem("main", "wifi", "Wifi", "fa fa-wifi"),
        MenuItem("main", "bluetooth", "Bluetooth", "fa-brands fa-bluetooth"),
        MenuItem("main", "files", "Files", "fa fa-folder"),
        MenuItem("main", "status", "Status", "fa-solid fa-microchip"),
        MenuItem("wifi", "escanear", "Scan", "fa-solid fa-binoculars")
    };

    String menu = "";
    for (const auto& item : menuItems) {
        String rowTemplateString = Storage::readFile("/list_item.html");

        if (item.getCategory() == menuName) {
            rowTemplateString.replace("{{FILE}}", item.getName());
            rowTemplateString.replace("{{URL}}", item.getUrl());
            rowTemplateString.replace("{{ICON}}", item.getIcon());
            menu += rowTemplateString;
        }
    }
    return menu;
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
    String name = WiFi.SSID(i);
    networks.push_back(WifiNetwork(name, WiFi.channel(i), getBSSIDString(i), encriptationsName[WiFi.encryptionType(i)], WiFi.RSSI(i)));
    networks[i].printInfo();
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
