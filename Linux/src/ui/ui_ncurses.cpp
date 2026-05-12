#include "ui_ncurses.h"
#include <sstream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

UINCurses::UINCurses(GRUBManager& mgr) : manager(mgr), main_win(nullptr), menu_win(nullptr), 
                                          info_win(nullptr), status_win(nullptr), menu(nullptr),
                                          main_panel(nullptr), info_panel(nullptr) {
}

UINCurses::~UINCurses() {
    destroy_windows();
}

void UINCurses::init_windows() {
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    init_pair(1, COLOR_WHITE, COLOR_BLUE);      // Header
    init_pair(2, COLOR_BLACK, COLOR_WHITE);      // Menu
    init_pair(3, COLOR_GREEN, COLOR_BLACK);      // Info
    init_pair(4, COLOR_RED, COLOR_BLACK);        // Warning
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);     // Highlight
    init_pair(6, COLOR_CYAN, COLOR_BLACK);       // Status
    
    getmaxyx(stdscr, max_y, max_x);
    
    // Main window
    main_win = newwin(max_y - 4, max_x, 0, 0);
    main_panel = new_panel(main_win);
    
    // Menu window (bottom)
    menu_win = newwin(3, max_x, max_y - 3, 0);
    
    // Status window (bottom line)
    status_win = newwin(1, max_x, max_y - 1, 0);
    
    // Info window (right side)
    info_win = newwin(max_y - 6, max_x / 2, 2, max_x / 2);
    info_panel = new_panel(info_win);
    hide_panel(info_panel);
}

void UINCurses::destroy_windows() {
    if (menu) {
        unpost_menu(menu);
        free_menu(menu);
    }
    
    if (info_panel) del_panel(info_panel);
    if (main_panel) del_panel(main_panel);
    
    if (info_win) delwin(info_win);
    if (menu_win) delwin(menu_win);
    if (status_win) delwin(status_win);
    if (main_win) delwin(main_win);
    
    endwin();
}

void UINCurses::draw_header() {
    werase(main_win);
    wbkgd(main_win, COLOR_PAIR(1));
    
    std::string title = "GRUB Storm Utils v1.0 - GRUB Configuration Manager";
    std::string mode = manager.get_safety_mode() == SafetyMode::SAFE ? "SAFE MODE" : "DANGEROUS MODE";
    std::string platform = manager.is_windows_platform() ? "Windows" : "Linux";
    
    mvwprintw(main_win, 0, (max_x - title.length()) / 2, "%s", title.c_str());
    mvwprintw(main_win, 1, 2, "Mode: %s", mode.c_str());
    mvwprintw(main_win, 1, max_x - platform.length() - 2, "%s", platform.c_str());
    
    if (manager.is_windows_platform()) {
        wattron(main_win, COLOR_PAIR(4));
        mvwprintw(main_win, 2, 2, "[WARNING] GRUB is Linux-only. Limited functionality on Windows.");
        wattroff(main_win, COLOR_PAIR(4));
    }
    
    wrefresh(main_win);
}

void UINCurses::draw_status() {
    werase(status_win);
    wbkgd(status_win, COLOR_PAIR(6));
    
    std::string status = "Press ESC to quit | Use arrow keys to navigate";
    mvwprintw(status_win, 0, 2, "%s", status.c_str());
    
    wrefresh(status_win);
}

void UINCurses::draw_menu() {
    werase(menu_win);
    wbkgd(menu_win, COLOR_PAIR(2));
    
    const char* menu_items[] = {
        "1. View Entries",
        "2. Entry Details",
        "3. View Config",
        "4. Default Entry",
        "5. GRUB Info",
        "6. Toggle Mode",
        "0. Exit"
    };
    
    int start_x = 2;
    for (int i = 0; i < 7; i++) {
        mvwprintw(menu_win, 1, start_x + i * 15, "%s", menu_items[i]);
    }
    
    wrefresh(menu_win);
}

void UINCurses::draw_info_panel(const std::string& title, const std::string& content) {
    werase(info_win);
    wbkgd(info_win, COLOR_PAIR(3));
    box(info_win, 0, 0);
    
    mvwprintw(info_win, 0, 2, "%s", title.c_str());
    
    int line = 1;
    int col = 2;
    int max_col = getmaxx(info_win) - 4;
    
    for (char c : content) {
        if (c == '\n') {
            line++;
            col = 2;
        } else {
            if (col >= max_col) {
                line++;
                col = 2;
            }
            mvwaddch(info_win, line, col++, c);
        }
        
        if (line >= getmaxy(info_win) - 2) break;
    }
    
    show_panel(info_panel);
    wrefresh(info_win);
    update_panels();
    doupdate();
}

void UINCurses::refresh_all() {
    draw_header();
    draw_menu();
    draw_status();
    update_panels();
    doupdate();
}

std::string UINCurses::get_grub_detailed_info() {
    std::stringstream info;
    
    info << "=== GRUB DETAILED INFORMATION ===\n\n";
    
    // GRUB Version
    info << "GRUB Version:\n";
    info << manager.get_grub_version() << "\n\n";
    
    // Installation Status
    info << "Installation Status:\n";
    info << (manager.check_grub_installed() ? "Installed" : "Not Installed") << "\n\n";
    
    // Config Path
    info << "Configuration Path:\n";
    if (manager.is_windows_platform()) {
        info << "C:\\grub\\grub.cfg\n";
    } else {
        info << "/boot/grub/grub.cfg\n";
        info << "/etc/default/grub\n";
    }
    info << "\n";
    
    // Default Entry
    info << "Default Boot Entry:\n";
    info << manager.get_default_entry() << "\n\n";
    
    // Entries Count
    auto entries = manager.get_entries();
    info << "Total Boot Entries: " << entries.size() << "\n\n";
    
    // System Info
    info << "System Information:\n";
    info << "Platform: " << (manager.is_windows_platform() ? "Windows" : "Linux") << "\n";
    info << "Current Mode: " << (manager.get_safety_mode() == SafetyMode::SAFE ? "SAFE" : "DANGEROUS") << "\n\n";
    
    // Safety Warning
    if (manager.get_safety_mode() == SafetyMode::DANGEROUS) {
        info << "[WARNING] DANGEROUS MODE ACTIVE\n";
        info << "Modifications to GRUB configuration can make your system unbootable!\n";
    }
    
    return info.str();
}

std::string UINCurses::get_entry_detailed_info(const GRUBEntry& entry) {
    std::stringstream info;
    
    info << "=== ENTRY DETAILED INFORMATION ===\n\n";
    
    info << "Entry Title:\n";
    info << entry.title << "\n\n";
    
    info << "UUID:\n";
    info << (entry.uuid.empty() ? "Not specified" : entry.uuid) << "\n\n";
    
    info << "Kernel Path:\n";
    info << (entry.kernel.empty() ? "Not specified" : entry.kernel) << "\n\n";
    
    info << "Initrd Path:\n";
    info << (entry.initrd.empty() ? "Not specified" : entry.initrd) << "\n\n";
    
    info << "Is Default:\n";
    info << (entry.is_default ? "Yes" : "No") << "\n\n";
    
    // Additional analysis
    info << "Analysis:\n";
    if (!entry.kernel.empty()) {
        info << "- Kernel specified: YES\n";
        if (entry.kernel.find("vmlinuz") != std::string::npos) {
            info << "- Kernel type: Linux kernel\n";
        }
    } else {
        info << "- Kernel specified: NO (may be chainloader or other)\n";
    }
    
    if (!entry.initrd.empty()) {
        info << "- Initrd specified: YES\n";
    } else {
        info << "- Initrd specified: NO\n";
    }
    
    return info.str();
}

void UINCurses::show_entries_window() {
    auto entries = manager.get_entries();
    
    werase(main_win);
    wbkgd(main_win, COLOR_PAIR(3));
    box(main_win, 0, 0);
    
    mvwprintw(main_win, 0, 2, " Boot Entries ");
    
    int line = 2;
    mvwprintw(main_win, line++, 2, "Total Entries: %d", (int)entries.size());
    line++;
    
    for (size_t i = 0; i < entries.size(); i++) {
        wattron(main_win, COLOR_PAIR(5));
        mvwprintw(main_win, line++, 2, "%zu. %s", i + 1, entries[i].title.c_str());
        wattroff(main_win, COLOR_PAIR(5));
        
        if (!entries[i].kernel.empty()) {
            mvwprintw(main_win, line++, 4, "Kernel: %s", entries[i].kernel.c_str());
        }
        if (!entries[i].initrd.empty()) {
            mvwprintw(main_win, line++, 4, "Initrd: %s", entries[i].initrd.c_str());
        }
        
        if (entries[i].is_default) {
            wattron(main_win, COLOR_PAIR(5));
            mvwprintw(main_win, line++, 4, "[DEFAULT]");
            wattroff(main_win, COLOR_PAIR(5));
        }
        
        line++;
        
        if (line >= max_y - 6) break;
    }
    
    wrefresh(main_win);
}

void UINCurses::show_entry_details(const GRUBEntry& entry) {
    std::string info = get_entry_detailed_info(entry);
    draw_info_panel("Entry Details", info);
}

void UINCurses::show_config_window() {
    std::string config = manager.get_config_content();
    
    werase(main_win);
    wbkgd(main_win, COLOR_PAIR(3));
    box(main_win, 0, 0);
    
    mvwprintw(main_win, 0, 2, " GRUB Configuration ");
    
    int line = 1;
    int col = 2;
    int max_col = max_x - 4;
    
    for (char c : config) {
        if (c == '\n') {
            line++;
            col = 2;
        } else {
            if (col >= max_col) {
                line++;
                col = 2;
            }
            mvwaddch(main_win, line, col++, c);
        }
        
        if (line >= max_y - 6) break;
    }
    
    wrefresh(main_win);
}

void UINCurses::show_default_window() {
    std::string def = manager.get_default_entry();
    
    werase(main_win);
    wbkgd(main_win, COLOR_PAIR(3));
    box(main_win, 0, 0);
    
    mvwprintw(main_win, 0, 2, " Default Boot Entry ");
    
    mvwprintw(main_win, 2, 2, "Current Default Entry:");
    wattron(main_win, COLOR_PAIR(5));
    mvwprintw(main_win, 3, 4, "%s", def.c_str());
    wattroff(main_win, COLOR_PAIR(5));
    
    mvwprintw(main_win, 5, 2, "This entry will be booted by default.");
    mvwprintw(main_win, 6, 2, "Press 'M' to change this in DANGEROUS mode.");
    
    wrefresh(main_win);
}

void UINCurses::show_grub_info_window() {
    std::string info = get_grub_detailed_info();
    
    werase(main_win);
    wbkgd(main_win, COLOR_PAIR(3));
    box(main_win, 0, 0);
    
    mvwprintw(main_win, 0, 2, " GRUB Information ");
    
    int line = 1;
    int col = 2;
    int max_col = max_x - 4;
    
    for (char c : info) {
        if (c == '\n') {
            line++;
            col = 2;
        } else {
            if (col >= max_col) {
                line++;
                col = 2;
            }
            mvwaddch(main_win, line, col++, c);
        }
        
        if (line >= max_y - 6) break;
    }
    
    wrefresh(main_win);
}

void UINCurses::run() {
    init_windows();
    
    int current_view = 0; // 0: entries, 1: config, 2: default, 3: info
    int selected_entry = 0;
    
    while (true) {
        refresh_all();
        
        switch (current_view) {
            case 0:
                show_entries_window();
                break;
            case 1:
                show_config_window();
                break;
            case 2:
                show_default_window();
                break;
            case 3:
                show_grub_info_window();
                break;
        }
        
        int ch = getch();
        
        switch (ch) {
            case '1': {
                current_view = 0;
                hide_panel(info_panel);
                break;
            }
            case '2': {
                current_view = 0;
                auto entries = manager.get_entries();
                if (!entries.empty()) {
                    show_entry_details(entries[selected_entry % entries.size()]);
                }
                break;
            }
            case '3': {
                current_view = 1;
                hide_panel(info_panel);
                break;
            }
            case '4': {
                current_view = 2;
                hide_panel(info_panel);
                break;
            }
            case '5': {
                current_view = 3;
                hide_panel(info_panel);
                break;
            }
            case '6': {
                manager.set_safety_mode(
                    manager.get_safety_mode() == SafetyMode::SAFE ? 
                    SafetyMode::DANGEROUS : SafetyMode::SAFE
                );
                break;
            }
            case '0':
            case 27: { // ESC
                destroy_windows();
                return;
            }
            case KEY_UP: {
                if (selected_entry > 0) selected_entry--;
                break;
            }
            case KEY_DOWN: {
                selected_entry++;
                break;
            }
            case KEY_RIGHT: {
                hide_panel(info_panel);
                break;
            }
        }
        
        update_panels();
        doupdate();
    }
}
