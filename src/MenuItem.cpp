#include "MenuItem.h"


MenuItem::MenuItem(String category, String url, String name, String icon, const std::vector<MenuItem>& subItems)
  : category(category), url(url), name(name), icon(icon), subItems(subItems) {
}

String MenuItem::getCategory() const { return category; }
String MenuItem::getUrl() const { return url; }
String MenuItem::getName() const { return name; }
String MenuItem::getIcon() const { return icon; }
const std::vector<MenuItem>& MenuItem::getSubItems() const {
  return subItems;
}


String MenuItem::toString() const {
  
  if(subItems.empty()) {
    return "<li><a href='" + url + "'><q class='"+ icon +"'></q>" + name + "</label><i class='fa-solid fa-chevron-right'></i></a></li>";
  } else {
    String output = "<li><label for='check_" + name + "'><q class='"+ icon +"'></q>" + name + "</label><input autocomplete='off' type='checkbox' id='check_" + name + "'><i class='fa-solid fa-chevron-up'></i><i class='fa-solid fa-chevron-down'></i><ul>";

    for (const auto& subIt : subItems) {  
      output += subIt.toString();
    }

    output += "</ul>";
    return output;
  }
}
