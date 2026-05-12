#include <gtk/gtk.h>
#include <string>
#include <vector>

// консольный виджет для выполнения команд
class ui_console {
private:
    GtkWidget *console_window;
    GtkWidget *text_view;
    GtkWidget *entry;
    GtkWidget *scrollbar;
    GtkTextBuffer *text_buffer;
    
    std::vector<std::string> command_history;
    int history_index;
    
public:
    ui_console() {
        history_index = -1;
        create_widgets();
        setup_layout();
        connect_signals();
        
        add_welcome_message();
    }
    
    ~ui_console() {
        gtk_widget_destroy(console_window);
    }
    
    GtkWidget* get_widget() {
        return console_window;
    }
    
    void execute_command(const std::string& command) {
        // добавляем команду в историю
        command_history.push_back(command);
        history_index = command_history.size();
        
        // отображаем команду
        append_text("$ " + command + "\n");
        
        // выполняем команду
        std::string output = execute_system_command(command);
        append_text(output + "\n");
        
        // прокручиваем вниз
        scroll_to_bottom();
    }
    
    void clear_console() {
        gtk_text_buffer_set_text(text_buffer, "", 0);
        add_welcome_message();
    }
    
private:
    void create_widgets() {
        console_window = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        text_view = gtk_text_view_new();
        entry = gtk_entry_new();
        scrollbar = gtk_scrolled_window_new(NULL, NULL);
        
        text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
        
        // настройки текстового поля
        gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
        gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), FALSE);
        gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);
        
        // настройки скроллбара
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollbar), 
                                      GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
        
        // настройки поля ввода
        gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "введите команду...");
    }
    
    void setup_layout() {
        // добавляем текстовое поле в скроллбар
        gtk_container_add(GTK_CONTAINER(scrollbar), text_view);
        
        // собираем интерфейс
        gtk_box_pack_start(GTK_BOX(console_window), scrollbar, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(console_window), entry, FALSE, FALSE, 0);
        
        // устанавливаем размеры
        gtk_widget_set_size_request(console_window, 600, 400);
    }
    
    void connect_signals() {
        g_signal_connect(entry, "key-press-event", G_CALLBACK(on_key_press), this);
        g_signal_connect(console_window, "destroy", G_CALLBACK(on_destroy), this);
    }
    
    void add_welcome_message() {
        append_text("=== GRUB Storm Utils Консоль ===\n");
        append_text("Введите 'help' для списка команд\n\n");
    }
    
    void append_text(const std::string& text) {
        GtkTextIter end;
        gtk_text_buffer_get_end_iter(text_buffer, &end);
        gtk_text_buffer_insert(text_buffer, &end, text.c_str(), -1);
    }
    
    void scroll_to_bottom() {
        GtkTextIter end;
        gtk_text_buffer_get_end_iter(text_buffer, &end);
        gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(text_view), &end, 0.0, FALSE, 0.0, 0.0);
    }
    
    std::string execute_system_command(const std::string& command) {
        // здесь должна быть реальная执行 команды
        // для примера вернём заглушку
        if (command == "help") {
            return "доступные команды:\n"
                   "  sysinfo - информация о системе\n"
                   "  grub-info - информация о grub\n"
                   "  entries - список записей\n"
                   "  clear - очистить консоль";
        } else if (command == "clear") {
            clear_console();
            return "";
        } else if (command == "sysinfo") {
            return "система: linux\n"
                   "ядро: 5.15.0\n"
                   "архитектура: x86_64";
        } else if (command == "grub-info") {
            return "версия grub: 2.06\n"
                   "установлен на: /dev/sda\n"
                   "конфиг: /boot/grub/grub.cfg";
        } else if (command == "entries") {
            return "записи загрузки:\n"
                   "0. Ubuntu\n"
                   "1. Windows 10\n"
                   "2. Advanced options";
        } else {
            return "неизвестная команда: " + command;
        }
    }
    
    static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data) {
        ui_console *console = static_cast<ui_console*>(data);
        
        if (event->keyval == GDK_KEY_Return) {
            const gchar *text = gtk_entry_get_text(GTK_ENTRY(widget));
            if (strlen(text) > 0) {
                console->execute_command(text);
                gtk_entry_set_text(GTK_ENTRY(widget), "");
            }
            return TRUE;
        } else if (event->keyval == GDK_KEY_Up) {
            console->navigate_history(-1);
            return TRUE;
        } else if (event->keyval == GDK_KEY_Down) {
            console->navigate_history(1);
            return TRUE;
        }
        
        return FALSE;
    }
    
    void navigate_history(int direction) {
        if (command_history.empty()) return;
        
        history_index += direction;
        
        if (history_index < 0) {
            history_index = 0;
        } else if (history_index >= (int)command_history.size()) {
            history_index = command_history.size() - 1;
        }
        
        gtk_entry_set_text(GTK_ENTRY(entry), command_history[history_index].c_str());
    }
    
    static void on_destroy(GtkWidget *widget, gpointer data) {
        // очистка при уничтожении
    }
};
