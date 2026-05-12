#ifndef GRUB_THEME_H
#define GRUB_THEME_H

#include <string>
#include <vector>

class grub_theme {
public:
    grub_theme();
    std::vector<std::string> get_available_themes();
    bool apply_theme(const std::string& theme_name);
    bool create_custom_theme(const std::string& theme_name, 
                           const std::string& bg_color,
                           const std::string& text_color);
    std::string get_current_theme();
};

#endif // GRUB_THEME_H
