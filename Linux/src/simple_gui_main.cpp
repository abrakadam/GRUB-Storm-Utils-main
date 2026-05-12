#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>

// простая псевдо-графическая версия с меню
void show_menu() {
    std::cout << "\n=== GRUB Storm Utils - Меню ===\n\n";
    std::cout << "1. Информация о системе\n";
    std::cout << "2. Информация о GRUB\n";
    std::cout << "3. Записи загрузки\n";
    std::cout << "4. Терминал\n";
    std::cout << "5. Выход\n\n";
    std::cout << "Выберите пункт меню: ";
}

void show_system_info() {
    std::cout << "\n=== Информация о системе ===\n\n";
    
    // информация о ядре
    std::ifstream version_file("/proc/version");
    if (version_file.is_open()) {
        std::string version;
        std::getline(version_file, version);
        std::cout << "Ядро: " << version << "\n";
        version_file.close();
    }
    
    // информация о дистрибутиве
    std::ifstream os_release("/etc/os-release");
    if (os_release.is_open()) {
        std::string line;
        while (std::getline(os_release, line)) {
            if (line.find("PRETTY_NAME") != std::string::npos) {
                size_t start = line.find("=") + 2;
                std::string distro = line.substr(start);
                distro = distro.substr(0, distro.length() - 1);
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
    
    // информация о cpu
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
    
    std::cout << "\nНажмите Enter для продолжения...";
    std::cin.ignore();
    std::cin.get();
}

void show_grub_info() {
    std::cout << "\n=== Информация о GRUB ===\n\n";
    
    // проверка grub
    std::ifstream grub_cfg("/boot/grub/grub.cfg");
    if (grub_cfg.is_open()) {
        std::cout << "GRUB установлен: /boot/grub/grub.cfg найден\n";
        
        // считаем записи
        std::string line;
        int entry_count = 0;
        while (std::getline(grub_cfg, line)) {
            if (line.find("menuentry") != std::string::npos) {
                entry_count++;
            }
        }
        std::cout << "Количество записей: " << entry_count << "\n";
        grub_cfg.close();
    } else {
        std::cout << "GRUB не найден или нет прав доступа\n";
    }
    
    // версия grub
    FILE* pipe = popen("grub-install --version 2>/dev/null", "r");
    if (pipe) {
        char buffer[128];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            std::cout << "Версия GRUB: " << buffer;
        }
        pclose(pipe);
    }
    
    // конфигурация
    std::ifstream default_grub("/etc/default/grub");
    if (default_grub.is_open()) {
        std::cout << "\nНастройки /etc/default/grub:\n";
        std::string line;
        while (std::getline(default_grub, line)) {
            if (!line.empty() && line[0] != '#') {
                std::cout << "  " << line << "\n";
            }
        }
        default_grub.close();
    }
    
    std::cout << "\nНажмите Enter для продолжения...";
    std::cin.ignore();
    std::cin.get();
}

void show_entries() {
    std::cout << "\n=== Записи загрузки GRUB ===\n\n";
    
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
    } else {
        std::cout << "Не удалось прочитать /boot/grub/grub.cfg\n";
    }
    
    std::cout << "\nНажмите Enter для продолжения...";
    std::cin.ignore();
    std::cin.get();
}

void show_terminal() {
    std::cout << "\n=== Терминал GRUB Storm Utils ===\n";
    std::cout << "Доступные команды: sysinfo, help, grub-info, entries, exit\n\n";
    
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
            std::cout << "  exit - выход из терминала\n";
        } else if (command == "sysinfo") {
            std::cout << "--- Системная информация ---\n";
            std::ifstream loadavg("/proc/loadavg");
            if (loadavg.is_open()) {
                std::string load;
                std::getline(loadavg, load);
                std::cout << "Загрузка: " << load.substr(0, load.find(" ")) << "\n";
                loadavg.close();
            }
        } else if (command == "grub-info") {
            std::cout << "--- Информация о GRUB ---\n";
            std::cout << "Используйте пункт меню 2 для детальной информации\n";
        } else if (command == "entries") {
            std::cout << "--- Записи загрузки ---\n";
            std::cout << "Используйте пункт меню 3 для детальной информации\n";
        } else if (!command.empty()) {
            std::cout << "Неизвестная команда: " << command << "\n";
            std::cout << "Введите 'help' для списка команд\n";
        }
    }
}

int main() {
    std::cout << "========================================\n";
    std::cout << "    GRUB Storm Utils - GUI версия\n";
    std::cout << "========================================\n";
    
    // проверка прав доступа
    if (getuid() != 0) {
        std::cout << "\n[ПРЕДУПРЕЖДЕНИЕ] Запущено без прав root\n";
        std::cout << "Для полной функциональности используйте: sudo ./GRUBStormGUI\n\n";
    }
    
    int choice;
    while (true) {
        show_menu();
        std::cin >> choice;
        std::cin.ignore(); // очищаем буфер
        
        switch (choice) {
            case 1:
                show_system_info();
                break;
            case 2:
                show_grub_info();
                break;
            case 3:
                show_entries();
                break;
            case 4:
                show_terminal();
                break;
            case 5:
                std::cout << "\nСпасибо за использование GRUB Storm Utils!\n";
                return 0;
            default:
                std::cout << "\nНеверный выбор. Попробуйте снова.\n";
                break;
        }
    }
    
    return 0;
}
