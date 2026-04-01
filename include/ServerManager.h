#pragma once
#include <vector>
#include "WifiNetwork.h"
#include "models/MenuItem.h"
#include "models/UsageStats.h"

class ServerManager {
public:
    ServerManager();
    void begin();
    void handleClient();
    
private:
    std::vector<WifiNetwork> networks;
    WebServer server{80};
    void handleRoot();
    void sendHtml(String html);
    void redirect(String path);
    String getMenu(const String& menuName);

    void handleEdit();
    void handleBluetooth();
    void handleBleStart();
    void handleBleStop();
    void handleCreateNetwork();
    void handleStopNetworks();
    void handleStopDeauth();
    void handleSave();
    void handleDoCreateNetwork();
    void handleDoConnectWifi();
    void handleDoCreateFile();
    String getMainTemplate(String title, String menu);
    void handleStatus();
    void handleFiles();
    void handleWifiList();
    void handleBadUsb();
    void handleBadUsbPayload();
    void handleDoDeauth();
    void getNetworks();
};
