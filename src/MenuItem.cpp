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
    return "<a href='" + url + "'><svg><use href='#"+ icon +"' /></svg>" + name + "<i class='fa-solid fa-chevron-right'></i></a>";
  } else {
    String output = "<div><label for='check_" + name + "'><svg><use href='#" + icon + "' /></svg>" + name + "</label><input autocomplete='off' class='hidden' type='checkbox' id='check_" + name + "'><div>";

    for (const auto& subIt : subItems) {  
      output += subIt.toString();
    }

    output += "</div></div>";
    return output;
  }
}


String MenuItem::getAsDropdownMenu() const {
  
  if(subItems.empty()) {
    return "<a href='" + url + "'>" + name + "</a>";
  } else {
    String checkbox = "<input class='hidden' type='checkbox' id='dropdown_" + name + "'>";
    String output = "<div><a href='" + url + "'><q class='"+ icon +"'></q>" + name + "<label for='dropdown_"+ name +"' class='fa-solid fa-ellipsis-vertical'></label></a>" + checkbox + "<div class='dropdown'>";

    for (const auto& subIt : subItems) {  
      output += subIt.getAsDropdownMenu();
    }

    output += "</div></div>"; 
    return output;
  }
}

