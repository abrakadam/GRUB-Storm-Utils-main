#ifndef UI_H
#define UI_H

#include <string>
#include "grub_manager.h"

// базовый консольный интерфейс управления grub
class UI {
private:
    GRUBManager& manager;  // ссылка на менеджер grub

    // методы отображения заголовка и меню
    void print_header();
    void print_menu();
    void print_safe_menu();      // меню в безопасном режиме (только чтение)
    void print_dangerous_menu(); // меню в опасном режиме (запись разрешена)
    void wait_for_key();         // пауза до нажатия enter

    // обработчики действий пользователя
    void view_entries();
    void view_config();
    void view_default();
    void change_default();
    void add_entry_ui();
    void remove_entry_ui();
    void change_timeout();
    void edit_config();
    void update_grub_ui();
    void change_mode();  // переключение между safe и dangerous

public:
    UI(GRUBManager& mgr);

    void run();  // запуск основного цикла интерфейса
};

#endif // UI_H
