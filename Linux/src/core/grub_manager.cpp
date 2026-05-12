#include "grub_manager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#endif

namespace fs = std::filesystem;

GRUBManager::GRUBManager() {
    current_mode = SafetyMode::SAFE;

    // устанавливаем пути к файлам конфигурации в зависимости от платформы
#ifdef _WIN32
    is_windows = true;
    grub_config_path = "C:\\grub\\grub.cfg";
    grub_default_path = "C:\\etc\\default\\grub";
#else
    is_windows = false;
    grub_config_path = "/boot/grub/grub.cfg";
    grub_default_path = "/etc/default/grub";
#endif
}

void GRUBManager::set_safety_mode(SafetyMode mode) {
    current_mode = mode;
}

SafetyMode GRUBManager::get_safety_mode() const {
    return current_mode;
}

bool GRUBManager::is_windows_platform() const {
    return is_windows;
}

bool GRUBManager::check_sudo_windows() {
#ifdef _WIN32
    // проверяем членство текущего процесса в группе администраторов
    BOOL isAdmin = FALSE;
    PSID adminGroup = NULL;

    SID_IDENTIFIER_AUTHORITY auth = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&auth, 2, SECURITY_BUILTIN_DOMAIN_RID,
                                DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup)) {
        if (!CheckTokenMembership(NULL, adminGroup, &isAdmin)) {
            isAdmin = FALSE;
        }
        FreeSid(adminGroup);
    }

    return isAdmin;
#else
    return false;
#endif
}

bool GRUBManager::check_sudo_linux() {
    // на linux root определяется по uid == 0
    return (getuid() == 0);
}

bool GRUBManager::backup_config() {
    if (is_windows) {
        // на windows копируем файл через стандартные потоки
        std::string backup = grub_config_path + ".backup";
        std::ifstream src(grub_config_path, std::ios::binary);
        std::ofstream dst(backup, std::ios::binary);
        dst << src.rdbuf();
        return true;
    } else {
        // на linux используем системную команду cp
        std::string cmd = "cp " + grub_config_path + " " + grub_config_path + ".backup";
        return system(cmd.c_str()) == 0;
    }
}

bool GRUBManager::restore_config() {
    if (is_windows) {
        // восстанавливаем из резервной копии через потоки
        std::string backup = grub_config_path + ".backup";
        std::ifstream src(backup, std::ios::binary);
        std::ofstream dst(grub_config_path, std::ios::binary);
        dst << src.rdbuf();
        return true;
    } else {
        // на linux восстанавливаем через cp
        std::string cmd = "cp " + grub_config_path + ".backup " + grub_config_path;
        return system(cmd.c_str()) == 0;
    }
}

bool GRUBManager::check_grub_installed() {
    if (is_windows) {
        // на windows просто проверяем существование файла конфигурации
        return fs::exists(grub_config_path);
    } else {
        // на linux проверяем наличие grub-install в PATH
        return system("which grub-install > /dev/null 2>&1") == 0;
    }
}

std::string GRUBManager::get_grub_version() {
    if (is_windows) {
        return "N/A (Windows)";
    } else {
        // получаем версию через grub-install --version
        FILE* pipe = popen("grub-install --version", "r");
        if (!pipe) return "Unknown";

        char buffer[128];
        std::string result;
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            result += buffer;
        }
        pclose(pipe);
        return result;
    }
}

std::vector<GRUBEntry> GRUBManager::get_entries() {
    std::vector<GRUBEntry> entries;

    if (is_windows) {
        // на windows grub не используется — возвращаем заглушку
        GRUBEntry entry;
        entry.title = "Windows Boot Manager";
        entry.uuid = "windows";
        entry.kernel = "";
        entry.initrd = "";
        entry.is_default = true;
        entries.push_back(entry);
        return entries;
    }

    // разбираем grub.cfg и извлекаем все menuentry-блоки
    std::ifstream file(grub_config_path);
    if (!file.is_open()) {
        return entries;
    }

    std::string line;
    GRUBEntry current_entry;
    bool in_entry = false;

    while (std::getline(file, line)) {
        if (line.find("menuentry") != std::string::npos) {
            // сохраняем предыдущую запись, если была
            if (in_entry) {
                entries.push_back(current_entry);
            }
            current_entry = GRUBEntry();
            in_entry = true;

            // извлекаем название записи из кавычек
            size_t start = line.find('"') + 1;
            size_t end = line.find('"', start);
            if (start != std::string::npos && end != std::string::npos) {
                current_entry.title = line.substr(start, end - start);
            }
        } else if (in_entry) {
            // извлекаем путь к ядру
            if (line.find("linux") != std::string::npos || line.find("kernel") != std::string::npos) {
                size_t start = line.find(' ') + 1;
                if (start != std::string::npos) {
                    current_entry.kernel = line.substr(start);
                }
            // извлекаем путь к initrd
            } else if (line.find("initrd") != std::string::npos) {
                size_t start = line.find(' ') + 1;
                if (start != std::string::npos) {
                    current_entry.initrd = line.substr(start);
                }
            }
        }
    }

    // добавляем последнюю запись
    if (in_entry) {
        entries.push_back(current_entry);
    }

    return entries;
}

std::string GRUBManager::get_default_entry() {
    if (is_windows) {
        return "Windows Boot Manager";
    }

    // читаем GRUB_DEFAULT из /etc/default/grub
    std::ifstream file(grub_default_path);
    if (!file.is_open()) {
        return "Unknown";
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.find("GRUB_DEFAULT=") != std::string::npos) {
            size_t start = line.find('"') + 1;
            size_t end = line.find('"', start);
            if (start != std::string::npos && end != std::string::npos) {
                return line.substr(start, end - start);
            }
        }
    }

    return "0";
}

std::string GRUBManager::get_config_content() {
    // читаем полное содержимое grub.cfg
    std::ifstream file(grub_config_path);
    if (!file.is_open()) {
        return "Cannot read config file";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool GRUBManager::set_default_entry(const std::string& entry) {
    // операция требует режима DANGEROUS
    if (current_mode != SafetyMode::DANGEROUS) {
        return false;
    }

    if (is_windows) {
        return false; // неприменимо на windows
    }

    // создаём резервную копию перед изменением
    backup_config();

    // заменяем строку GRUB_DEFAULT в /etc/default/grub
    std::ifstream file(grub_default_path);
    std::vector<std::string> lines;
    std::string line;

    while (std::getline(file, line)) {
        if (line.find("GRUB_DEFAULT=") != std::string::npos) {
            lines.push_back("GRUB_DEFAULT=\"" + entry + "\"");
        } else {
            lines.push_back(line);
        }
    }
    file.close();

    std::ofstream out(grub_default_path);
    for (const auto& l : lines) {
        out << l << "\n";
    }

    return true;
}

bool GRUBManager::add_entry(const GRUBEntry& entry) {
    // операция требует режима DANGEROUS
    if (current_mode != SafetyMode::DANGEROUS) {
        return false;
    }

    if (is_windows) {
        return false;
    }

    // создаём резервную копию и дописываем новую запись в конец grub.cfg
    backup_config();

    std::ofstream file(grub_config_path, std::ios::app);
    if (!file.is_open()) {
        return false;
    }

    file << "\nmenuentry \"" << entry.title << "\" {\n";
    if (!entry.kernel.empty()) {
        file << "    linux " << entry.kernel << "\n";
    }
    if (!entry.initrd.empty()) {
        file << "    initrd " << entry.initrd << "\n";
    }
    file << "}\n";

    return true;
}

bool GRUBManager::remove_entry(const std::string& title) {
    // операция требует режима DANGEROUS
    if (current_mode != SafetyMode::DANGEROUS) {
        return false;
    }

    if (is_windows) {
        return false;
    }

    // создаём резервную копию перед удалением
    backup_config();

    // читаем файл, пропуская нужный menuentry-блок
    std::ifstream file(grub_config_path);
    std::vector<std::string> lines;
    std::string line;
    bool skip = false;
    int brace_count = 0;

    while (std::getline(file, line)) {
        if (line.find("menuentry \"" + title + "\"") != std::string::npos) {
            skip = true;
            brace_count = 1;
            continue;
        }

        if (skip) {
            // отслеживаем вложенность фигурных скобок
            if (line.find("{") != std::string::npos) brace_count++;
            if (line.find("}") != std::string::npos) brace_count--;
            if (brace_count == 0) skip = false;
            continue;
        }

        lines.push_back(line);
    }
    file.close();

    std::ofstream out(grub_config_path);
    for (const auto& l : lines) {
        out << l << "\n";
    }

    return true;
}

bool GRUBManager::update_timeout(int timeout) {
    // операция требует режима DANGEROUS
    if (current_mode != SafetyMode::DANGEROUS) {
        return false;
    }

    if (is_windows) {
        return false;
    }

    // создаём резервную копию и обновляем GRUB_TIMEOUT
    backup_config();

    std::ifstream file(grub_default_path);
    std::vector<std::string> lines;
    std::string line;

    while (std::getline(file, line)) {
        if (line.find("GRUB_TIMEOUT=") != std::string::npos) {
            lines.push_back("GRUB_TIMEOUT=" + std::to_string(timeout));
        } else {
            lines.push_back(line);
        }
    }
    file.close();

    std::ofstream out(grub_default_path);
    for (const auto& l : lines) {
        out << l << "\n";
    }

    return true;
}

bool GRUBManager::update_config(const std::string& new_config) {
    // операция требует режима DANGEROUS
    if (current_mode != SafetyMode::DANGEROUS) {
        return false;
    }

    // сохраняем резервную копию и перезаписываем grub.cfg
    backup_config();

    std::ofstream file(grub_config_path);
    if (!file.is_open()) {
        return false;
    }

    file << new_config;
    return true;
}

bool GRUBManager::update_grub() {
    // операция требует режима DANGEROUS
    if (current_mode != SafetyMode::DANGEROUS) {
        return false;
    }

    if (is_windows) {
        return false;
    }

    // запускаем update-grub для пересборки grub.cfg
    return system("update-grub") == 0;
}

std::string GRUBManager::get_last_error() {
    return "No error";
}
