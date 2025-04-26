#ifndef MENUITEM_H
#define MENUITEM_H

#include <Arduino.h>

class MenuItem {
  public:
    MenuItem(String category, String url, String name, String icon);
    String getCategory() const;
    String getUrl() const;
    String getName() const;
    String getIcon() const;

  private:
    String category;
    String url;
    String name;
    String icon;
};

#endif
