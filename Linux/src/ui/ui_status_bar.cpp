#include <gtk/gtk.h>
#include <string>
#include <chrono>
#include <thread>

// статус-бар для отображения состояния системы
class ui_status_bar {
private:
    GtkWidget *status_bar;
    GtkWidget *status_label;
    GtkWidget *progress_bar;
    GtkWidget *status_icon;
    
    std::string current_status;
    int progress_value;
    
public:
    ui_status_bar() {
        create_widgets();
        setup_layout();
        progress_value = 0;
        current_status = "готово";
    }
    
    ~ui_status_bar() {
        // gtk автоматически очистит виджеты
    }
    
    GtkWidget* get_widget() {
        return status_bar;
    }
    
    void set_status(const std::string& status) {
        current_status = status;
        gtk_label_set_text(GTK_LABEL(status_label), status.c_str());
        update_status_icon(status);
    }
    
    void set_progress(int value) {
        progress_value = value;
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), value / 100.0);
    }
    
    void show_operation(const std::string& operation) {
        set_status("выполнение: " + operation);
        set_progress(0);
        
        // имитация выполнения операции
        std::thread([this, operation]() {
            for (int i = 0; i <= 100; i += 5) {
                set_progress(i);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            set_status(operation + " завершено");
        }).detach();
    }
    
    void show_error(const std::string& error) {
        set_status("ошибка: " + error);
        set_progress(0);
        // можно добавить всплывающее окно с ошибкой
    }
    
    void show_success(const std::string& message) {
        set_status("успешно: " + message);
        set_progress(100);
    }
    
private:
    void create_widgets() {
        status_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        status_label = gtk_label_new("готово");
        progress_bar = gtk_progress_bar_new();
        status_icon = gtk_image_new_from_icon_name("dialog-ok", GTK_ICON_SIZE_MENU);
        
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), "");
        gtk_widget_set_size_request(progress_bar, 200, -1);
    }
    
    void setup_layout() {
        gtk_box_pack_start(GTK_BOX(status_bar), status_icon, FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(status_bar), status_label, FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(status_bar), gtk_separator_new(GTK_ORIENTATION_VERTICAL), FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(status_bar), progress_bar, TRUE, TRUE, 5);
        
        // добавляем отступы
        gtk_widget_set_margin_top(status_bar, 5);
        gtk_widget_set_margin_bottom(status_bar, 5);
        gtk_widget_set_margin_left(status_bar, 10);
        gtk_widget_set_margin_right(status_bar, 10);
    }
    
    void update_status_icon(const std::string& status) {
        const gchar *icon_name = "dialog-ok";
        
        if (status.find("ошибка") != std::string::npos) {
            icon_name = "dialog-error";
        } else if (status.find("выполнение") != std::string::npos) {
            icon_name = "dialog-information";
        } else if (status.find("предупреждение") != std::string::npos) {
            icon_name = "dialog-warning";
        }
        
        gtk_image_set_from_icon_name(GTK_IMAGE(status_icon), icon_name, GTK_ICON_SIZE_MENU);
    }
};
