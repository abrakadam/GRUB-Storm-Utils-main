#include "ui_gtk.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

int main(int argc, char* argv[]) {
    std::cout << "GRUB Storm Utils - GTK GUI\n";
    std::cout << "=============================\n\n";

    // проверяем наличие необходимых привилегий
#ifdef _WIN32
    std::cout << "Running on Windows platform.\n";
    std::cout << "Note: GRUB is Linux-specific. Limited functionality on Windows.\n\n";
#else
    if (getuid() != 0) {
        // предупреждаем, если нет прав root
        std::cout << "[WARNING] Not running as root (sudo).\n";
        std::cout << "Some operations may fail. Run with sudo for full functionality.\n\n";
    }
#endif

    try {
        // создаём менеджер grub и запускаем gtk-интерфейс
        GRUBManager manager;
        GTKUI ui(manager);
        ui.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
