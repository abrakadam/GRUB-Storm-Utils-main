#ifndef UI_NCURSES_H
#define UI_NCURSES_H

#include <string>
#include <vector>
#include "grub_manager.h"

#ifdef _WIN32
#include <curses.h>
#else
#include <ncurses.h>
#include <menu.h>
#include <panel.h>
#include <form.h>
#endif

// терминальный интерфейс на основе ncurses
class UINCurses {
private:
    GRUBManager& manager;  // ссылка на менеджер grub
    WINDOW *main_win;      // основное окно контента
    WINDOW *menu_win;      // окно нижнего меню
    WINDOW *info_win;      // боковая панель с деталями записи
    WINDOW *status_win;    // строка статуса (подсказки)
    MENU *menu;
    PANEL *main_panel;
    PANEL *info_panel;
    int max_x, max_y;      // текущие размеры терминала

    // инициализация и освобождение ncurses-окон
    void init_windows();
    void destroy_windows();

    // методы отрисовки элементов интерфейса
    void draw_header();
    void draw_status();
    void draw_menu();
    void draw_info_panel(const std::string& title, const std::string& content);

    // отображение различных экранов
    void show_entries_window();
    void show_entry_details(const GRUBEntry& entry);
    void show_config_window();
    void show_default_window();
    void show_grub_info_window();
    void refresh_all();  // перерисовать все панели

    // формирование строк с детальной информацией
    std::string get_grub_detailed_info();
    std::string get_entry_detailed_info(const GRUBEntry& entry);

public:
    UINCurses(GRUBManager& mgr);
    ~UINCurses();

    void run();  // запуск цикла ncurses
};

#endif // UI_NCURSES_H
