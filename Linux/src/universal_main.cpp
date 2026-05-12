#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

// универсальная версия без зависимостей от GUI библиотек

// функции для определения доступных интерфейсов
bool has_gui_support() {
    // проверяем наличие X11
    if (getenv("DISPLAY") != nullptr) {
        return true;
    }
    return false;
}

bool has_zenity() {
    return system("which zenity > /dev/null 2>&1") == 0;
}

bool has_dialog() {
    return system("which dialog > /dev/null 2>&1") == 0;
}

// базовые функции для получения информации
std::string get_system_info() {
    std::string info = "=== Информация о системе ===\n\n";
    
    // информация о ядре
    std::ifstream version_file("/proc/version");
    if (version_file.is_open()) {
        std::string version;
        std::getline(version_file, version);
        info += "Ядро: " + version + "\n";
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
                info += "ОС: " + distro + "\n";
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
                info += line + "\n";
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
                info += "CPU: " + line.substr(line.find(":") + 2) + "\n";
                break;
            }
        }
        cpuinfo.close();
    }
    
    return info;
}

std::string get_grub_info() {
    std::string info = "=== Информация о GRUB ===\n\n";
    
    // проверка grub
    std::ifstream grub_cfg("/boot/grub/grub.cfg");
    if (grub_cfg.is_open()) {
        info += "GRUB установлен: /boot/grub/grub.cfg найден\n";
        
        // считаем записи
        std::string line;
        int entry_count = 0;
        while (std::getline(grub_cfg, line)) {
            if (line.find("menuentry") != std::string::npos) {
                entry_count++;
            }
        }
        info += "Количество записей: " + std::to_string(entry_count) + "\n";
        grub_cfg.close();
    } else {
        info += "GRUB не найден или нет прав доступа\n";
    }
    
    // версия grub
    FILE* pipe = popen("grub-install --version 2>/dev/null", "r");
    if (pipe) {
        char buffer[128];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            info += "Версия GRUB: " + std::string(buffer);
        }
        pclose(pipe);
    }
    
    return info;
}

std::string get_grub_entries() {
    std::string info = "=== Записи загрузки GRUB ===\n\n";
    
    std::ifstream grub_cfg("/boot/grub/grub.cfg");
    if (grub_cfg.is_open()) {
        std::string line;
        int entry_num = 0;
        while (std::getline(grub_cfg, line)) {
            if (line.find("menuentry") != std::string::npos) {
                size_t start = line.find("'") + 1;
                size_t end = line.rfind("'");
                if (start != std::string::npos && end != std::string::npos) {
                    info += std::to_string(entry_num) + ". " + line.substr(start, end - start) + "\n";
                    entry_num++;
                }
            }
        }
        grub_cfg.close();
    } else {
        info += "Не удалось прочитать /boot/grub/grub.cfg\n";
    }
    
    return info;
}

// функции для разных интерфейсов
void show_text_interface() {
    while (true) {
        std::cout << "\n=== GRUB Storm Utils - Универсальная версия ===\n\n";
        std::cout << "1. Информация о системе\n";
        std::cout << "2. Информация о GRUB\n";
        std::cout << "3. Записи загрузки\n";
        std::cout << "4. Терминал\n";
        std::cout << "5. Выход\n\n";
        std::cout << "Выберите пункт меню: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1:
                std::cout << "\n" << get_system_info() << "\n";
                std::cout << "Нажмите Enter для продолжения...";
                std::cin.get();
                break;
            case 2:
                std::cout << "\n" << get_grub_info() << "\n";
                std::cout << "Нажмите Enter для продолжения...";
                std::cin.get();
                break;
            case 3:
                std::cout << "\n" << get_grub_entries() << "\n";
                std::cout << "Нажмите Enter для продолжения...";
                std::cin.get();
                break;
            case 4: {
                std::cout << "\n=== Терминал ===\n";
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
                    } else if (!command.empty()) {
                        std::cout << "Неизвестная команда: " << command << "\n";
                    }
                }
                break;
            }
            case 5:
                return;
            default:
                std::cout << "\nНеверный выбор. Попробуйте снова.\n";
                break;
        }
    }
}

void show_zenity_interface() {
    while (true) {
        std::string command = "zenity --list --title=\"GRUB Storm Utils\" --text=\"Выберите действие:\" --column=\"Действие\" \"Информация о системе\" \"Информация о GRUB\" \"Записи загрузки\" \"Терминал\" \"Выход\" --width=400 --height=300 2>/dev/null";
        
        FILE* pipe = popen(command.c_str(), "r");
        if (!pipe) return;
        
        char buffer[128];
        std::string choice = "";
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            choice = buffer;
            if (!choice.empty() && choice.back() == '\n') {
                choice.pop_back();
            }
        }
        pclose(pipe);
        
        if (choice == "Выход" || choice.empty()) {
            break;
        } else if (choice == "Информация о системе") {
            std::string info = get_system_info();
            std::string temp_file = "/tmp/grub_storm_system_info.txt";
            std::ofstream temp(temp_file);
            temp << info;
            temp.close();
            
            std::string cmd = "zenity --text-info --title=\"Информация о системе\" --filename=\"" + temp_file + "\" --width=800 --height=600 2>/dev/null";
            system(cmd.c_str());
            remove(temp_file.c_str());
        } else if (choice == "Информация о GRUB") {
            std::string info = get_grub_info();
            std::string temp_file = "/tmp/grub_storm_grub_info.txt";
            std::ofstream temp(temp_file);
            temp << info;
            temp.close();
            
            std::string cmd = "zenity --text-info --title=\"Информация о GRUB\" --filename=\"" + temp_file + "\" --width=800 --height=600 2>/dev/null";
            system(cmd.c_str());
            remove(temp_file.c_str());
        } else if (choice == "Записи загрузки") {
            std::string info = get_grub_entries();
            std::string temp_file = "/tmp/grub_storm_entries.txt";
            std::ofstream temp(temp_file);
            temp << info;
            temp.close();
            
            std::string cmd = "zenity --text-info --title=\"Записи загрузки\" --filename=\"" + temp_file + "\" --width=800 --height=600 2>/dev/null";
            system(cmd.c_str());
            remove(temp_file.c_str());
        } else if (choice == "Терминал") {
            std::string cmd = "zenity --text-info --title=\"Терминал GRUB Storm Utils\" --width=800 --height=600 --editable 2>/dev/null";
            system(cmd.c_str());
        }
    }
}

void show_dialog_interface() {
    while (true) {
        std::string command = "dialog --menu \"GRUB Storm Utils - Выберите действие:\" 15 40 5 1 \"Информация о системе\" 2 \"Информация о GRUB\" 3 \"Записи загрузки\" 4 \"Терминал\" 5 \"Выход\" 2>/dev/tty";
        
        FILE* pipe = popen(command.c_str(), "r");
        if (!pipe) return;
        
        char buffer[128];
        std::string choice = "";
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            choice = buffer;
            if (!choice.empty() && choice.back() == '\n') {
                choice.pop_back();
            }
        }
        pclose(pipe);
        
        if (choice == "5" || choice.empty()) {
            break;
        } else if (choice == "1") {
            std::string info = get_system_info();
            std::string cmd = "dialog --msgbox \"" + info + "\" 20 60 2>/dev/tty";
            system(cmd.c_str());
        } else if (choice == "2") {
            std::string info = get_grub_info();
            std::string cmd = "dialog --msgbox \"" + info + "\" 20 60 2>/dev/tty";
            system(cmd.c_str());
        } else if (choice == "3") {
            std::string info = get_grub_entries();
            std::string cmd = "dialog --msgbox \"" + info + "\" 20 60 2>/dev/tty";
            system(cmd.c_str());
        } else if (choice == "4") {
            std::string cmd = "dialog --inputbox \"Терминал GRUB Storm Utils\\nВведите команду:\" 10 40 2>/dev/tty";
            system(cmd.c_str());
        }
    }
}

int main() {
    std::cout << "========================================\n";
    std::cout << "    GRUB Storm Utils - Универсальная версия\n";
    std::cout << "========================================\n\n";
    
    // проверка прав доступа
    if (getuid() != 0) {
        std::cout << "[ПРЕДУПРЕЖДЕНИЕ] Запущено без прав root\n";
        std::cout << "Для полной функциональности используйте: sudo ./GRUBStormUniversal\n\n";
    }
    
    // определяем лучший доступный интерфейс
    if (has_gui_support()) {
        if (has_zenity()) {
            std::cout << "Обнаружен Zenity, используется графический интерфейс...\n";
            show_zenity_interface();
        } else if (has_dialog()) {
            std::cout << "Обнаружен Dialog, используется текстовый графический интерфейс...\n";
            show_dialog_interface();
        } else {
            std::cout << "Графический интерфейс недоступен, используется текстовый режим...\n";
            show_text_interface();
        }
    } else {
        std::cout << "Графическая среда недоступна, используется текстовый режим...\n";
        show_text_interface();
    }
    
    std::cout << "\nСпасибо за использование GRUB Storm Utils!\n";
    return 0;
}
