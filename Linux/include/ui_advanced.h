#ifndef UI_ADVANCED_H
#define UI_ADVANCED_H

#include <string>
#include <vector>
#include "grub_manager.h"
#include "system_info.h"

// расширенный консольный интерфейс с поддержкой системной информации и встроенного терминала
class AdvancedUI {
private:
    GRUBManager& manager;  // ссылка на менеджер grub
    SystemInfo sys_info;   // кэшированная информация о системе

    // методы отображения
    void clear_screen();
    void print_header();
    void print_separator();
    void print_menu();
    void print_system_info();
    void print_grub_info();
    void print_entries_detailed();
    void print_terminal_help();  // справка по командам терминала

    // обработчики пунктов главного меню
    void view_system_info();
    void view_grub_entries();
    void view_entry_details();
    void view_grub_config();
    void view_default_entry();
    void terminal_mode();      // встроенный командный терминал
    void grub_operations();    // операции с grub (только в режиме dangerous)
    void change_safety_mode(); // переключение режима безопасности

    // выполнение команды во встроенном терминале
    void execute_terminal_command(const std::string& cmd);

public:
    AdvancedUI(GRUBManager& mgr);
    void run();  // запуск основного цикла
};

#endif // UI_ADVANCED_H
