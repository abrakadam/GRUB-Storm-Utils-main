#include <fstream>
#include <sstream>
#include <vector>
#include <string>

// класс для управления темами grub
class grub_theme {
private:
    std::string theme_path;
    
public:
    grub_theme() : theme_path("/boot/grub/themes/") {}
    
    // получить список доступных тем
    std::vector<std::string> get_available_themes() {
        std::vector<std::string> themes;
        // здесь должна быть реализация сканирования директории тем
        // для примера добавим несколько популярных тем
        themes.push_back("default");
        themes.push_back("starfield");
        themes.push_back("dark");
        themes.push_back("cyber");
        return themes;
    }
    
    // применить тему
    bool apply_theme(const std::string& theme_name) {
        std::string theme_file = theme_path + theme_name + "/theme.txt";
        std::ifstream file(theme_file);
        
        if (!file.is_open()) {
            return false;
        }
        
        // читаем и применяем настройки темы
        std::string line;
        while (std::getline(file, line)) {
            // обработка настроек темы
            if (line.find("desktop-image:") != std::string::npos) {
                // установка фонового изображения
            }
            else if (line.find("desktop-color:") != std::string::npos) {
                // установка цвета фона
            }
            else if (line.find("terminal-font:") != std::string::npos) {
                // установка шрифта терминала
            }
        }
        
        file.close();
        return true;
    }
    
    // создать кастомную тему
    bool create_custom_theme(const std::string& theme_name, 
                           const std::string& bg_color,
                           const std::string& text_color) {
        std::string theme_dir = theme_path + theme_name;
        std::string theme_file = theme_dir + "/theme.txt";
        
        // создаём директорию темы
        std::string command = "mkdir -p " + theme_dir;
        system(command.c_str());
        
        // создаём файл темы
        std::ofstream file(theme_file);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# custom theme: " << theme_name << "\n";
        file << "desktop-color: " << bg_color << "\n";
        file << "terminal-font: \"Unifont Regular 16\"\n";
        file << "terminal-left: 0\n";
        file << "terminal-top: 0\n";
        file << "terminal-width: 100%\n";
        file << "terminal-height: 100%\n";
        file << "terminal-border: 0\n";
        
        file.close();
        return true;
    }
    
    // получить информацию о текущей теме
    std::string get_current_theme() {
        std::ifstream grub_cfg("/boot/grub/grub.cfg");
        std::string line;
        
        while (std::getline(grub_cfg, line)) {
            if (line.find("set theme=") != std::string::npos) {
                size_t start = line.find("$prefix/") + 8;
                size_t end = line.find("/theme.txt");
                if (start != std::string::npos && end != std::string::npos) {
                    return line.substr(start, end - start);
                }
            }
        }
        
        return "default";
    }
};
