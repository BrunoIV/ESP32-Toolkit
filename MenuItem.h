#ifndef MENUITEM_H
#define MENUITEM_H

#include <Arduino.h>
#include <vector>

class MenuItem {
  public:
    MenuItem(String category, String url, String name, String icon, const std::vector<MenuItem>& subItems);

    String getCategory() const;
    String getUrl() const;
    String getName() const;
    String getIcon() const;
    const std::vector<MenuItem>& getSubItems() const;
    String toString() const;

  private:
    String category;
    String url;
    String name;
    String icon;
    std::vector<MenuItem> subItems;
};

#endif
