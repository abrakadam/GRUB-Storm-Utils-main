#ifndef UI_GTK_H
#define UI_GTK_H

#include <gtk/gtk.h>
#include "grub_manager.h"
#include "system_info.h"

// графический интерфейс на основе gtk3
class GTKUI {
private:
    GRUBManager& manager;  // ссылка на менеджер grub
    SystemInfo sys_info;   // кэшированная информация о системе

    // главное окно приложения
    GtkWidget* window;
    GtkWidget* notebook;   // виджет вкладок (notebook)

    // виджеты вкладки «system info»
    GtkWidget* os_label;
    GtkWidget* kernel_label;
    GtkWidget* arch_label;
    GtkWidget* distro_label;
    GtkWidget* cpu_label;
    GtkWidget* memory_label;
    GtkWidget* user_label;
    GtkWidget* mode_label;  // метка текущего режима безопасности

    // виджеты вкладки «grub info»
    GtkWidget* grub_version_label;
    GtkWidget* grub_installed_label;
    GtkWidget* default_entry_label;
    GtkWidget* entries_count_label;
    GtkWidget* grub_config_text;   // текстовая область с конфигурацией

    // виджеты вкладки «entries» (таблица записей)
    GtkWidget* entries_list;
    GtkListStore* entries_store;

    // виджеты встроенного терминала
    GtkWidget* terminal_output;
    GtkWidget* terminal_input;

    // виджеты вкладки «operations»
    GtkWidget* default_entry_combo;
    GtkWidget* new_entry_title;
    GtkWidget* new_entry_kernel;
    GtkWidget* new_entry_initrd;
    GtkWidget* timeout_spin;       // спиннер таймаута загрузки

    // виджеты вкладки «modes» (выбор режима безопасности)
    GtkWidget* mode_safe_radio;
    GtkWidget* mode_dangerous_radio;
    GtkWidget* modes_info_label;
    GtkWidget* modes_operations_frame;

    // виджеты расширенной настройки grub
    GtkWidget* kernel_params_entry;   // параметры ядра
    GtkWidget* gfxmode_entry;         // графический режим
    GtkWidget* resolution_entry;      // разрешение экрана
    GtkWidget* splash_check;          // сплэш-экран
    GtkWidget* quiet_check;           // тихая загрузка

    // виджет вкладки «help»
    GtkWidget* help_text;

    int operations_tab_index;  // индекс вкладки operations (для блокировки в safe mode)

    // статические callback-функции gtk
    static void on_refresh_entries(GtkWidget* widget, gpointer data);
    static gboolean on_page_switch(GtkWidget* widget, GtkWidget* page, guint page_num, gpointer data);
    static void on_toggle_mode(GtkWidget* widget, gpointer data);
    static void on_mode_safe_toggled(GtkWidget* widget, gpointer data);
    static void on_mode_dangerous_toggled(GtkWidget* widget, gpointer data);
    static void on_execute_command(GtkWidget* widget, gpointer data);
    static void on_set_default(GtkWidget* widget, gpointer data);
    static void on_set_timeout(GtkWidget* widget, gpointer data);
    static void on_add_entry(GtkWidget* widget, gpointer data);
    static void on_update_grub(GtkWidget* widget, gpointer data);
    static void on_destroy(GtkWidget* widget, gpointer data);

    // методы построения вкладок
    void setup_system_info_tab();
    void setup_grub_info_tab();
    void setup_entries_tab();
    void setup_terminal_tab();
    void setup_modes_tab();
    void setup_help_tab();
    void setup_operations_tab();
    void show_startup_warning();   // предупреждение при запуске

    // методы обновления данных в виджетах
    void update_system_info();
    void update_grub_info();
    void update_entries_list();
    void refresh_all();  // обновить все вкладки сразу

public:
    GTKUI(GRUBManager& mgr);
    void run();  // запуск gtk main loop
};

#endif // UI_GTK_H
