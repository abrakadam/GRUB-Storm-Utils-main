#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

// Простая Windows версия GRUB Storm Utils без зависимостей

std::string get_system_info() {
    std::string info = "=== Информация о системе ===\n\n";
    
    // пробуем получить информацию о системе через системные команды
    info += "Операционная система: Windows\n";
    
    // версия системы через systeminfo
    FILE* pipe = popen("systeminfo | findstr /B /C:\"OS Name\" /C:\"OS Version\" /C:\"System Type\" 2>nul", "r");
    if (pipe) {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            info += buffer;
        }
        pclose(pipe);
    }
    
    // информация о процессоре
    pipe = popen("wmic cpu get name 2>nul | findstr /v \"Name\"", "r");
    if (pipe) {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            std::string line = buffer;
            if (!line.empty() && line.find("Name") == std::string::npos) {
                info += "Процессор: " + line;
                break;
            }
        }
        pclose(pipe);
    }
    
    // информация о памяти
    pipe = popen("wmic computersystem get totalphysicalmemory 2>nul | findstr /v \"TotalPhysicalMemory\"", "r");
    if (pipe) {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            std::string line = buffer;
            if (!line.empty() && line.find("TotalPhysicalMemory") == std::string::npos) {
                try {
                    unsigned long long memory = std::stoull(line);
                    double gb = memory / (1024.0 * 1024.0 * 1024.0);
                    info += "Оперативная память: " + std::to_string((int)gb) + " GB\n";
                } catch (...) {
                    info += "Оперативная память: " + line;
                }
                break;
            }
        }
        pclose(pipe);
    }
    
    // информация о дисках
    info += "\n=== Диски ===\n";
    pipe = popen("wmic logicaldisk get size,freespace,caption 2>nul", "r");
    if (pipe) {
        char buffer[256];
        bool first = true;
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            std::string line = buffer;
            if (first) {
                first = false;
                continue;
            }
            if (!line.empty()) {
                info += line;
            }
        }
        pclose(pipe);
    }
    
    return info;
}

std::string get_grub_info() {
    std::string info = "=== Информация о GRUB ===\n\n";
    info += "GRUB (GRand Unified Bootloader) - это загрузчик для Linux/Unix систем\n\n";
    info += "На Windows GRUB обычно не устанавливается,\n";
    info += "но эта программа поможет подготовить систему:\n\n";
    info += "1. Проверить информацию о системе\n";
    info += "2. Оценить свободное место на дисках\n";
    info += "3. Получить рекомендации по установке Linux\n\n";
    info += "Для полного управления GRUB используйте Linux версию программы.\n";
    return info;
}

std::string get_linux_help() {
    std::string help = "=== Помощь по установке Linux ===\n\n";
    help += "Подготовка к установке Linux рядом с Windows:\n\n";
    help += "1. СОЗДАНИЕ РЕЗЕРВНОЙ КОПИИ\n";
    help += "   - Создайте точку восстановления Windows\n";
    help += "   - Сделайте резервную копию важных файлов\n\n";
    help += "2. ПОДГОТОВКА ДИСКА\n";
    help += "   - Освободите минимум 20-30GB места\n";
    help += "   - Используйте Управление дисками Windows\n";
    help += "   - Создайте неразмеченный раздел\n\n";
    help += "3. ВЫБОР ДИСТРИБУТИВА\n";
    help += "   Рекомендуемые для начинающих:\n";
    help += "   - Ubuntu LTS (стабильная)\n";
    help += "   - Linux Mint (удобная)\n";
    help += "   - Fedora (современная)\n\n";
    help += "4. СОЗДАНИЕ ЗАГРУЗОЧНОЙ ФЛЕШКИ\n";
    help += "   - Скачайте Rufus или balenaEtcher\n";
    help += "   - Скачайте ISO образ дистрибутива\n";
    help += "   - Создайте загрузочную USB флешку\n\n";
    help += "5. УСТАНОВКА LINUX\n";
    help += "   - Загрузитесь с USB флешки\n";
    help += "   - Выберите \"Установить рядом с Windows\"\n";
    help += "   - GRUB установится автоматически\n\n";
    help += "6. ПОСЛЕ УСТАНОВКИ\n";
    help += "   - При загрузке появится меню GRUB\n";
    help += "   - Выбирайте ОС из меню\n";
    help += "   - GRUB автоматически настроит двойную загрузку\n\n";
    help += "ВОПРОСЫ И ПРОБЛЕМЫ:\n";
    help += "   - GRUB не видит Windows: sudo update-grub\n";
    help += "   - Загрузка только в Linux: проверьте BIOS/UEFI\n";
    help += "   - Удаление Linux: используйте Boot-Repair\n";
    return help;
}

void show_menu() {
    std::cout << "\n========================================\n";
    std::cout << "    GRUB Storm Utils - Windows версия\n";
    std::cout << "========================================\n\n";
    std::cout << "1. Информация о системе\n";
    std::cout << "2. Информация о GRUB\n";
    std::cout << "3. Проверка дисков\n";
    std::cout << "4. Помощь по установке Linux\n";
    std::cout << "5. Выход\n\n";
    std::cout << "Выберите пункт меню: ";
}

void clear_screen() {
    system("cls");
}

void pause_screen() {
    std::cout << "\nНажмите Enter для продолжения...";
    std::cin.ignore();
    std::cin.get();
}

int main() {
    // настройка консоли
    system("title GRUB Storm Utils - Windows");
    system("color 0B");
    
    int choice;
    bool running = true;
    
    while (running) {
        clear_screen();
        show_menu();
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1:
                clear_screen();
                std::cout << get_system_info() << "\n";
                pause_screen();
                break;
            case 2:
                clear_screen();
                std::cout << get_grub_info() << "\n";
                pause_screen();
                break;
            case 3:
                clear_screen();
                std::cout << "=== Информация о дисках ===\n\n";
                system("wmic logicaldisk get size,freespace,caption");
                pause_screen();
                break;
            case 4:
                clear_screen();
                std::cout << get_linux_help() << "\n";
                pause_screen();
                break;
            case 5:
                running = false;
                break;
            default:
                std::cout << "\nНеверный выбор. Попробуйте снова.\n";
                system("timeout 2 >nul");
                break;
        }
    }
    
    clear_screen();
    std::cout << "Спасибо за использование GRUB Storm Utils!\n";
    std::cout << "Для полного управления GRUB используйте Linux версию.\n\n";
    
    return 0;
}
