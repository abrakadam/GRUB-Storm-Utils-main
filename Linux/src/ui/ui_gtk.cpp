#include "ui_gtk.h"
#include <sstream>

GTKUI::GTKUI(GRUBManager& mgr) : manager(mgr), operations_tab_index(-1) {
    sys_info = SystemInfoCollector::collect();
    
    // Initialize GTK
    gtk_init(NULL, NULL);
    
    // Create main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GRUB Storm Utils - Advanced GRUB Manager");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 700);
    g_signal_connect(window, "destroy", G_CALLBACK(on_destroy), this);
    
    // Create notebook (tabs)
    notebook = gtk_notebook_new();
    gtk_container_add(GTK_CONTAINER(window), notebook);
    
    // Setup tabs
    setup_system_info_tab();
    setup_grub_info_tab();
    setup_entries_tab();
    setup_terminal_tab();
    setup_help_tab();
    setup_operations_tab();
    
    // Update all info
    refresh_all();
    
    // Show startup warning
    show_startup_warning();
    
    // Show window
    gtk_widget_show_all(window);
}

void GTKUI::setup_system_info_tab() {
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    
    // Mode label at top
    mode_label = gtk_label_new("Mode: SAFE");
    gtk_widget_set_halign(mode_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(vbox), mode_label, FALSE, FALSE, 5);
    
    GtkWidget* frame = gtk_frame_new("Operating System");
    GtkWidget* frame_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(frame_vbox), 10);
    
    os_label = gtk_label_new("");
    kernel_label = gtk_label_new("");
    arch_label = gtk_label_new("");
    distro_label = gtk_label_new("");
    
    gtk_widget_set_halign(os_label, GTK_ALIGN_START);
    gtk_widget_set_halign(kernel_label, GTK_ALIGN_START);
    gtk_widget_set_halign(arch_label, GTK_ALIGN_START);
    gtk_widget_set_halign(distro_label, GTK_ALIGN_START);
    
    gtk_box_pack_start(GTK_BOX(frame_vbox), os_label, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(frame_vbox), kernel_label, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(frame_vbox), arch_label, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(frame_vbox), distro_label, FALSE, FALSE, 2);
    
    gtk_container_add(GTK_CONTAINER(frame), frame_vbox);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 5);
    
    frame = gtk_frame_new("Hardware");
    frame_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(frame_vbox), 10);
    
    cpu_label = gtk_label_new("");
    memory_label = gtk_label_new("");
    
    gtk_widget_set_halign(cpu_label, GTK_ALIGN_START);
    gtk_widget_set_halign(memory_label, GTK_ALIGN_START);
    
    gtk_box_pack_start(GTK_BOX(frame_vbox), cpu_label, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(frame_vbox), memory_label, FALSE, FALSE, 2);
    
    gtk_container_add(GTK_CONTAINER(frame), frame_vbox);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 5);
    
    frame = gtk_frame_new("User Info");
    frame_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(frame_vbox), 10);
    
    user_label = gtk_label_new("");
    gtk_widget_set_halign(user_label, GTK_ALIGN_START);
    
    gtk_box_pack_start(GTK_BOX(frame_vbox), user_label, FALSE, FALSE, 2);
    gtk_container_add(GTK_CONTAINER(frame), frame_vbox);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 5);
    
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, gtk_label_new("System Info"));
}

void GTKUI::setup_grub_info_tab() {
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    
    GtkWidget* frame = gtk_frame_new("GRUB Information");
    GtkWidget* frame_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(frame_vbox), 10);
    
    grub_version_label = gtk_label_new("");
    grub_installed_label = gtk_label_new("");
    default_entry_label = gtk_label_new("");
    entries_count_label = gtk_label_new("");
    
    gtk_widget_set_halign(grub_version_label, GTK_ALIGN_START);
    gtk_widget_set_halign(grub_installed_label, GTK_ALIGN_START);
    gtk_widget_set_halign(default_entry_label, GTK_ALIGN_START);
    gtk_widget_set_halign(entries_count_label, GTK_ALIGN_START);
    
    gtk_box_pack_start(GTK_BOX(frame_vbox), grub_version_label, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(frame_vbox), grub_installed_label, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(frame_vbox), default_entry_label, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(frame_vbox), entries_count_label, FALSE, FALSE, 2);
    
    gtk_container_add(GTK_CONTAINER(frame), frame_vbox);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 5);
    
    frame = gtk_frame_new("GRUB Configuration");
    GtkWidget* scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scrolled, -1, 300);
    
    grub_config_text = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(grub_config_text), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(grub_config_text), GTK_WRAP_WORD);
    gtk_container_add(GTK_CONTAINER(scrolled), grub_config_text);
    
    gtk_container_add(GTK_CONTAINER(frame), scrolled);
    gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);
    
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, gtk_label_new("GRUB Info"));
}

void GTKUI::setup_entries_tab() {
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    
    // Create list store
    entries_store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    
    // Create tree view
    entries_list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(entries_store));
    
    // Add columns
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn* column = gtk_tree_view_column_new_with_attributes("Title", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(entries_list), column);
    
    column = gtk_tree_view_column_new_with_attributes("UUID", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(entries_list), column);
    
    column = gtk_tree_view_column_new_with_attributes("Kernel", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(entries_list), column);
    
    column = gtk_tree_view_column_new_with_attributes("Default", renderer, "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(entries_list), column);
    
    // Scrolled window
    GtkWidget* scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled), entries_list);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 5);
    
    // Buttons
    GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    
    GtkWidget* refresh_btn = gtk_button_new_with_label("Refresh");
    g_signal_connect(refresh_btn, "clicked", G_CALLBACK(on_refresh_entries), this);
    gtk_box_pack_start(GTK_BOX(hbox), refresh_btn, FALSE, FALSE, 5);
    
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);
    
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, gtk_label_new("Entries"));
}

void GTKUI::setup_terminal_tab() {
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    
    // Output
    GtkWidget* scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scrolled, -1, 400);
    
    terminal_output = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(terminal_output), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(terminal_output), GTK_WRAP_WORD);
    gtk_container_add(GTK_CONTAINER(scrolled), terminal_output);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 5);
    
    // Input
    GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    terminal_input = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox), terminal_input, TRUE, TRUE, 5);
    
    GtkWidget* execute_btn = gtk_button_new_with_label("Execute");
    g_signal_connect(execute_btn, "clicked", G_CALLBACK(on_execute_command), this);
    gtk_box_pack_start(GTK_BOX(hbox), execute_btn, FALSE, FALSE, 5);
    
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);
    
    // Welcome message with user prompt
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(terminal_output));
    std::string prompt = sys_info.username + "@" + sys_info.hostname + ":~$ ";
    gtk_text_buffer_set_text(buffer, ("GRUB Storm Utils Terminal\n" + prompt + "\nType 'help' for available commands\n\n").c_str(), -1);
    
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, gtk_label_new("Terminal"));
}

void GTKUI::setup_modes_tab() {
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    
    // Mode selection frame
    GtkWidget* mode_frame = gtk_frame_new("Select Mode");
    GtkWidget* mode_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(mode_vbox), 15);
    
    mode_safe_radio = gtk_radio_button_new_with_label(NULL, "SAFE MODE - Read Only");
    gtk_box_pack_start(GTK_BOX(mode_vbox), mode_safe_radio, FALSE, FALSE, 5);
    
    GSList* mode_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(mode_safe_radio));
    mode_dangerous_radio = gtk_radio_button_new_with_label(mode_group, "DANGEROUS MODE - Full Control");
    gtk_box_pack_start(GTK_BOX(mode_vbox), mode_dangerous_radio, FALSE, FALSE, 5);
    
    g_signal_connect(mode_safe_radio, "toggled", G_CALLBACK(on_mode_safe_toggled), this);
    g_signal_connect(mode_dangerous_radio, "toggled", G_CALLBACK(on_mode_dangerous_toggled), this);
    
    gtk_container_add(GTK_CONTAINER(mode_frame), mode_vbox);
    gtk_box_pack_start(GTK_BOX(vbox), mode_frame, FALSE, FALSE, 10);
    
    // Mode info label
    modes_info_label = gtk_label_new("");
    gtk_widget_set_halign(modes_info_label, GTK_ALIGN_START);
    gtk_label_set_line_wrap(GTK_LABEL(modes_info_label), TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), modes_info_label, FALSE, FALSE, 10);
    
    // Operations frame (only visible in dangerous mode)
    modes_operations_frame = gtk_frame_new("GRUB Operations (DANGEROUS MODE)");
    GtkWidget* ops_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(ops_vbox), 15);
    
    // Set default entry
    GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new("Set Default Entry:"), FALSE, FALSE, 5);
    default_entry_combo = gtk_combo_box_text_new();
    gtk_box_pack_start(GTK_BOX(hbox), default_entry_combo, TRUE, TRUE, 5);
    GtkWidget* set_default_btn = gtk_button_new_with_label("Apply");
    g_signal_connect(set_default_btn, "clicked", G_CALLBACK(on_set_default), this);
    gtk_box_pack_start(GTK_BOX(hbox), set_default_btn, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(ops_vbox), hbox, FALSE, FALSE, 5);
    
    // Add new entry
    GtkWidget* add_frame = gtk_frame_new("Add New Entry");
    GtkWidget* add_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(add_vbox), 10);
    
    new_entry_title = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(new_entry_title), "Entry Title");
    gtk_box_pack_start(GTK_BOX(add_vbox), new_entry_title, FALSE, FALSE, 5);
    
    new_entry_kernel = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(new_entry_kernel), "Kernel Path (e.g., /boot/vmlinuz-...)");
    gtk_box_pack_start(GTK_BOX(add_vbox), new_entry_kernel, FALSE, FALSE, 5);
    
    new_entry_initrd = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(new_entry_initrd), "Initrd Path (e.g., /boot/initrd.img-...)");
    gtk_box_pack_start(GTK_BOX(add_vbox), new_entry_initrd, FALSE, FALSE, 5);
    
    GtkWidget* add_entry_btn = gtk_button_new_with_label("Add Entry");
    g_signal_connect(add_entry_btn, "clicked", G_CALLBACK(on_add_entry), this);
    gtk_box_pack_start(GTK_BOX(add_vbox), add_entry_btn, FALSE, FALSE, 5);
    
    gtk_container_add(GTK_CONTAINER(add_frame), add_vbox);
    gtk_box_pack_start(GTK_BOX(ops_vbox), add_frame, FALSE, FALSE, 10);
    
    // Timeout setting
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new("Boot Timeout (seconds):"), FALSE, FALSE, 5);
    timeout_spin = gtk_spin_button_new_with_range(0, 300, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(timeout_spin), 5);
    gtk_box_pack_start(GTK_BOX(hbox), timeout_spin, FALSE, FALSE, 5);
    GtkWidget* set_timeout_btn = gtk_button_new_with_label("Set Timeout");
    g_signal_connect(set_timeout_btn, "clicked", G_CALLBACK(on_set_timeout), this);
    gtk_box_pack_start(GTK_BOX(hbox), set_timeout_btn, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(ops_vbox), hbox, FALSE, FALSE, 5);
    
    // Update GRUB button
    GtkWidget* update_grub_btn = gtk_button_new_with_label("Update GRUB (run update-grub)");
    gtk_widget_set_name(update_grub_btn, "dangerous-btn");
    g_signal_connect(update_grub_btn, "clicked", G_CALLBACK(on_update_grub), this);
    gtk_box_pack_start(GTK_BOX(ops_vbox), update_grub_btn, FALSE, FALSE, 10);
    
    gtk_container_add(GTK_CONTAINER(modes_operations_frame), ops_vbox);
    gtk_box_pack_start(GTK_BOX(vbox), modes_operations_frame, TRUE, TRUE, 10);
    
    // Advanced GRUB Customization Frame
    GtkWidget* advanced_frame = gtk_frame_new("Advanced GRUB Customization");
    GtkWidget* advanced_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(advanced_vbox), 15);
    
    // Kernel parameters
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new("Kernel Parameters:"), FALSE, FALSE, 5);
    kernel_params_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(kernel_params_entry), "e.g., nomodeset acpi=off");
    gtk_box_pack_start(GTK_BOX(hbox), kernel_params_entry, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(advanced_vbox), hbox, FALSE, FALSE, 5);
    
    // Graphics mode
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new("Graphics Mode:"), FALSE, FALSE, 5);
    gfxmode_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(gfxmode_entry), "e.g., text, keep, auto, 640x480");
    gtk_box_pack_start(GTK_BOX(hbox), gfxmode_entry, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(advanced_vbox), hbox, FALSE, FALSE, 5);
    
    // Resolution
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new("Resolution:"), FALSE, FALSE, 5);
    resolution_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(resolution_entry), "e.g., 1920x1080");
    gtk_box_pack_start(GTK_BOX(hbox), resolution_entry, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(advanced_vbox), hbox, FALSE, FALSE, 5);
    
    // Splash screen
    splash_check = gtk_check_button_new_with_label("Enable Splash Screen");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(splash_check), TRUE);
    gtk_box_pack_start(GTK_BOX(advanced_vbox), splash_check, FALSE, FALSE, 5);
    
    // Quiet boot
    quiet_check = gtk_check_button_new_with_label("Quiet Boot (hide boot messages)");
    gtk_box_pack_start(GTK_BOX(advanced_vbox), quiet_check, FALSE, FALSE, 5);
    
    // Apply advanced settings button
    GtkWidget* apply_advanced_btn = gtk_button_new_with_label("Apply Advanced Settings");
    g_signal_connect(apply_advanced_btn, "clicked", G_CALLBACK(on_update_grub), this);
    gtk_box_pack_start(GTK_BOX(advanced_vbox), apply_advanced_btn, FALSE, FALSE, 10);
    
    gtk_container_add(GTK_CONTAINER(advanced_frame), advanced_vbox);
    gtk_box_pack_start(GTK_BOX(vbox), advanced_frame, FALSE, FALSE, 10);
    
    // Set initial state
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(mode_safe_radio), TRUE);
    
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, gtk_label_new("Modes"));
}

void GTKUI::setup_help_tab() {
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    
    GtkWidget* scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
    help_text = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(help_text), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(help_text), GTK_WRAP_WORD);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(help_text), 10);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(help_text), 10);
    
    std::string help_content = 
        "╔══════════════════════════════════════════════════════════════════════════════╗\n"
        "║                    GRUB STORM UTILS - ИНСТРУКЦИЯ                              ║\n"
        "╚══════════════════════════════════════════════════════════════════════════════╝\n\n"
        
        "⚠️  ВАЖНОЕ ПРЕДУПРЕЖДЕНИЕ О БЕЗОПАСНОСТИ ⚠️\n\n"
        
        "Эта программа позволяет модифицировать конфигурацию загрузчика GRUB.\n"
        "НЕПРАВИЛЬНЫЕ ИЗМЕНЕНИЯ МОГУТ СДЕЛАТЬ ВАШУ СИСТЕМУ НЕЗАГРУЖАЕМОЙ!\n\n"
        
        "─────────────────────────────────────────────────────────────────────────────────\n\n"
        
        "📋 О ПРОГРАММЕ\n\n"
        
        "GRUB Storm Utils - это утилита для управления загрузчиком GRUB с графическим\n"
        "интерфейсом. Программа предоставляет два режима работы:\n\n"
        
        "🟢 SAFE MODE (БЕЗОПАСНЫЙ РЕЖИМ)\n"
        "  • Только чтение конфигурации GRUB\n"
        "  • Просмотр записей загрузки\n"
        "  • Просмотр системной информации\n"
        "  • Никаких модификаций не разрешено\n\n"
        
        "🔴 DANGEROUS MODE (ОПАСНЫЙ РЕЖИМ)\n"
        "  • Полный контроль над GRUB\n"
        "  • Изменение записи по умолчанию\n"
        "  • Добавление новых записей\n"
        "  • Изменение таймаута\n"
        "  • Обновление GRUB (update-grub)\n\n"
        
        "─────────────────────────────────────────────────────────────────────────────────\n\n"
        
        "📖 ИНСТРУКЦИЯ ПО ИСПОЛЬЗОВАНИЮ\n\n"
        
        "1. ЗАПУСК ПРОГРАММЫ\n"
        "   • Запускайте с правами root: sudo ./GRUBStormUtils\n"
        "   • При запуске появится предупреждение - прочитайте его внимательно\n"
        "   • Программа автоматически соберет информацию о системе\n\n"
        
        "2. ВКЛАДКИ ИНТЕРФЕЙСА\n"
        
        "   📊 System Info\n"
        "      • Показывает детальную информацию о системе\n"
        "      • ОС, ядро, архитектура, дистрибутив\n"
        "      • CPU, память, пользователь\n\n"
        
        "   🔧 GRUB Info\n"
        "      • Информация о версии GRUB\n"
        "      • Статус установки\n"
        "      • Текущая запись по умолчанию\n"
        "      • Полная конфигурация GRUB\n\n"
        
        "   📝 Entries\n"
        "      • Таблица всех записей загрузки\n"
        "      • Кнопка Refresh для обновления списка\n"
        "      • Кнопка Toggle Mode для переключения режимов\n\n"
        
        "   💻 Terminal\n"
        "      • Встроенный терминал для выполнения команд\n"
        "      • Введите команду и нажмите Execute\n"
        "      • Результаты отображаются в окне\n\n"
        
        "   ⚙️  Modes (РЕЖИМЫ)\n"
        "      • Отдельная вкладка для управления режимами\n"
        "      • Выберите SAFE или DANGEROUS режим\n"
        "      • В DANGEROUS режиме доступны операции с GRUB:\n"
        "        - Установка записи по умолчанию\n"
        "        - Добавление новых записей\n"
        "        - Изменение таймаута загрузки\n"
        "        - Обновление GRUB\n\n"
        
        "   🛠️  Operations\n"
        "      • Дополнительные операции с GRUB\n"
        "      • Требуют DANGEROUS режим\n\n"
        
        "3. ПЕРЕКЛЮЧЕНИЕ РЕЖИМОВ\n"
        "   • Используйте вкладку Modes для переключения\n"
        "   • Или кнопку Toggle Mode на вкладке Entries\n"
        "   • При переключении в DANGEROUS режим появится подтверждение\n"
        "   • Введите 'DANGEROUS' для подтверждения\n\n"
        
        "─────────────────────────────────────────────────────────────────────────────────\n\n"
        
        "⚠️  МЕРЫ ПРЕДОСТОРОЖНОСТИ\n\n"
        
        "ПЕРЕД ИСПОЛЬЗОВАНИЕМ DANGEROUS РЕЖИМА:\n"
        "   1. Создайте резервную копию конфигурации GRUB\n"
        "   2. Имейте под рукой Live USB с Linux\n"
        "   3. Запишите текущую конфигурацию\n"
        "   4. Понимайте что вы делаете\n\n"
        
        "ПРИ ВНЕСЕНИИ ИЗМЕНЕНИЙ:\n"
        "   • Вносите изменения по одному\n"
        "   • Проверяйте каждое изменение\n"
        "   • Не изменяйте параметры если не уверены\n"
        "   • После изменений запустите update-grub\n\n"
        
        "ЕСЛИ СИСТЕМА НЕ ЗАГРУЖАЕТСЯ:\n"
        "   • Загрузитесь с Live USB\n"
        "   • Смонтируйте раздел с /boot\n"
        "   • Восстановите конфигурацию из резервной копии\n"
        "   • Или переустановите GRUB\n\n"
        
        "─────────────────────────────────────────────────────────────────────────────────\n\n"
        
        "💡 СОВЕТЫ\n\n"
        
        "• Начните с SAFE режима для изучения конфигурации\n"
        "• Используйте Terminal для проверки команд перед выполнением\n"
        "• Документируйте все изменения\n"
        "• Регулярно создавайте резервные копии\n"
        "• Если не уверены - не меняйте ничего\n\n"
        
        "─────────────────────────────────────────────────────────────────────────────────\n\n"
        
        "📞 ПОДДЕРЖКА\n\n"
        
        "При возникновении проблем:\n"
        "   • Проверьте логи в /var/log/\n"
        "   • Используйте man grub, man update-grub\n"
        "   • Консультируйтесь с документацией вашего дистрибутива\n\n"
        
        "─────────────────────────────────────────────────────────────────────────────────\n\n"
        
        "📜 ЛИЦЕНЗИЯ\n\n"
        
        "MIT License\n"
        "Используйте на свой риск и ответственность.\n\n"
        
        "═══════════════════════════════════════════════════════════════════════════════\n"
        "                ВНИМАТЕЛЬНО ПРОЧИТАЙТЕ ЭТУ ИНСТРУКЦИЮ\n"
        "       ПЕРЕД ИСПОЛЬЗОВАНИЕМ DANGEROUS РЕЖИМА И МОДИФИКАЦИЕЙ GRUB\n"
        "═══════════════════════════════════════════════════════════════════════════════\n";
    
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(help_text));
    gtk_text_buffer_set_text(buffer, help_content.c_str(), -1);
    
    // Set monospace font for better readability
    PangoFontDescription* font_desc = pango_font_description_from_string("monospace 9");
    gtk_widget_override_font(help_text, font_desc);
    pango_font_description_free(font_desc);
    
    gtk_container_add(GTK_CONTAINER(scrolled), help_text);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 5);
    
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, gtk_label_new("Help"));
}

void GTKUI::setup_operations_tab() {
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    
    // Warning
    GtkWidget* warning = gtk_label_new("DANGEROUS MODE REQUIRED for these operations");
    gtk_widget_set_name(warning, "warning");
    PangoFontDescription* font_desc = pango_font_description_from_string("bold");
    gtk_widget_modify_font(warning, font_desc);
    pango_font_description_free(font_desc);
    gtk_box_pack_start(GTK_BOX(vbox), warning, FALSE, FALSE, 5);
    
    // Set default entry
    GtkWidget* frame = gtk_frame_new("Set Default Entry");
    GtkWidget* frame_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(frame_vbox), 10);
    
    default_entry_combo = gtk_combo_box_text_new();
    gtk_box_pack_start(GTK_BOX(frame_vbox), default_entry_combo, FALSE, FALSE, 5);
    
    GtkWidget* set_default_btn = gtk_button_new_with_label("Set Default");
    g_signal_connect(set_default_btn, "clicked", G_CALLBACK(on_set_default), this);
    gtk_box_pack_start(GTK_BOX(frame_vbox), set_default_btn, FALSE, FALSE, 5);
    
    gtk_container_add(GTK_CONTAINER(frame), frame_vbox);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 5);
    
    // Add entry
    frame = gtk_frame_new("Add New Entry");
    frame_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(frame_vbox), 10);
    
    new_entry_title = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(new_entry_title), "Title");
    gtk_box_pack_start(GTK_BOX(frame_vbox), new_entry_title, FALSE, FALSE, 5);
    
    new_entry_kernel = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(new_entry_kernel), "Kernel path");
    gtk_box_pack_start(GTK_BOX(frame_vbox), new_entry_kernel, FALSE, FALSE, 5);
    
    new_entry_initrd = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(new_entry_initrd), "Initrd path");
    gtk_box_pack_start(GTK_BOX(frame_vbox), new_entry_initrd, FALSE, FALSE, 5);
    
    GtkWidget* add_entry_btn = gtk_button_new_with_label("Add Entry");
    g_signal_connect(add_entry_btn, "clicked", G_CALLBACK(on_add_entry), this);
    gtk_box_pack_start(GTK_BOX(frame_vbox), add_entry_btn, FALSE, FALSE, 5);
    
    gtk_container_add(GTK_CONTAINER(frame), frame_vbox);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 5);
    
    // Update GRUB
    GtkWidget* update_btn = gtk_button_new_with_label("Update GRUB (update-grub)");
    g_signal_connect(update_btn, "clicked", G_CALLBACK(on_update_grub), this);
    gtk_box_pack_start(GTK_BOX(vbox), update_btn, FALSE, FALSE, 5);
    
    operations_tab_index = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, gtk_label_new("Operations"));
}

void GTKUI::update_system_info() {
    std::string os_text = "OS: " + sys_info.os_name + " " + sys_info.os_version;
    gtk_label_set_text(GTK_LABEL(os_label), os_text.c_str());
    
    std::string kernel_text = "Kernel: " + sys_info.kernel_version;
    gtk_label_set_text(GTK_LABEL(kernel_label), kernel_text.c_str());
    
    std::string arch_text = "Architecture: " + sys_info.architecture + 
                           (sys_info.is_64bit ? " (64-bit)" : " (32-bit)");
    gtk_label_set_text(GTK_LABEL(arch_label), arch_text.c_str());
    
    std::string distro_text = "Distribution: " + sys_info.distribution;
    gtk_label_set_text(GTK_LABEL(distro_label), distro_text.c_str());
    
    std::string cpu_text = "CPU: " + sys_info.cpu_model;
    gtk_label_set_text(GTK_LABEL(cpu_label), cpu_text.c_str());
    
    std::string mem_text = "Memory: " + SystemInfoCollector::format_bytes(sys_info.total_memory) + 
                          " total, " + SystemInfoCollector::format_bytes(sys_info.available_memory) + " available";
    gtk_label_set_text(GTK_LABEL(memory_label), mem_text.c_str());
    
    std::string user_text = "User: " + sys_info.username + "@" + sys_info.hostname;
    gtk_label_set_text(GTK_LABEL(user_label), user_text.c_str());
    
    if (manager.get_safety_mode() == SafetyMode::SAFE) {
        gtk_label_set_text(GTK_LABEL(mode_label), "Mode: SAFE");
    } else {
        gtk_label_set_text(GTK_LABEL(mode_label), "Mode: DANGEROUS");
    }
}

void GTKUI::update_grub_info() {
    gtk_label_set_text(GTK_LABEL(grub_version_label), 
                       ("Version: " + manager.get_grub_version()).c_str());
    gtk_label_set_text(GTK_LABEL(grub_installed_label), 
                       manager.check_grub_installed() ? "Installed: Yes" : "Installed: No");
    gtk_label_set_text(GTK_LABEL(default_entry_label), 
                       ("Default: " + manager.get_default_entry()).c_str());
    
    auto entries = manager.get_entries();
    gtk_label_set_text(GTK_LABEL(entries_count_label), 
                       ("Total Entries: " + std::to_string(entries.size())).c_str());
    
    // Enhanced GRUB information
    std::string enhanced_config = "=== GRUB CONFIGURATION ===\n\n";
    enhanced_config += manager.get_config_content();
    enhanced_config += "\n\n=== ADDITIONAL GRUB INFORMATION ===\n\n";
    
    // Get default GRUB configuration
    std::string default_config = SystemInfoCollector::execute_command("cat /etc/default/grub 2>/dev/null");
    if (!default_config.empty()) {
        enhanced_config += "DEFAULT CONFIGURATION (/etc/default/grub):\n";
        enhanced_config += default_config;
        enhanced_config += "\n";
    }
    
    // Get GRUB scripts information
    enhanced_config += "GRUB SCRIPTS (/etc/grub.d/):\n";
    std::string scripts = SystemInfoCollector::execute_command("ls -la /etc/grub.d/ 2>/dev/null");
    enhanced_config += scripts + "\n";
    
    // Get GRUB scripts content
    enhanced_config += "\nGRUB SCRIPT CONTENTS:\n";
    std::string script_content = SystemInfoCollector::execute_command("head -50 /etc/grub.d/10_linux 2>/dev/null");
    enhanced_config += "10_linux (first 50 lines):\n" + script_content + "\n";
    
    // Get GRUB modules
    enhanced_config += "\nGRUB MODULES (/boot/grub/):\n";
    std::string modules = SystemInfoCollector::execute_command("ls -la /boot/grub/*.mod 2>/dev/null | head -30");
    enhanced_config += modules + "\n";
    
    // Get GRUB fonts
    enhanced_config += "GRUB FONTS:\n";
    std::string fonts = SystemInfoCollector::execute_command("ls -la /boot/grub/*.pf2 2>/dev/null");
    enhanced_config += fonts + "\n";
    
    // Get GRUB themes
    enhanced_config += "GRUB THEMES:\n";
    std::string themes = SystemInfoCollector::execute_command("ls -la /boot/grub/themes/ 2>/dev/null");
    enhanced_config += themes + "\n";
    
    // Get GRUB device map
    enhanced_config += "\nGRUB DEVICE MAP:\n";
    std::string device_map = SystemInfoCollector::execute_command("cat /boot/grub/device.map 2>/dev/null");
    enhanced_config += device_map + "\n";
    
    // Get partition information
    enhanced_config += "\nPARTITION INFORMATION:\n";
    std::string partitions = SystemInfoCollector::execute_command("sudo fdisk -l 2>/dev/null | head -40");
    enhanced_config += partitions + "\n";
    
    // Get EFI information if available
    enhanced_config += "\nEFI INFORMATION:\n";
    std::string efi_info = SystemInfoCollector::execute_command("ls -la /boot/efi/ 2>/dev/null");
    enhanced_config += efi_info + "\n";
    
    // Get UUID information
    enhanced_config += "\nDISK UUID INFORMATION:\n";
    std::string uuid_info = SystemInfoCollector::execute_command("sudo blkid 2>/dev/null | head -20");
    enhanced_config += uuid_info + "\n";
    
    // Get mount information
    enhanced_config += "\nMOUNT INFORMATION:\n";
    std::string mount_info = SystemInfoCollector::execute_command("mount | grep -E '(boot|efi)' 2>/dev/null");
    enhanced_config += mount_info + "\n";
    
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(grub_config_text));
    gtk_text_buffer_set_text(buffer, enhanced_config.c_str(), -1);
}

void GTKUI::update_entries_list() {
    gtk_list_store_clear(entries_store);
    
    auto entries = manager.get_entries();
    for (const auto& entry : entries) {
        GtkTreeIter iter;
        gtk_list_store_append(entries_store, &iter);
        gtk_list_store_set(entries_store, &iter,
                          0, entry.title.c_str(),
                          1, entry.uuid.c_str(),
                          2, entry.kernel.c_str(),
                          3, entry.is_default ? "Yes" : "No",
                          -1);
        
        // Also update combo box
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(default_entry_combo), entry.title.c_str());
    }
}

void GTKUI::refresh_all() {
    update_system_info();
    update_grub_info();
    update_entries_list();
}

void GTKUI::show_startup_warning() {
    GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(window),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_WARNING,
        GTK_BUTTONS_OK,
        "%s",
        "⚠️  ВАЖНОЕ ПРЕДУПРЕЖДЕНИЕ ⚠️\n\n"
        "Эта программа позволяет модифицировать конфигурацию загрузчика GRUB.\n"
        "НЕПРАВИЛЬНЫЕ ИЗМЕНЕНИЯ МОГУТ СДЕЛАТЬ ВАШУ СИСТЕМУ НЕЗАГРУЖАЕМОЙ!\n\n"
        "Рекомендации:\n"
        "• Прочитайте инструкцию на вкладке Help\n"
        "• Создайте резервную копию конфигурации GRUB\n"
        "• Имейте под рукой Live USB с Linux для восстановления\n"
        "• Вносите изменения осознанно\n"
        "• Используйте команду 'backup' в терминале перед изменениями\n\n"
        "Вы используете эту программу на свой собственный риск.\n"
        "Авторы не несут ответственности за потерю данных или повреждение системы.");
    
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void GTKUI::run() {
    gtk_main();
}

// Static callbacks
gboolean GTKUI::on_page_switch(GtkWidget* widget, GtkWidget* page, guint page_num, gpointer data) {
    GTKUI* ui = static_cast<GTKUI*>(data);
    
    // Check if trying to access Operations tab in SAFE mode
    if (page_num == ui->operations_tab_index && ui->manager.get_safety_mode() == SafetyMode::SAFE) {
        // Show warning dialog
        GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(ui->window),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_WARNING,
            GTK_BUTTONS_OK,
            "%s",
            "DANGEROUS MODE REQUIRED\n\n"
            "The Operations tab is only available in DANGEROUS mode.\n"
            "Switch to DANGEROUS mode on the Modes tab to access these operations.");
        
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        
        // Prevent switching to Operations tab
        return TRUE;
    }
    
    return FALSE;
}

void GTKUI::on_refresh_entries(GtkWidget* widget, gpointer data) {
    GTKUI* ui = static_cast<GTKUI*>(data);
    ui->update_entries_list();
}

void GTKUI::on_toggle_mode(GtkWidget* widget, gpointer data) {
    GTKUI* ui = static_cast<GTKUI*>(data);
    if (ui->manager.get_safety_mode() == SafetyMode::SAFE) {
        ui->manager.set_safety_mode(SafetyMode::DANGEROUS);
    } else {
        ui->manager.set_safety_mode(SafetyMode::SAFE);
    }
    ui->refresh_all();
}

void GTKUI::on_mode_safe_toggled(GtkWidget* widget, gpointer data) {
    GTKUI* ui = static_cast<GTKUI*>(data);
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        ui->manager.set_safety_mode(SafetyMode::SAFE);
        gtk_label_set_text(GTK_LABEL(ui->modes_info_label), 
            "SAFE MODE ACTIVE\n\n"
            "Read-only access to GRUB configuration.\n"
            "You can view entries, configuration, and system information.\n"
            "No modifications allowed.\n\n"
            "Switch to DANGEROUS MODE for full control.");
        gtk_widget_set_sensitive(ui->modes_operations_frame, FALSE);
        ui->refresh_all();
    }
}

void GTKUI::on_mode_dangerous_toggled(GtkWidget* widget, gpointer data) {
    GTKUI* ui = static_cast<GTKUI*>(data);
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        ui->manager.set_safety_mode(SafetyMode::DANGEROUS);
        gtk_label_set_text(GTK_LABEL(ui->modes_info_label), 
            "DANGEROUS MODE ACTIVE\n\n"
            "Full control over GRUB configuration.\n"
            "You can modify entries, change defaults, update GRUB.\n"
            "⚠️  WARNING: Incorrect changes may make your system unbootable!\n"
            "Make sure you have a backup before making changes.");
        gtk_widget_set_sensitive(ui->modes_operations_frame, TRUE);
        ui->refresh_all();
    }
}

void GTKUI::on_execute_command(GtkWidget* widget, gpointer data) {
    GTKUI* ui = static_cast<GTKUI*>(data);
    const gchar* cmd = gtk_entry_get_text(GTK_ENTRY(ui->terminal_input));
    
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui->terminal_output));
    gtk_text_buffer_insert_at_cursor(buffer, ("$ " + std::string(cmd) + "\n").c_str(), -1);
    
    std::string cmd_str(cmd);
    
    // Built-in commands
    if (cmd_str == "help" || cmd_str == "?") {
        std::string help_text = 
            "=== GRUB Storm Utils Terminal Commands ===\n\n"
            "Built-in Commands:\n"
            "  help, ?              - Show this help\n"
            "  clear                - Clear terminal\n"
            "  sysinfo              - Show system information\n"
            "  grub                 - Show GRUB information\n"
            "  entries              - List all GRUB entries\n"
            "  default              - Show default entry\n"
            "  version              - Show GRUB version\n"
            "  config               - Show GRUB configuration\n"
            "  set-default <name>   - Set default entry\n"
            "  add-entry <title>    - Add new entry\n"
            "  timeout <seconds>    - Set boot timeout\n"
            "  update-grub          - Run update-grub\n"
            "  backup               - Backup GRUB configuration\n"
            "  restore              - Restore GRUB configuration\n"
            "  check                - Check GRUB installation\n"
            "  install <device>     - Install GRUB to device\n"
            "  remove <device>      - Remove GRUB from device\n"
            "  memtest              - Add memtest86+ entry\n"
            "  recovery             - Add recovery mode entry\n"
            "  exit, quit           - Exit program\n\n"
            "Shell Commands:\n"
            "  Any other command will be executed in shell\n"
            "  Examples: ls, cat /etc/default/grub, uname -a\n\n";
        gtk_text_buffer_insert_at_cursor(buffer, help_text.c_str(), -1);
    }
    else if (cmd_str == "clear") {
        GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui->terminal_output));
        gtk_text_buffer_set_text(buffer, "", -1);
        gtk_text_buffer_insert_at_cursor(buffer, "=== GRUB Storm Utils Terminal ===\n", -1);
    }
    else if (cmd_str == "sysinfo") {
        std::string info = 
            "=== System Information ===\n\n"
            "OS: " + ui->sys_info.os_name + " " + ui->sys_info.os_version + "\n"
            "Kernel: " + ui->sys_info.kernel_version + "\n"
            "Architecture: " + ui->sys_info.architecture + (ui->sys_info.is_64bit ? " (64-bit)" : " (32-bit)") + "\n"
            "Distribution: " + ui->sys_info.distribution + "\n"
            "Hostname: " + ui->sys_info.hostname + "\n"
            "Username: " + ui->sys_info.username + "\n"
            "CPU: " + ui->sys_info.cpu_model + "\n"
            "CPU Cores: " + std::to_string(ui->sys_info.cpu_cores) + "\n"
            "Total Memory: " + SystemInfoCollector::format_bytes(ui->sys_info.total_memory) + "\n"
            "Available Memory: " + SystemInfoCollector::format_bytes(ui->sys_info.available_memory) + "\n\n";
        gtk_text_buffer_insert_at_cursor(buffer, info.c_str(), -1);
    }
    else if (cmd_str == "grub") {
        std::string info = 
            "=== GRUB Information ===\n\n"
            "Version: " + ui->manager.get_grub_version() + "\n"
            "Installed: " + (ui->manager.check_grub_installed() ? "Yes" : "No") + "\n"
            "Default Entry: " + ui->manager.get_default_entry() + "\n"
            "Config Path: /boot/grub/grub.cfg\n"
            "Default Config: /etc/default/grub\n"
            "Scripts Path: /etc/grub.d/\n\n";
        gtk_text_buffer_insert_at_cursor(buffer, info.c_str(), -1);
    }
    else if (cmd_str == "entries") {
        auto entries = ui->manager.get_entries();
        std::string info = "=== GRUB Boot Entries ===\n\n";
        info += "Total Entries: " + std::to_string(entries.size()) + "\n\n";
        
        for (size_t i = 0; i < entries.size(); ++i) {
            const auto& entry = entries[i];
            info += "[" + std::to_string(i+1) + "] " + entry.title + "\n";
            info += "    UUID: " + (entry.uuid.empty() ? "N/A" : entry.uuid) + "\n";
            info += "    Kernel: " + (entry.kernel.empty() ? "N/A" : entry.kernel) + "\n";
            info += "    Initrd: " + (entry.initrd.empty() ? "N/A" : entry.initrd) + "\n";
            if (entry.is_default) info += "    [DEFAULT]\n";
            info += "\n";
        }
        gtk_text_buffer_insert_at_cursor(buffer, info.c_str(), -1);
    }
    else if (cmd_str == "default") {
        std::string info = "=== Default Boot Entry ===\n\n";
        info += "Current Default: " + ui->manager.get_default_entry() + "\n";
        info += "This entry will be booted automatically after timeout.\n\n";
        gtk_text_buffer_insert_at_cursor(buffer, info.c_str(), -1);
    }
    else if (cmd_str == "version") {
        std::string info = "=== GRUB Version ===\n\n";
        info += ui->manager.get_grub_version() + "\n\n";
        gtk_text_buffer_insert_at_cursor(buffer, info.c_str(), -1);
    }
    else if (cmd_str == "config") {
        std::string info = "=== GRUB Configuration ===\n\n";
        info += ui->manager.get_config_content() + "\n\n";
        gtk_text_buffer_insert_at_cursor(buffer, info.c_str(), -1);
    }
    else if (cmd_str == "check") {
        std::string result = SystemInfoCollector::execute_command("which grub-install");
        std::string info = "=== GRUB Installation Check ===\n\n";
        info += "grub-install: " + (result.empty() ? "Not found" : result) + "\n";
        result = SystemInfoCollector::execute_command("which update-grub");
        info += "update-grub: " + (result.empty() ? "Not found" : result) + "\n";
        result = SystemInfoCollector::execute_command("ls -la /boot/grub/");
        info += "\n/boot/grub/ contents:\n" + result + "\n";
        gtk_text_buffer_insert_at_cursor(buffer, info.c_str(), -1);
    }
    else if (cmd_str == "backup") {
        std::string result = SystemInfoCollector::execute_command("sudo cp /boot/grub/grub.cfg /boot/grub/grub.cfg.backup");
        std::string info = "=== GRUB Configuration Backup ===\n\n";
        info += "Backup created at /boot/grub/grub.cfg.backup\n";
        gtk_text_buffer_insert_at_cursor(buffer, info.c_str(), -1);
    }
    else if (cmd_str == "restore") {
        std::string result = SystemInfoCollector::execute_command("sudo cp /boot/grub/grub.cfg.backup /boot/grub/grub.cfg");
        std::string info = "=== GRUB Configuration Restore ===\n\n";
        info += "Configuration restored from /boot/grub/grub.cfg.backup\n";
        gtk_text_buffer_insert_at_cursor(buffer, info.c_str(), -1);
    }
    else if (cmd_str == "memtest") {
        GRUBEntry entry;
        entry.title = "Memory Test (memtest86+)";
        entry.kernel = "/boot/memtest86+.bin";
        if (ui->manager.add_entry(entry)) {
            gtk_text_buffer_insert_at_cursor(buffer, "Memtest86+ entry added successfully.\n", -1);
        } else {
            gtk_text_buffer_insert_at_cursor(buffer, "Failed to add Memtest86+ entry.\n", -1);
        }
    }
    else if (cmd_str == "recovery") {
        GRUBEntry entry;
        entry.title = "Recovery Mode";
        entry.kernel = "/boot/vmlinuz-$(uname -r)";
        entry.initrd = "/boot/initrd.img-$(uname -r)";
        if (ui->manager.add_entry(entry)) {
            gtk_text_buffer_insert_at_cursor(buffer, "Recovery mode entry added successfully.\n", -1);
        } else {
            gtk_text_buffer_insert_at_cursor(buffer, "Failed to add recovery mode entry.\n", -1);
        }
    }
    else if (cmd_str.substr(0, 12) == "set-default ") {
        std::string entry_name = cmd_str.substr(12);
        if (ui->manager.set_default_entry(entry_name)) {
            gtk_text_buffer_insert_at_cursor(buffer, ("Default entry set to: " + entry_name + "\n").c_str(), -1);
            ui->refresh_all();
        } else {
            gtk_text_buffer_insert_at_cursor(buffer, "Failed to set default entry.\n", -1);
        }
    }
    else if (cmd_str.substr(0, 10) == "add-entry ") {
        std::string title = cmd_str.substr(10);
        GRUBEntry entry;
        entry.title = title;
        if (ui->manager.add_entry(entry)) {
            gtk_text_buffer_insert_at_cursor(buffer, ("Entry added: " + title + "\n").c_str(), -1);
            ui->refresh_all();
        } else {
            gtk_text_buffer_insert_at_cursor(buffer, "Failed to add entry.\n", -1);
        }
    }
    else if (cmd_str.substr(0, 8) == "timeout ") {
        try {
            int timeout = std::stoi(cmd_str.substr(8));
            if (ui->manager.update_timeout(timeout)) {
                gtk_text_buffer_insert_at_cursor(buffer, ("Timeout set to " + std::to_string(timeout) + " seconds\n").c_str(), -1);
            } else {
                gtk_text_buffer_insert_at_cursor(buffer, "Failed to set timeout.\n", -1);
            }
        } catch (...) {
            gtk_text_buffer_insert_at_cursor(buffer, "Invalid timeout value.\n", -1);
        }
    }
    else if (cmd_str == "update-grub") {
        gtk_text_buffer_insert_at_cursor(buffer, "Running update-grub...\n", -1);
        if (ui->manager.update_grub()) {
            gtk_text_buffer_insert_at_cursor(buffer, "GRUB updated successfully.\n", -1);
            ui->refresh_all();
        } else {
            gtk_text_buffer_insert_at_cursor(buffer, "Failed to update GRUB.\n", -1);
        }
    }
    else if (cmd_str.substr(0, 8) == "install ") {
        std::string device = cmd_str.substr(8);
        std::string cmd = "sudo grub-install " + device;
        std::string result = SystemInfoCollector::execute_command(cmd);
        gtk_text_buffer_insert_at_cursor(buffer, ("Installing GRUB to " + device + "...\n").c_str(), -1);
        gtk_text_buffer_insert_at_cursor(buffer, result.c_str(), -1);
    }
    else if (cmd_str.substr(0, 7) == "remove ") {
        gtk_text_buffer_insert_at_cursor(buffer, "GRUB removal is dangerous. Use 'grub-install' to reinstall.\n", -1);
    }
    else if (cmd_str == "exit" || cmd_str == "quit") {
        gtk_main_quit();
    }
    else if (cmd_str.empty()) {
        // Do nothing
    }
    else {
        // Execute as shell command
        std::string result = SystemInfoCollector::execute_command(cmd);
        gtk_text_buffer_insert_at_cursor(buffer, result.c_str(), -1);
    }
    
    gtk_entry_set_text(GTK_ENTRY(ui->terminal_input), "");
}

void GTKUI::on_set_default(GtkWidget* widget, gpointer data) {
    GTKUI* ui = static_cast<GTKUI*>(data);
    gchar* entry = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(ui->default_entry_combo));
    if (entry) {
        if (ui->manager.set_default_entry(entry)) {
            GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui->terminal_output));
            gtk_text_buffer_insert_at_cursor(buffer, ("Default entry set to: " + std::string(entry) + "\n").c_str(), -1);
            ui->refresh_all();
        }
        g_free(entry);
    }
}

void GTKUI::on_set_timeout(GtkWidget* widget, gpointer data) {
    GTKUI* ui = static_cast<GTKUI*>(data);
    int timeout = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->timeout_spin));
    if (ui->manager.update_timeout(timeout)) {
        GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui->terminal_output));
        gtk_text_buffer_insert_at_cursor(buffer, ("Timeout set to " + std::to_string(timeout) + " seconds\n").c_str(), -1);
    } else {
        GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui->terminal_output));
        gtk_text_buffer_insert_at_cursor(buffer, "Failed to set timeout\n", -1);
    }
}

void GTKUI::on_add_entry(GtkWidget* widget, gpointer data) {
    GTKUI* ui = static_cast<GTKUI*>(data);
    const gchar* title = gtk_entry_get_text(GTK_ENTRY(ui->new_entry_title));
    const gchar* kernel = gtk_entry_get_text(GTK_ENTRY(ui->new_entry_kernel));
    const gchar* initrd = gtk_entry_get_text(GTK_ENTRY(ui->new_entry_initrd));
    
    GRUBEntry entry;
    entry.title = title;
    entry.kernel = kernel;
    entry.initrd = initrd;
    
    if (ui->manager.add_entry(entry)) {
        GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui->terminal_output));
        gtk_text_buffer_insert_at_cursor(buffer, "Entry added successfully\n", -1);
        ui->refresh_all();
    }
}

void GTKUI::on_update_grub(GtkWidget* widget, gpointer data) {
    GTKUI* ui = static_cast<GTKUI*>(data);
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui->terminal_output));
    gtk_text_buffer_insert_at_cursor(buffer, "Running update-grub...\n", -1);
    
    if (ui->manager.update_grub()) {
        gtk_text_buffer_insert_at_cursor(buffer, "GRUB updated successfully\n", -1);
        ui->refresh_all();
    } else {
        gtk_text_buffer_insert_at_cursor(buffer, "Failed to update GRUB\n", -1);
    }
}

void GTKUI::on_destroy(GtkWidget* widget, gpointer data) {
    gtk_main_quit();
}
