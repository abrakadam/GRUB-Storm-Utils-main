#include "grub_manager.h"
#include "ui_advanced.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

int main() {
    std::cout << "GRUB Storm Utils - Advanced GRUB Manager\n";
    std::cout << "==========================================\n\n";

    GRUBManager manager;

    // проверяем наличие необходимых привилегий
#ifdef _WIN32
    // на windows пробуем запуск без прав администратора (только чтение)
    std::cout << "Running on Windows platform.\n";
    std::cout << "Note: GRUB is Linux-specific. Limited functionality on Windows.\n\n";
#else
    if (getuid() != 0) {
        std::cout << "[WARNING] Not running as root (sudo).\n";
        std::cout << "Some operations may fail. Run with sudo for full functionality.\n\n";
    }
#endif

    std::cout << "Starting Advanced Interface...\n";
    std::cout << "Press Enter to continue...";
    std::cin.get();

    try {
        // запускаем расширенный текстовый интерфейс
        AdvancedUI ui(manager);
        ui.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
