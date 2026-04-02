#ifndef MENUITEM_H
#define MENUITEM_H

#include <Arduino.h>
#include <map>

class MenuItem {
  public:
    MenuItem(String url, String name, String icon, std::map<String, String> extraAttributes);

    String getUrl() const;
    String getName() const;
    String getIcon() const;
    std::map<String, String> getExtraAttributes() const;
    String toString() const;

  private:
    String url;
    String name;
    String icon;
    std::map<String, String> extraAttributes;
};

#endif
