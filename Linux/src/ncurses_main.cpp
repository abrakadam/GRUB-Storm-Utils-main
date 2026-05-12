#include <ncurses.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>

// цвета и константы
#define COLOR_HIGHLIGHT 1
#define COLOR_NORMAL 2
#define COLOR_BORDER 3

// функции
void init_ncurses();
void draw_main_window();
void draw_system_info();
void draw_grub_info();
void draw_entries();
void draw_terminal();
void show_status(const std::string& message);

int main() {
    init_ncurses();
    
    int choice = 0;
    bool running = true;
    
    while (running) {
        draw_main_window();
        
        // получаем ввод пользователя
        int ch = getch();
        
        switch (ch) {
            case '1':
                draw_system_info();
                break;
            case '2':
                draw_grub_info();
                break;
            case '3':
                draw_entries();
                break;
            case '4':
                draw_terminal();
                break;
            case '5':
            case 'q':
            case 27: // ESC
                running = false;
                break;
            case KEY_UP:
                choice = (choice > 1) ? choice - 1 : 5;
                break;
            case KEY_DOWN:
                choice = (choice < 5) ? choice + 1 : 1;
                break;
            case 10: // Enter
                switch (choice) {
                    case 1:
                        draw_system_info();
                        break;
                    case 2:
                        draw_grub_info();
                        break;
                    case 3:
                        draw_entries();
                        break;
                    case 4:
                        draw_terminal();
                        break;
                    case 5:
                        running = false;
                        break;
                }
                break;
        }
    }
    
    endwin();
    return 0;
}

void init_ncurses() {
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    // инициализация цветов
    init_pair(COLOR_HIGHLIGHT, COLOR_BLACK, COLOR_WHITE);
    init_pair(COLOR_NORMAL, COLOR_WHITE, COLOR_BLUE);
    init_pair(COLOR_BORDER, COLOR_CYAN, COLOR_BLACK);
    
    if (has_colors() == FALSE) {
        endwin();
        std::cout << "Ваш терминал не поддерживает цвета\n";
        exit(1);
    }
}

void draw_main_window() {
    clear();
    
    // заголовок
    attron(COLOR_PAIR(COLOR_HIGHLIGHT));
    for (int i = 0; i < COLS; i++) {
        mvaddch(0, i, ' ');
    }
    mvprintw(0, (COLS - 20) / 2, "GRUB Storm Utils");
    attroff(COLOR_PAIR(COLOR_HIGHLIGHT));
    
    // рамка
    attron(COLOR_PAIR(COLOR_BORDER));
    box(stdscr, 0, 0);
    attroff(COLOR_PAIR(COLOR_BORDER));
    
    // меню
    int menu_start_y = (LINES - 7) / 2;
    int menu_start_x = (COLS - 30) / 2;
    
    mvprintw(menu_start_y - 2, menu_start_x, "=== Главное меню ===");
    
    const char* menu_items[] = {
        "1. Информация о системе",
        "2. Информация о GRUB", 
        "3. Записи загрузки",
        "4. Терминал",
        "5. Выход"
    };
    
    for (int i = 0; i < 5; i++) {
        mvprintw(menu_start_y + i, menu_start_x, "%s", menu_items[i]);
    }
    
    // статусная строка
    attron(COLOR_PAIR(COLOR_NORMAL));
    for (int i = 0; i < COLS; i++) {
        mvaddch(LINES - 1, i, ' ');
    }
    mvprintw(LINES - 1, 1, "Используйте стрелки и Enter для навигации, q для выхода");
    attroff(COLOR_PAIR(COLOR_NORMAL));
    
    refresh();
}

void draw_system_info() {
    clear();
    
    // заголовок
    attron(COLOR_PAIR(COLOR_HIGHLIGHT));
    for (int i = 0; i < COLS; i++) {
        mvaddch(0, i, ' ');
    }
    mvprintw(0, (COLS - 18) / 2, "Информация о системе");
    attroff(COLOR_PAIR(COLOR_HIGHLIGHT));
    
    // рамка
    attron(COLOR_PAIR(COLOR_BORDER));
    box(stdscr, 0, 0);
    attroff(COLOR_PAIR(COLOR_BORDER));
    
    int y = 3;
    int x = 2;
    
    // информация о ядре
    mvprintw(y++, x, "Ядро: ");
    std::ifstream version_file("/proc/version");
    if (version_file.is_open()) {
        std::string version;
        std::getline(version_file, version);
        if (version.length() > COLS - 10) {
            version = version.substr(0, COLS - 13) + "...";
        }
        mvprintw(y - 1, x + 7, "%s", version.c_str());
        version_file.close();
    }
    y++;
    
    // информация о дистрибутиве
    mvprintw(y++, x, "ОС: ");
    std::ifstream os_release("/etc/os-release");
    if (os_release.is_open()) {
        std::string line;
        while (std::getline(os_release, line)) {
            if (line.find("PRETTY_NAME") != std::string::npos) {
                size_t start = line.find("=") + 2;
                std::string distro = line.substr(start);
                distro = distro.substr(0, distro.length() - 1);
                if (distro.length() > COLS - 10) {
                    distro = distro.substr(0, COLS - 13) + "...";
                }
                mvprintw(y - 1, x + 4, "%s", distro.c_str());
                break;
            }
        }
        os_release.close();
    }
    y++;
    
    // информация о памяти
    mvprintw(y++, x, "Память: ");
    std::ifstream meminfo("/proc/meminfo");
    if (meminfo.is_open()) {
        std::string line;
        while (std::getline(meminfo, line)) {
            if (line.find("MemTotal") != std::string::npos) {
                mvprintw(y - 1, x + 9, "%s", line.c_str());
                break;
            }
        }
        meminfo.close();
    }
    y++;
    
    // информация о CPU
    mvprintw(y++, x, "CPU: ");
    std::ifstream cpuinfo("/proc/cpuinfo");
    if (cpuinfo.is_open()) {
        std::string line;
        while (std::getline(cpuinfo, line)) {
            if (line.find("model name") != std::string::npos) {
                std::string cpu = line.substr(line.find(":") + 2);
                if (cpu.length() > COLS - 10) {
                    cpu = cpu.substr(0, COLS - 13) + "...";
                }
                mvprintw(y - 1, x + 5, "%s", cpu.c_str());
                break;
            }
        }
        cpuinfo.close();
    }
    
    // статусная строка
    attron(COLOR_PAIR(COLOR_NORMAL));
    for (int i = 0; i < COLS; i++) {
        mvaddch(LINES - 1, i, ' ');
    }
    mvprintw(LINES - 1, 1, "Нажмите любую клавишу для возврата в меню");
    attroff(COLOR_PAIR(COLOR_NORMAL));
    
    refresh();
    getch();
}

void draw_grub_info() {
    clear();
    
    // заголовок
    attron(COLOR_PAIR(COLOR_HIGHLIGHT));
    for (int i = 0; i < COLS; i++) {
        mvaddch(0, i, ' ');
    }
    mvprintw(0, (COLS - 16) / 2, "Информация о GRUB");
    attroff(COLOR_PAIR(COLOR_HIGHLIGHT));
    
    // рамка
    attron(COLOR_PAIR(COLOR_BORDER));
    box(stdscr, 0, 0);
    attroff(COLOR_PAIR(COLOR_BORDER));
    
    int y = 3;
    int x = 2;
    
    // проверка grub
    std::ifstream grub_cfg("/boot/grub/grub.cfg");
    if (grub_cfg.is_open()) {
        mvprintw(y++, x, "GRUB установлен: /boot/grub/grub.cfg найден");
        
        // считаем записи
        std::string line;
        int entry_count = 0;
        while (std::getline(grub_cfg, line)) {
            if (line.find("menuentry") != std::string::npos) {
                entry_count++;
            }
        }
        mvprintw(y++, x, "Количество записей: %d", entry_count);
        grub_cfg.close();
    } else {
        mvprintw(y++, x, "GRUB не найден или нет прав доступа");
    }
    y++;
    
    // версия grub
    FILE* pipe = popen("grub-install --version 2>/dev/null", "r");
    if (pipe) {
        char buffer[128];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            mvprintw(y++, x, "Версия GRUB: %s", buffer);
        }
        pclose(pipe);
    }
    
    // статусная строка
    attron(COLOR_PAIR(COLOR_NORMAL));
    for (int i = 0; i < COLS; i++) {
        mvaddch(LINES - 1, i, ' ');
    }
    mvprintw(LINES - 1, 1, "Нажмите любую клавишу для возврата в меню");
    attroff(COLOR_PAIR(COLOR_NORMAL));
    
    refresh();
    getch();
}

void draw_entries() {
    clear();
    
    // заголовок
    attron(COLOR_PAIR(COLOR_HIGHLIGHT));
    for (int i = 0; i < COLS; i++) {
        mvaddch(0, i, ' ');
    }
    mvprintw(0, (COLS - 18) / 2, "Записи загрузки GRUB");
    attroff(COLOR_PAIR(COLOR_HIGHLIGHT));
    
    // рамка
    attron(COLOR_PAIR(COLOR_BORDER));
    box(stdscr, 0, 0);
    attroff(COLOR_PAIR(COLOR_BORDER));
    
    int y = 3;
    int x = 2;
    
    std::ifstream grub_cfg("/boot/grub/grub.cfg");
    if (grub_cfg.is_open()) {
        std::string line;
        int entry_num = 0;
        while (std::getline(grub_cfg, line) && y < LINES - 3) {
            if (line.find("menuentry") != std::string::npos) {
                size_t start = line.find("'") + 1;
                size_t end = line.rfind("'");
                if (start != std::string::npos && end != std::string::npos) {
                    std::string entry = std::to_string(entry_num) + ". " + line.substr(start, end - start);
                    if (entry.length() > COLS - 5) {
                        entry = entry.substr(0, COLS - 8) + "...";
                    }
                    mvprintw(y++, x, "%s", entry.c_str());
                    entry_num++;
                }
            }
        }
        grub_cfg.close();
    } else {
        mvprintw(y++, x, "Не удалось прочитать /boot/grub/grub.cfg");
    }
    
    // статусная строка
    attron(COLOR_PAIR(COLOR_NORMAL));
    for (int i = 0; i < COLS; i++) {
        mvaddch(LINES - 1, i, ' ');
    }
    mvprintw(LINES - 1, 1, "Нажмите любую клавишу для возврата в меню");
    attroff(COLOR_PAIR(COLOR_NORMAL));
    
    refresh();
    getch();
}

void draw_terminal() {
    clear();
    
    // заголовок
    attron(COLOR_PAIR(COLOR_HIGHLIGHT));
    for (int i = 0; i < COLS; i++) {
        mvaddch(0, i, ' ');
    }
    mvprintw(0, (COLS - 12) / 2, "Терминал");
    attroff(COLOR_PAIR(COLOR_HIGHLIGHT));
    
    // рамка
    attron(COLOR_PAIR(COLOR_BORDER));
    box(stdscr, 0, 0);
    attroff(COLOR_PAIR(COLOR_BORDER));
    
    int y = 3;
    int x = 2;
    
    mvprintw(y++, x, "Доступные команды:");
    mvprintw(y++, x, "  sysinfo - информация о системе");
    mvprintw(y++, x, "  grub-info - информация о GRUB");
    mvprintw(y++, x, "  entries - записи загрузки");
    mvprintw(y++, x, "  help - справка");
    mvprintw(y++, x, "  exit - выход из терминала");
    y++;
    
    // простой ввод команд
    echo();
    curs_set(1);
    
    std::string command;
    while (true) {
        mvprintw(y, x, "grub-storm> ");
        move(y, x + 12);
        
        char input[256];
        getnstr(input, sizeof(input));
        command = input;
        
        if (command == "exit" || command == "quit") {
            break;
        } else if (command == "help") {
            mvprintw(++y, x + 2, "Доступные команды: sysinfo, grub-info, entries, help, exit");
        } else if (command == "sysinfo") {
            mvprintw(++y, x + 2, "--- Системная информация ---");
            std::ifstream loadavg("/proc/loadavg");
            if (loadavg.is_open()) {
                std::string load;
                std::getline(loadavg, load);
                mvprintw(++y, x + 2, "Загрузка: %s", load.substr(0, load.find(" ")).c_str());
                loadavg.close();
            }
        } else if (!command.empty()) {
            mvprintw(++y, x + 2, "Неизвестная команда: %s", command.c_str());
        }
        
        y++;
        if (y >= LINES - 3) {
            // очищаем и начинаем заново
            clear();
            attron(COLOR_PAIR(COLOR_HIGHLIGHT));
            for (int i = 0; i < COLS; i++) {
                mvaddch(0, i, ' ');
            }
            mvprintw(0, (COLS - 12) / 2, "Терминал");
            attroff(COLOR_PAIR(COLOR_HIGHLIGHT));
            attron(COLOR_PAIR(COLOR_BORDER));
            box(stdscr, 0, 0);
            attroff(COLOR_PAIR(COLOR_BORDER));
            y = 3;
        }
    }
    
    noecho();
    curs_set(0);
}
