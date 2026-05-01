#pragma once
#include <vector>
#include "WifiNetwork.h"
#include "models/MenuItem.h"
#include "models/UsageStats.h"
#include <ESPAsyncWebServer.h>
class ServerManager {
public:
    ServerManager() : server(80) {}
    void setUpRoutes();
    void begin();
    void handleClient(AsyncWebServerRequest *request);
    void readAsyncReport();
    
private:
    AsyncWebServer server;

    std::vector<WifiNetwork> networks;
    void handleRoot(AsyncWebServerRequest *request);
    void sendHtml(AsyncWebServerRequest *request, String html);
    void redirect(AsyncWebServerRequest *request, String path);
    String getMenu(const String& menuName);

    void handleEdit(AsyncWebServerRequest *request);
    void handleBluetooth(AsyncWebServerRequest *request);
    void handleBleStart(AsyncWebServerRequest *request);
    void handleBleStop(AsyncWebServerRequest *request);
    void handleCreateNetwork(AsyncWebServerRequest *request);
    void handleStopNetworks(AsyncWebServerRequest *request);
    void handleStopDeauth(AsyncWebServerRequest *request);
    void handleSave(AsyncWebServerRequest *request);
    void handleDoCreateNetwork(AsyncWebServerRequest *request);
    void handleDoConnectWifi(AsyncWebServerRequest *request);
    void handleDoCreateFile(AsyncWebServerRequest *request);
    void handleStatus(AsyncWebServerRequest *request);
    void handleFiles(AsyncWebServerRequest *request);
    void handleWifiList(AsyncWebServerRequest *request);
    void handleBadUsb(AsyncWebServerRequest *request);
    void handleBadUsbPayload(AsyncWebServerRequest *request);
    void handleDoDeauth(AsyncWebServerRequest *request);
    void getNetworks(AsyncWebServerRequest *request);
    void handleNetwork(AsyncWebServerRequest *request);
    void handleScanPorts(AsyncWebServerRequest *request);
    void handleScanIps(AsyncWebServerRequest *request);
};
