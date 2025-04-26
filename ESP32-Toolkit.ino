#include <WiFi.h>
#include <WebServer.h>
#include <vector>

#include "secrets.h"
#include "WifiNetwork.h"
#include "MenuItem.h"


std::vector<WifiNetwork> networks;
std::vector<MenuItem> menus;
WebServer server(80);

void setup() {
  Serial.begin(115200);
  connectWifi(WIFI_SSID, WIFI_PASS);

  server.on("/", handleRoot);
  server.begin();
}

void handleRoot(){
  server.send(200, "text/html", getMenu("main"));
}


String getMenu(const String& menuName) {
    std::vector<MenuItem> menuItems = {
        MenuItem("main", "wifi", "Wifi", ""),
        MenuItem("main", "bluetooth", "Bluetooth", ""),
        MenuItem("main", "files", "Files", ""),
        MenuItem("main", "status", "Status", ""),
        MenuItem("wifi", "escanear", "Scan", "")
    };

    String menu = "<html><body><div class='list'>";
    for (const auto& item : menuItems) {
        if (item.getCategory() == menuName) {
            menu += "<a href='/" + item.getUrl() + "'>" + item.getName() + "</a><br>";
        }
    }
    menu += "</div></body></html>";
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
