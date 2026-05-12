#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

// класс для валидации конфигурации grub
class grub_validator {
private:
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
public:
    // проверить конфигурацию на ошибки
    bool validate_config(const std::string& config_path = "/boot/grub/grub.cfg") {
        errors.clear();
        warnings.clear();
        
        std::ifstream config_file(config_path);
        if (!config_file.is_open()) {
            errors.push_back("не удалось открыть файл конфигурации: " + config_path);
            return false;
        }
        
        std::string line;
        int line_number = 0;
        
        while (std::getline(config_file, line)) {
            line_number++;
            
            // проверка синтаксиса
            if (!validate_line_syntax(line)) {
                errors.push_back("синтаксическая ошибка в строке " + std::to_string(line_number) + ": " + line);
            }
            
            // проверка путей
            if (!validate_paths(line)) {
                warnings.push_back("возможная проблема с путём в строке " + std::to_string(line_number) + ": " + line);
            }
            
            // проверка ключевых параметров
            validate_key_parameters(line, line_number);
        }
        
        config_file.close();
        return errors.empty();
    }
    
    // проверить синтаксис отдельной строки
    bool validate_line_syntax(const std::string& line) {
        // пустые строки и комментарии всегда корректны
        if (line.empty() || line.substr(0, 1) == "#") {
            return true;
        }
        
        // проверка на сбалансированные кавычки
        int quote_count = std::count(line.begin(), line.end(), '"');
        if (quote_count % 2 != 0) {
            return false;
        }
        
        // проверка на корректные команды grub
        std::vector<std::string> valid_commands = {
            "set", "menuentry", "linux", "initrd", "chainloader",
            "boot", "configfile", "halt", "reboot", "exit",
            "search", "insmod", "terminal", "background_color",
            "gfxmode", "gfxpayload", "loadfont", "setfont"
        };
        
        // извлекаем первую команду из строки
        std::istringstream iss(line);
        std::string command;
        iss >> command;
        
        // удаляем возможные кавычки
        if (command.front() == '"' && command.back() == '"') {
            command = command.substr(1, command.length() - 2);
        }
        
        // проверяем, является ли команда валидной
        for (const auto& valid_cmd : valid_commands) {
            if (command == valid_cmd) {
                return true;
            }
        }
        
        return false;
    }
    
    // проверить пути в конфигурации
    bool validate_paths(const std::string& line) {
        // ищем пути в строке
        size_t pos = 0;
        while ((pos = line.find("/", pos)) != std::string::npos) {
            // извлекаем путь
            size_t end = line.find(" ", pos);
            std::string path = line.substr(pos, end - pos);
            
            // проверяем, не содержит ли путь недопустимых символов
            if (path.find("..") != std::string::npos) {
                return false;
            }
            
            pos = end + 1;
        }
        
        return true;
    }
    
    // проверить ключевые параметры
    void validate_key_parameters(const std::string& line, int line_number) {
        // проверка таймаута
        if (line.find("set timeout=") != std::string::npos) {
            size_t start = line.find("=") + 1;
            std::string timeout_str = line.substr(start);
            
            try {
                int timeout = std::stoi(timeout_str);
                if (timeout < 0 || timeout > 300) {
                    warnings.push_back("таймаут должен быть в диапазоне 0-300 секунд (строка " + std::to_string(line_number) + ")");
                }
            } catch (...) {
                errors.push_back("некорректное значение таймаута (строка " + std::to_string(line_number) + ")");
            }
        }
        
        // проверка режима графики
        if (line.find("set gfxmode=") != std::string::npos) {
            size_t start = line.find("=") + 1;
            std::string gfxmode = line.substr(start);
            
            // проверяем корректные разрешения
            std::vector<std::string> valid_modes = {
                "auto", "640x480", "800x600", "1024x768", "1280x1024", "1920x1080"
            };
            
            bool valid = false;
            for (const auto& mode : valid_modes) {
                if (gfxmode.find(mode) != std::string::npos) {
                    valid = true;
                    break;
                }
            }
            
            if (!valid) {
                warnings.push_back("нестандартное разрешение экрана (строка " + std::to_string(line_number) + ")");
            }
        }
    }
    
    // получить список ошибок
    std::vector<std::string> get_errors() const {
        return errors;
    }
    
    // получить список предупреждений
    std::vector<std::string> get_warnings() const {
        return warnings;
    }
    
    // исправить базовые ошибки
    bool fix_basic_errors(const std::string& config_path = "/boot/grub/grub.cfg") {
        // здесь может быть реализация автоисправления
        // пока просто возвращаем true
        return true;
    }
};
