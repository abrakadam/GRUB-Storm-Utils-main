#include <gtk/gtk.h>
#include <iostream>
#include <fstream>
#include <string>

// глобальные виджеты
GtkWidget *window;
GtkWidget *notebook;
GtkWidget *text_view_system;
GtkWidget *text_view_grub;
GtkWidget *text_view_entries;
GtkWidget *entry_command;
GtkWidget *text_output;

// функции обратного вызова
static void destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

static void on_refresh_clicked(GtkWidget *widget, gpointer data) {
    // обновляем информацию о системе
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view_system));
    gtk_text_buffer_set_text(buffer, "", -1);
    
    // читаем информацию о системе
    std::ifstream version_file("/proc/version");
    if (version_file.is_open()) {
        std::string version;
        std::getline(version_file, version);
        gtk_text_buffer_insert_at_cursor(buffer, "Ядро: ");
        gtk_text_buffer_insert_at_cursor(buffer, version.c_str());
        gtk_text_buffer_insert_at_cursor(buffer, "\n");
        version_file.close();
    }
    
    // читаем информацию о дистрибутиве
    std::ifstream os_release("/etc/os-release");
    if (os_release.is_open()) {
        std::string line;
        while (std::getline(os_release, line)) {
            if (line.find("PRETTY_NAME") != std::string::npos) {
                size_t start = line.find("=") + 2;
                std::string distro = line.substr(start);
                distro = distro.substr(0, distro.length() - 1);
                gtk_text_buffer_insert_at_cursor(buffer, "ОС: ");
                gtk_text_buffer_insert_at_cursor(buffer, distro.c_str());
                gtk_text_buffer_insert_at_cursor(buffer, "\n");
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
                gtk_text_buffer_insert_at_cursor(buffer, line.c_str());
                gtk_text_buffer_insert_at_cursor(buffer, "\n");
                break;
            }
        }
        meminfo.close();
    }
}

static void on_grub_info_clicked(GtkWidget *widget, gpointer data) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view_grub));
    gtk_text_buffer_set_text(buffer, "", -1);
    
    // проверка grub
    std::ifstream grub_cfg("/boot/grub/grub.cfg");
    if (grub_cfg.is_open()) {
        gtk_text_buffer_insert_at_cursor(buffer, "GRUB установлен: /boot/grub/grub.cfg найден\n");
        
        // считаем записи
        std::string line;
        int entry_count = 0;
        while (std::getline(grub_cfg, line)) {
            if (line.find("menuentry") != std::string::npos) {
                entry_count++;
            }
        }
        gtk_text_buffer_insert_at_cursor(buffer, ("Количество записей: " + std::to_string(entry_count) + "\n").c_str());
        grub_cfg.close();
    } else {
        gtk_text_buffer_insert_at_cursor(buffer, "GRUB не найден или нет прав доступа\n");
    }
    
    // версия grub
    FILE* pipe = popen("grub-install --version 2>/dev/null", "r");
    if (pipe) {
        char buffer_cmd[128];
        if (fgets(buffer_cmd, sizeof(buffer_cmd), pipe) != nullptr) {
            gtk_text_buffer_insert_at_cursor(buffer, "Версия GRUB: ");
            gtk_text_buffer_insert_at_cursor(buffer, buffer_cmd);
        }
        pclose(pipe);
    }
}

static void on_entries_clicked(GtkWidget *widget, gpointer data) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view_entries));
    gtk_text_buffer_set_text(buffer, "", -1);
    
    std::ifstream grub_cfg("/boot/grub/grub.cfg");
    if (grub_cfg.is_open()) {
        std::string line;
        int entry_num = 0;
        gtk_text_buffer_insert_at_cursor(buffer, "--- Записи загрузки GRUB ---\n\n");
        while (std::getline(grub_cfg, line)) {
            if (line.find("menuentry") != std::string::npos) {
                size_t start = line.find("'") + 1;
                size_t end = line.rfind("'");
                if (start != std::string::npos && end != std::string::npos) {
                    std::string entry = std::to_string(entry_num) + ". " + line.substr(start, end - start) + "\n";
                    gtk_text_buffer_insert_at_cursor(buffer, entry.c_str());
                    entry_num++;
                }
            }
        }
        grub_cfg.close();
    } else {
        gtk_text_buffer_insert_at_cursor(buffer, "Не удалось прочитать /boot/grub/grub.cfg\n");
    }
}

static void on_execute_clicked(GtkWidget *widget, gpointer data) {
    const gchar *command = gtk_entry_get_text(GTK_ENTRY(entry_command));
    if (strlen(command) == 0) return;
    
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_output));
    gtk_text_buffer_insert_at_cursor(buffer, "$ ");
    gtk_text_buffer_insert_at_cursor(buffer, command);
    gtk_text_buffer_insert_at_cursor(buffer, "\n");
    
    // выполняем команду
    std::string result;
    if (std::string(command) == "sysinfo") {
        result = "--- Системная информация ---\n";
        std::ifstream cpuinfo("/proc/cpuinfo");
        if (cpuinfo.is_open()) {
            std::string line;
            while (std::getline(cpuinfo, line)) {
                if (line.find("model name") != std::string::npos) {
                    result += "CPU: " + line.substr(line.find(":") + 2) + "\n";
                    break;
                }
            }
            cpuinfo.close();
        }
    } else if (std::string(command) == "help") {
        result = "Доступные команды:\n"
                "  sysinfo - информация о системе\n"
                "  grub-info - информация о GRUB\n"
                "  entries - записи загрузки\n"
                "  help - справка\n";
    } else {
        result = "Неизвестная команда: " + std::string(command) + "\n";
    }
    
    gtk_text_buffer_insert_at_cursor(buffer, result.c_str());
    gtk_text_buffer_insert_at_cursor(buffer, "\n");
    
    // очищаем поле ввода
    gtk_entry_set_text(GTK_ENTRY(entry_command), "");
    
    // прокручиваем вниз
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(text_output), &end, 0.0, FALSE, 0.0, 0.0);
}

static void activate(GtkApplication *app, gpointer user_data) {
    // создаем главное окно
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "GRUB Storm Utils");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    
    // создаем вертикальный контейнер
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    
    // создаем блокнот для вкладок
    notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 5);
    
    // === Вкладка 1: System Info ===
    GtkWidget *system_frame = gtk_frame_new("Информация о системе");
    text_view_system = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view_system), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view_system), GTK_WRAP_WORD);
    
    GtkWidget *scrolled_system = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_system), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_system), text_view_system);
    gtk_container_add(GTK_CONTAINER(system_frame), scrolled_system);
    
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), system_frame, gtk_label_new("System Info"));
    
    // === Вкладка 2: GRUB Info ===
    GtkWidget *grub_frame = gtk_frame_new("Информация о GRUB");
    text_view_grub = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view_grub), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view_grub), GTK_WRAP_WORD);
    
    GtkWidget *scrolled_grub = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_grub), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_grub), text_view_grub);
    gtk_container_add(GTK_CONTAINER(grub_frame), scrolled_grub);
    
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), grub_frame, gtk_label_new("GRUB Info"));
    
    // === Вкладка 3: Entries ===
    GtkWidget *entries_frame = gtk_frame_new("Записи загрузки");
    text_view_entries = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view_entries), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view_entries), GTK_WRAP_WORD);
    
    GtkWidget *scrolled_entries = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_entries), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_entries), text_view_entries);
    gtk_container_add(GTK_CONTAINER(entries_frame), scrolled_entries);
    
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), entries_frame, gtk_label_new("Entries"));
    
    // === Вкладка 4: Terminal ===
    GtkWidget *terminal_frame = gtk_frame_new("Терминал");
    GtkWidget *terminal_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(terminal_frame), terminal_vbox);
    
    // область вывода
    text_output = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_output), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_output), GTK_WRAP_WORD);
    
    GtkWidget *scrolled_output = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_output), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_output), text_output);
    gtk_box_pack_start(GTK_BOX(terminal_vbox), scrolled_output, TRUE, TRUE, 5);
    
    // панель ввода
    GtkWidget *input_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(terminal_vbox), input_hbox, FALSE, FALSE, 5);
    
    entry_command = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_command), "Введите команду...");
    gtk_box_pack_start(GTK_BOX(input_hbox), entry_command, TRUE, TRUE, 5);
    
    GtkWidget *execute_btn = gtk_button_new_with_label("Выполнить");
    gtk_box_pack_start(GTK_BOX(input_hbox), execute_btn, FALSE, FALSE, 5);
    
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), terminal_frame, gtk_label_new("Terminal"));
    
    // === Панель кнопок ===
    GtkWidget *button_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), button_hbox, FALSE, FALSE, 5);
    
    GtkWidget *refresh_btn = gtk_button_new_with_label("Обновить");
    GtkWidget *grub_info_btn = gtk_button_new_with_label("GRUB Info");
    GtkWidget *entries_btn = gtk_button_new_with_label("Entries");
    
    gtk_box_pack_start(GTK_BOX(button_hbox), refresh_btn, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(button_hbox), grub_info_btn, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(button_hbox), entries_btn, FALSE, FALSE, 5);
    
    // подключаем сигналы
    g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);
    g_signal_connect(refresh_btn, "clicked", G_CALLBACK(on_refresh_clicked), NULL);
    g_signal_connect(grub_info_btn, "clicked", G_CALLBACK(on_grub_info_clicked), NULL);
    g_signal_connect(entries_btn, "clicked", G_CALLBACK(on_entries_clicked), NULL);
    g_signal_connect(execute_btn, "clicked", G_CALLBACK(on_execute_clicked), NULL);
    g_signal_connect(entry_command, "activate", G_CALLBACK(on_execute_clicked), NULL);
    
    // показываем все виджеты
    gtk_widget_show_all(window);
    
    // автоматически загружаем информацию
    on_refresh_clicked(NULL, NULL);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;
    
    app = gtk_application_new("org.grubstorm.utils", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    
    return status;
}
