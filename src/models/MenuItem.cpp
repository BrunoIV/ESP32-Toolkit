#include "models/MenuItem.h"
#include <map>

MenuItem::MenuItem(String url, String name, String icon, 
                   std::map<String, String> extraAttributes)
  : url(url), name(name), icon(icon), extraAttributes(extraAttributes) {
}

String MenuItem::getUrl() const { return url; }
String MenuItem::getName() const { return name; }
String MenuItem::getIcon() const { return icon; }
std::map<String, String> MenuItem::getExtraAttributes() const { return extraAttributes; }

String MenuItem::toString() const {
  String html = "<a href='" + url + "'";

  for (const auto& entry : extraAttributes) {
    html += entry.first + "='" + entry.second + "'";
  }

  html += ">";
  html += "<svg><use href='#" + icon + "' /></svg>" + name + "<i>❯</i></a>";

  return html;
}