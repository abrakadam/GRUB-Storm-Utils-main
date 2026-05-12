#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <unistd.h>

// функции для получения информации
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
    
    // конфигурация
    std::ifstream default_grub("/etc/default/grub");
    if (default_grub.is_open()) {
        info += "\nНастройки /etc/default/grub:\n";
        std::string line;
        while (std::getline(default_grub, line)) {
            if (!line.empty() && line[0] != '#') {
                info += "  " + line + "\n";
            }
        }
        default_grub.close();
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

void show_zenity_dialog(const std::string& title, const std::string& text, const std::string& type = "info") {
    std::string command = "zenity --" + type + " --title=\"" + title + "\" --text=\"" + text + "\" --width=600 --height=400";
    system(command.c_str());
}

std::string show_zenity_menu() {
    std::string command = "zenity --list --title=\"GRUB Storm Utils\" --text=\"Выберите действие:\" --column=\"Действие\" \"Информация о системе\" \"Информация о GRUB\" \"Записи загрузки\" \"Терминал\" \"Выход\" --width=400 --height=300";
    
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return "exit";
    
    char buffer[128];
    std::string result = "";
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result = buffer;
        // удаляем перенос строки
        if (!result.empty() && result.back() == '\n') {
            result.pop_back();
        }
    }
    pclose(pipe);
    
    return result;
}

std::string show_zenity_input(const std::string& prompt) {
    std::string command = "zenity --entry --title=\"Терминал\" --text=\"" + prompt + "\" --width=400";
    
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return "";
    
    char buffer[256];
    std::string result = "";
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result = buffer;
        // удаляем перенос строки
        if (!result.empty() && result.back() == '\n') {
            result.pop_back();
        }
    }
    pclose(pipe);
    
    return result;
}

void run_terminal() {
    std::string command = "zenity --text-info --title=\"Терминал GRUB Storm Utils\" --width=800 --height=600 --editable";
    system(command.c_str());
}

int main() {
    // проверка прав доступа
    if (getuid() != 0) {
        show_zenity_dialog("GRUB Storm Utils - Предупреждение", 
                           "[ПРЕДУПРЕЖДЕНИЕ] Запущено без прав root\n\nДля полной функциональности используйте:\nsudo ./GRUBStormGUI", 
                           "warning");
    }
    
    while (true) {
        std::string choice = show_zenity_menu();
        
        if (choice == "exit" || choice.empty()) {
            break;
        } else if (choice == "Информация о системе") {
            std::string info = get_system_info();
            std::string temp_file = "/tmp/grub_storm_system_info.txt";
            std::ofstream temp(temp_file);
            temp << info;
            temp.close();
            
            std::string command = "zenity --text-info --title=\"Информация о системе\" --filename=\"" + temp_file + "\" --width=800 --height=600";
            system(command.c_str());
            
            remove(temp_file.c_str());
        } else if (choice == "Информация о GRUB") {
            std::string info = get_grub_info();
            std::string temp_file = "/tmp/grub_storm_grub_info.txt";
            std::ofstream temp(temp_file);
            temp << info;
            temp.close();
            
            std::string command = "zenity --text-info --title=\"Информация о GRUB\" --filename=\"" + temp_file + "\" --width=800 --height=600";
            system(command.c_str());
            
            remove(temp_file.c_str());
        } else if (choice == "Записи загрузки") {
            std::string info = get_grub_entries();
            std::string temp_file = "/tmp/grub_storm_entries.txt";
            std::ofstream temp(temp_file);
            temp << info;
            temp.close();
            
            std::string command = "zenity --text-info --title=\"Записи загрузки\" --filename=\"" + temp_file + "\" --width=800 --height=600";
            system(command.c_str());
            
            remove(temp_file.c_str());
        } else if (choice == "Терминал") {
            run_terminal();
        }
    }
    
    show_zenity_dialog("GRUB Storm Utils", "Спасибо за использование GRUB Storm Utils!", "info");
    return 0;
}
