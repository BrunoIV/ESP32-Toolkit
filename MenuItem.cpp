#include "MenuItem.h"


MenuItem::MenuItem(String category, String url, String name, String icon)
  : category(category), url(url), name(name), icon(icon) {
}

String MenuItem::getCategory() const { return category; }
String MenuItem::getUrl() const { return url; }
String MenuItem::getName() const { return name; }
String MenuItem::getIcon() const { return icon; }

