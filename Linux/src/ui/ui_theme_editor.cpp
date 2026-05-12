#include <gtk/gtk.h>
#include <string>
#include <vector>

// редактор тем grub в gtk3
class ui_theme_editor {
private:
    GtkWidget *window;
    GtkWidget *theme_combo;
    GtkWidget *color_button;
    GtkWidget *font_button;
    GtkWidget *preview_area;
    
    std::vector<std::string> available_themes;
    std::string current_theme;
    
public:
    ui_theme_editor() {
        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(window), "Редактор тем GRUB");
        gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
        
        create_widgets();
        setup_layout();
        connect_signals();
        
        load_available_themes();
    }
    
    ~ui_theme_editor() {
        gtk_widget_destroy(window);
    }
    
    void show() {
        gtk_widget_show_all(window);
    }
    
private:
    void create_widgets() {
        // выпадающий список тем
        theme_combo = gtk_combo_box_text_new();
        
        // кнопка выбора цвета фона
        color_button = gtk_color_button_new();
        
        // кнопка выбора шрифта
        font_button = gtk_font_button_new();
        
        // область предпросмотра
        preview_area = gtk_drawing_area_new();
        gtk_widget_set_size_request(preview_area, 400, 300);
    }
    
    void setup_layout() {
        GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        
        // верхняя панель с элементами управления
        gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new("Тема:"), FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(hbox), theme_combo, FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new("Цвет фона:"), FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(hbox), color_button, FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new("Шрифт:"), FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(hbox), font_button, FALSE, FALSE, 5);
        
        // кнопки действий
        GtkWidget *button_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        GtkWidget *apply_btn = gtk_button_new_with_label("Применить");
        GtkWidget *save_btn = gtk_button_new_with_label("Сохранить");
        GtkWidget *reset_btn = gtk_button_new_with_label("Сбросить");
        
        gtk_box_pack_start(GTK_BOX(button_hbox), apply_btn, FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(button_hbox), save_btn, FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(button_hbox), reset_btn, FALSE, FALSE, 5);
        
        // собираем всё вместе
        gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 10);
        gtk_box_pack_start(GTK_BOX(vbox), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(vbox), preview_area, TRUE, TRUE, 10);
        gtk_box_pack_start(GTK_BOX(vbox), button_hbox, FALSE, FALSE, 10);
        
        gtk_container_add(GTK_CONTAINER(window), vbox);
    }
    
    void connect_signals() {
        g_signal_connect(window, "destroy", G_CALLBACK(on_window_close), this);
        g_signal_connect(theme_combo, "changed", G_CALLBACK(on_theme_changed), this);
        g_signal_connect(color_button, "color-set", G_CALLBACK(on_color_changed), this);
        g_signal_connect(font_button, "font-set", G_CALLBACK(on_font_changed), this);
    }
    
    void load_available_themes() {
        // здесь должна быть загрузка реальных тем
        available_themes = {"default", "starfield", "dark", "cyber", "custom"};
        
        for (const auto& theme : available_themes) {
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(theme_combo), theme.c_str());
        }
    }
    
    static void on_window_close(GtkWidget *widget, gpointer data) {
        ui_theme_editor *editor = static_cast<ui_theme_editor*>(data);
        delete editor;
        gtk_main_quit();
    }
    
    static void on_theme_changed(GtkWidget *widget, gpointer data) {
        ui_theme_editor *editor = static_cast<ui_theme_editor*>(data);
        const gchar *theme_name = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
        if (theme_name) {
            editor->current_theme = theme_name;
            editor->update_preview();
        }
    }
    
    static void on_color_changed(GtkWidget *widget, gpointer data) {
        ui_theme_editor *editor = static_cast<ui_theme_editor*>(data);
        GdkRGBA color;
        gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(widget), &color);
        editor->update_preview();
    }
    
    static void on_font_changed(GtkWidget *widget, gpointer data) {
        ui_theme_editor *editor = static_cast<ui_theme_editor*>(data);
        const gchar *font_name = gtk_font_button_get_font_name(GTK_FONT_BUTTON(widget));
        editor->update_preview();
    }
    
    void update_preview() {
        // обновление предпросмотра темы
        // здесь должна быть отрисовка превью
        gtk_widget_queue_draw(preview_area);
    }
};
