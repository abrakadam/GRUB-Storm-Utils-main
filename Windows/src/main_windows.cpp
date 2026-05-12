#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
#include <shlobj.h>

// Windows версия GRUB Storm Utils

// функции для получения информации
std::string get_windows_info() {
    std::string info = "=== Информация о системе Windows ===\n\n";
    
    // версия Windows
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    
    if (GetVersionEx((OSVERSIONINFO*)&osvi)) {
        info += "Версия Windows: " + std::to_string(osvi.dwMajorVersion) + "." + 
                std::to_string(osvi.dwMinorVersion) + " Build " + 
                std::to_string(osvi.dwBuildNumber) + "\n";
    }
    
    // информация о системе
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    info += "Архитектура: ";
    if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) {
        info += "x64\n";
    } else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL) {
        info += "x86\n";
    } else {
        info += "Другая\n";
    }
    
    info += "Процессоры: " + std::to_string(si.dwNumberOfProcessors) + "\n";
    
    // память
    MEMORYSTATUSEX ms;
    ms.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&ms)) {
        info += "Всего памяти: " + std::to_string(ms.ullTotalPhys / (1024 * 1024 * 1024)) + " GB\n";
        info += "Доступно памяти: " + std::to_string(ms.ullAvailPhys / (1024 * 1024 * 1024)) + " GB\n";
    }
    
    // имя компьютера
    char computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(computerName);
    if (GetComputerName(computerName, &size)) {
        info += "Имя компьютера: " + std::string(computerName) + "\n";
    }
    
    // имя пользователя
    char userName[UNLEN + 1];
    DWORD userSize = sizeof(userName);
    if (GetUserName(userName, &userSize)) {
        info += "Пользователь: " + std::string(userName) + "\n";
    }
    
    return info;
}

std::string get_grub_info() {
    std::string info = "=== Информация о GRUB ===\n\n";
    info += "GRUB является загрузчиком Linux/Unix систем\n";
    info += "На Windows GRUB обычно не используется\n";
    info += "Эта версия программы предназначена для:\n";
    info += "- Информации о системе Windows\n";
    info += "- Подготовки к установке Linux\n";
    info += "- Управления двойной загрузкой\n\n";
    info += "Для полного управления GRUB используйте Linux версию\n";
    return info;
}

void show_windows_console() {
    SetConsoleOutputCP(CP_UTF8);
    
    while (true) {
        system("cls");
        std::cout << "========================================\n";
        std::cout << "    GRUB Storm Utils - Windows версия\n";
        std::cout << "========================================\n\n";
        std::cout << "1. Информация о системе Windows\n";
        std::cout << "2. Информация о GRUB\n";
        std::cout << "3. Проверка дисков\n";
        std::cout << "4. Помощь по установке Linux\n";
        std::cout << "5. Выход\n\n";
        std::cout << "Выберите пункт меню: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1: {
                system("cls");
                std::cout << get_windows_info() << "\n";
                std::cout << "Нажмите Enter для продолжения...";
                std::cin.get();
                break;
            }
            case 2: {
                system("cls");
                std::cout << get_grub_info() << "\n";
                std::cout << "Нажмите Enter для продолжения...";
                std::cin.get();
                break;
            }
            case 3: {
                system("cls");
                std::cout << "=== Информация о дисках ===\n\n";
                system("wmic logicaldisk get size,freespace,caption");
                std::cout << "\nНажмите Enter для продолжения...";
                std::cin.get();
                break;
            }
            case 4: {
                system("cls");
                std::cout << "=== Помощь по установке Linux ===\n\n";
                std::cout << "1. Создайте резервную копию Windows\n";
                std::cout << "2. Освободите место на диске (минимум 20GB)\n";
                std::cout << "3. Скачайте дистрибутив Linux\n";
                std::cout << "4. Создайте загрузочную USB флешку\n";
                std::cout << "5. Установите Linux рядом с Windows\n";
                std::cout << "6. GRUB будет установлен автоматически\n";
                std::cout << "7. При загрузке выбирайте ОС в меню GRUB\n\n";
                std::cout << "Рекомендуемые дистрибутивы:\n";
                std::cout << "- Ubuntu (для начинающих)\n";
                std::cout << "- Fedora (современный)\n";
                std::cout << "- Debian (стабильный)\n";
                std::cout << "- Arch Linux (для продвинутых)\n\n";
                std::cout << "Нажмите Enter для продолжения...";
                std::cin.get();
                break;
            }
            case 5:
                return;
            default:
                std::cout << "\nНеверный выбор. Попробуйте снова.\n";
                Sleep(1000);
                break;
        }
    }
}

int main() {
    // настройка консоли
    SetConsoleTitle("GRUB Storm Utils - Windows");
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    
    show_windows_console();
    
    return 0;
}
