#include "TemplateManager.h"
#include <Storage.h>
#include <vector>
#include "WifiNetwork.h"
#include <Utils.h>

String TemplateManager::getMainTemplate(const String& title, const String& menu) {
  String html = Storage::readFile("/templates/cmn/index.html");
  String stylesText = Storage::readFile("/styles.css");
  html.replace("{{TITLE}}", title);
  html.replace("{{MENU}}", menu);
  html.replace("<link rel='stylesheet' href='styles.css' />", "<style>" + stylesText + "</style>");
  return html;
}

/// @brief Renders the files page including the floating menu
///
/// @param files Vector of file paths to include in the page
/// @return A String with the template, the file list and the floating menu
String TemplateManager::getFiles(const std::vector<String>& files){
  String stringFiles = "";
  for (const auto& file : files) {
    stringFiles += "'" + file + "',";
  }

  String tplFiles = Storage::readFile("/templates/files.html");
  tplFiles.replace("/*paths*/", stringFiles);

  String tpl = TemplateManager::getMainTemplate("Files", tplFiles);

  String floatingMenu = Storage::readFile("/templates/cmn/float_menu.html");
  tpl.replace("<!-- floating_menu -->", floatingMenu);
  return tpl;
}

String TemplateManager::getNetworks(const std::vector<WifiNetwork>& networks){
  String wifiString = "<div class='list'>";
  int i = 0;
  for (const auto& network : networks) {
    MenuItem menu = MenuItem("#network_" + String(i), network.getName(), "wifi", { {"onclick", "showDetail(\"" + network.getAsJson() + "\")" } });
    wifiString+= menu.toString();
    i++;
  }
  wifiString += "</div>";
  
  String details = Storage::readFile("/templates/wifi/details.html");
  String tpl = TemplateManager::getMainTemplate("WiFi Networks", wifiString + details);
  tpl.replace("<!-- right_icons -->", "<a onclick='history.back()' href='#'><svg><use href='#save' /></svg></a>");

  return tpl;
}

String TemplateManager::getBadUsbPayloads(const std::vector<String>& files){
  String stringFiles = "";
  for (const auto& file : files) {
    stringFiles+= "'" + file + "',";
  }

  String tplFiles = Storage::readFile("/templates/files.html");
  tplFiles.replace("/*paths*/", stringFiles);
  tplFiles.replace("/edit?", "/badUsbPayload?");

  String tpl = TemplateManager::getMainTemplate("USB Payloads", tplFiles);
  return tpl;
}

String TemplateManager::getSystemStatus(UsageStats& storageStatus, UsageStats& memoryStatus, NetworkInfo networkInfo){
  String returnHtml = "";
  
  String html = Storage::readFile("/chart.html");
  html.replace("{{CHART_TITLE}}", "Storage");
  html.replace("{{FREE_STORAGE}}", String(storageStatus.getFreeAsKb()));
  html.replace("{{VAL1}}", String(storageStatus.getPercentUsed()));
  html.replace("{{VAL2}}", String(storageStatus.getPercentUsed()));
  html.replace("{{1}}", String(storageStatus.getUsedAsKb()));
  html.replace("{{2}}", String(storageStatus.getTotalAsKb()));
  returnHtml += html;


  html = Storage::readFile("/chart.html");
  html.replace("{{CHART_TITLE}}", "RAM");
  html.replace("{{FREE_STORAGE}}", String(memoryStatus.getFreeAsKb()));
  html.replace("{{VAL1}}", String(memoryStatus.getPercentUsed()));
  html.replace("{{VAL2}}", String(memoryStatus.getPercentUsed()));
  html.replace("{{1}}", String(memoryStatus.getUsedAsKb()));
  html.replace("{{2}}", String(memoryStatus.getTotalAsKb()));
  returnHtml += html;


  html = Storage::readFile("/templates/status/network.html");
  html.replace("{{IP}}", networkInfo.ip);
  html.replace("{{MASK}}", networkInfo.mask);
  html.replace("{{GATEWAY}}", networkInfo.gateway);
  html.replace("{{DNS}}", networkInfo.dns);
  html.replace("{{MAC}}", networkInfo.mac);
  html.replace("{{NETWORK}}", networkInfo.ssid); //name
  html.replace("{{SIGNAL}}", String(networkInfo.signal)); //rssi
  html.replace("{{DISTANCE}}", String(networkInfo.getDistance())); //Estimated (very estimated)

  returnHtml += html;
  String tpl = TemplateManager::getMainTemplate("System Status", returnHtml);
  return tpl;
}


String TemplateManager::getTextEditor(String path, String content) {
  String html = Storage::readFile("/editor.html");
  html.replace("{{FILE_NAME}}", path);
  html.replace("{{TEXT}}", Utils::escapeHTML(content));

  String fullPage = TemplateManager::getMainTemplate(path, html);
  fullPage.replace("<!-- right_icons -->", "<a onclick='save()' href='#'><svg><use href='#save' /></svg></a>");
  return fullPage;
}

String TemplateManager::getCreateNetwork() {
  return TemplateManager::getMainTemplate("Create Network", Storage::readFile("/create_network.html"));
}

String TemplateManager::getBluetoothList() {
  String details = Storage::readFile("/templates/bluetooth/bluetooth.html");
  return TemplateManager::getMainTemplate("Bluetooth", details);
}

String TemplateManager::getIndex(const std::vector<MenuItem>& menuItems) {
  String menu = "<div class='list'>";
  for (const auto& item : menuItems) {
    menu += item.toString();
  }
  menu += "</div>";

  return TemplateManager::getMainTemplate("ESP32 Toolkit", menu);
}

String TemplateManager::getNetworkMenu(NetworkInfo networkInfo) {
  String html = Storage::readFile("/templates/network/network.html");

  html.replace("{{IP}}", networkInfo.ip);
  html.replace("{{MASK}}", networkInfo.mask);
  html.replace("{{GATEWAY}}", networkInfo.gateway);
  html.replace("{{DNS}}", networkInfo.dns);
  html.replace("{{MAC}}", networkInfo.mac);
  html.replace("{{NETWORK}}", networkInfo.ssid); //name
  html.replace("{{SIGNAL}}", String(networkInfo.signal)); //rssi
  html.replace("{{DISTANCE}}", String(networkInfo.getDistance())); //Estimated (very estimated)

  return TemplateManager::getMainTemplate("Network", html);
}