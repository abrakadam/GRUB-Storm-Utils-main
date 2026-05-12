#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>

// простая демонстрационная версия
int main() {
    std::cout << "=== GRUB Storm Utils - Демонстрационная версия ===\n\n";
    
    // проверка прав доступа
    if (getuid() != 0) {
        std::cout << "[ПРЕДУПРЕЖДЕНИЕ] Запущено без прав root\n";
        std::cout << "Для полной функциональности используйте: sudo ./GRUBStormUtils\n\n";
    }
    
    // информация о системе
    std::cout << "--- Информация о системе ---\n";
    
    // читаем версию ядра
    std::ifstream version_file("/proc/version");
    if (version_file.is_open()) {
        std::string version;
        std::getline(version_file, version);
        std::cout << "Ядро: " << version << "\n";
        version_file.close();
    }
    
    // читаем информацию о дистрибутиве
    std::ifstream os_release("/etc/os-release");
    if (os_release.is_open()) {
        std::string line;
        while (std::getline(os_release, line)) {
            if (line.find("PRETTY_NAME") != std::string::npos) {
                size_t start = line.find("=") + 2;
                std::string distro = line.substr(start);
                distro = distro.substr(0, distro.length() - 1); // удаляем кавычки
                std::cout << "ОС: " << distro << "\n";
                break;
            }
        }
        os_release.close();
    }
    
    // информация о памяти
    std::ifstream meminfo("/proc/meminfo");
    if (meminfo.is_open()) {
        std::string line;
        while (std::getline(meminfo, line)) {
            if (line.find("MemTotal") != std::string::npos) {
                std::cout << line << "\n";
                break;
            }
        }
        meminfo.close();
    }
    
    std::cout << "\n--- Проверка GRUB ---\n";
    
    // проверка установки grub
    std::ifstream grub_cfg("/boot/grub/grub.cfg");
    if (grub_cfg.is_open()) {
        std::cout << "GRUB установлен: /boot/grub/grub.cfg найден\n";
        
        // считаем количество записей
        std::string line;
        int entry_count = 0;
        while (std::getline(grub_cfg, line)) {
            if (line.find("menuentry") != std::string::npos) {
                entry_count++;
            }
        }
        std::cout << "Количество записей загрузки: " << entry_count << "\n";
        grub_cfg.close();
    } else {
        std::cout << "GRUB не найден или нет прав доступа\n";
    }
    
    // проверка версии grub
    FILE* pipe = popen("grub-install --version 2>/dev/null", "r");
    if (pipe) {
        char buffer[128];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            std::cout << "Версия GRUB: " << buffer;
        }
        pclose(pipe);
    }
    
    std::cout << "\n--- Доступные команды ---\n";
    std::cout << "sysinfo - показать системную информацию\n";
    std::cout << "grub-info - показать информацию о GRUB\n";
    std::cout << "entries - показать записи загрузки\n";
    std::cout << "help - показать справку\n";
    std::cout << "exit - выход\n\n";
    
    // простой интерактивный режим
    std::string command;
    while (true) {
        std::cout << "grub-storm> ";
        std::getline(std::cin, command);
        
        if (command == "exit" || command == "quit") {
            break;
        } else if (command == "help") {
            std::cout << "Доступные команды:\n";
            std::cout << "  sysinfo - информация о системе\n";
            std::cout << "  grub-info - информация о GRUB\n";
            std::cout << "  entries - записи загрузки\n";
            std::cout << "  help - справка\n";
            std::cout << "  exit - выход\n";
        } else if (command == "sysinfo") {
            std::cout << "--- Полная системная информация ---\n";
            
            // cpu информация
            std::ifstream cpuinfo("/proc/cpuinfo");
            if (cpuinfo.is_open()) {
                std::string line;
                while (std::getline(cpuinfo, line)) {
                    if (line.find("model name") != std::string::npos) {
                        std::cout << "CPU: " << line.substr(line.find(":") + 2) << "\n";
                        break;
                    }
                }
                cpuinfo.close();
            }
            
            // загрузка системы
            std::ifstream loadavg("/proc/loadavg");
            if (loadavg.is_open()) {
                std::string load;
                std::getline(loadavg, load);
                std::cout << "Загрузка: " << load.substr(0, load.find(" ")) << "\n";
                loadavg.close();
            }
            
        } else if (command == "grub-info") {
            std::cout << "--- Информация о GRUB ---\n";
            
            // проверка конфигурации
            std::ifstream default_grub("/etc/default/grub");
            if (default_grub.is_open()) {
                std::string line;
                std::cout << "Настройки /etc/default/grub:\n";
                while (std::getline(default_grub, line)) {
                    if (!line.empty() && line[0] != '#') {
                        std::cout << "  " << line << "\n";
                    }
                }
                default_grub.close();
            }
            
        } else if (command == "entries") {
            std::cout << "--- Записи загрузки GRUB ---\n";
            
            std::ifstream grub_cfg("/boot/grub/grub.cfg");
            if (grub_cfg.is_open()) {
                std::string line;
                int entry_num = 0;
                while (std::getline(grub_cfg, line)) {
                    if (line.find("menuentry") != std::string::npos) {
                        size_t start = line.find("'") + 1;
                        size_t end = line.rfind("'");
                        if (start != std::string::npos && end != std::string::npos) {
                            std::cout << entry_num << ". " << line.substr(start, end - start) << "\n";
                            entry_num++;
                        }
                    }
                }
                grub_cfg.close();
            }
            
        } else if (!command.empty()) {
            std::cout << "Неизвестная команда: " << command << "\n";
            std::cout << "Введите 'help' для списка команд\n";
        }
    }
    
    std::cout << "\nСпасибо за использование GRUB Storm Utils!\n";
    return 0;
}
