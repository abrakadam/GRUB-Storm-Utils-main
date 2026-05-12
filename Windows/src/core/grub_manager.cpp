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
    return (getuid() == 0);
}

bool GRUBManager::backup_config() {
    if (is_windows) {
        std::string backup = grub_config_path + ".backup";
        std::ifstream src(grub_config_path, std::ios::binary);
        std::ofstream dst(backup, std::ios::binary);
        dst << src.rdbuf();
        return true;
    } else {
        std::string cmd = "cp " + grub_config_path + " " + grub_config_path + ".backup";
        return system(cmd.c_str()) == 0;
    }
}

bool GRUBManager::restore_config() {
    if (is_windows) {
        std::string backup = grub_config_path + ".backup";
        std::ifstream src(backup, std::ios::binary);
        std::ofstream dst(grub_config_path, std::ios::binary);
        dst << src.rdbuf();
        return true;
    } else {
        std::string cmd = "cp " + grub_config_path + ".backup " + grub_config_path;
        return system(cmd.c_str()) == 0;
    }
}

bool GRUBManager::check_grub_installed() {
    if (is_windows) {
        return fs::exists(grub_config_path);
    } else {
        return system("which grub-install > /dev/null 2>&1") == 0;
    }
}

std::string GRUBManager::get_grub_version() {
    if (is_windows) {
        return "N/A (Windows)";
    } else {
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
        GRUBEntry entry;
        entry.title = "Windows Boot Manager";
        entry.uuid = "windows";
        entry.kernel = "";
        entry.initrd = "";
        entry.is_default = true;
        entries.push_back(entry);
        return entries;
    }
    
    std::ifstream file(grub_config_path);
    if (!file.is_open()) {
        return entries;
    }
    
    std::string line;
    GRUBEntry current_entry;
    bool in_entry = false;
    
    while (std::getline(file, line)) {
        if (line.find("menuentry") != std::string::npos) {
            if (in_entry) {
                entries.push_back(current_entry);
            }
            current_entry = GRUBEntry();
            in_entry = true;
            
            size_t start = line.find('"') + 1;
            size_t end = line.find('"', start);
            if (start != std::string::npos && end != std::string::npos) {
                current_entry.title = line.substr(start, end - start);
            }
        } else if (in_entry) {
            if (line.find("linux") != std::string::npos || line.find("kernel") != std::string::npos) {
                size_t start = line.find(' ') + 1;
                if (start != std::string::npos) {
                    current_entry.kernel = line.substr(start);
                }
            } else if (line.find("initrd") != std::string::npos) {
                size_t start = line.find(' ') + 1;
                if (start != std::string::npos) {
                    current_entry.initrd = line.substr(start);
                }
            }
        }
    }
    
    if (in_entry) {
        entries.push_back(current_entry);
    }
    
    return entries;
}

std::string GRUBManager::get_default_entry() {
    if (is_windows) {
        return "Windows Boot Manager";
    }
    
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
    std::ifstream file(grub_config_path);
    if (!file.is_open()) {
        return "Cannot read config file";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool GRUBManager::set_default_entry(const std::string& entry) {
    if (current_mode != SafetyMode::DANGEROUS) {
        return false;
    }
    
    if (is_windows) {
        return false; // Not applicable on Windows
    }
    
    backup_config();
    
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
    if (current_mode != SafetyMode::DANGEROUS) {
        return false;
    }
    
    if (is_windows) {
        return false;
    }
    
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
    if (current_mode != SafetyMode::DANGEROUS) {
        return false;
    }
    
    if (is_windows) {
        return false;
    }
    
    backup_config();
    
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
    if (current_mode != SafetyMode::DANGEROUS) {
        return false;
    }
    
    if (is_windows) {
        return false;
    }
    
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
    if (current_mode != SafetyMode::DANGEROUS) {
        return false;
    }
    
    backup_config();
    
    std::ofstream file(grub_config_path);
    if (!file.is_open()) {
        return false;
    }
    
    file << new_config;
    return true;
}

bool GRUBManager::update_grub() {
    if (current_mode != SafetyMode::DANGEROUS) {
        return false;
    }
    
    if (is_windows) {
        return false;
    }
    
    return system("update-grub") == 0;
}

std::string GRUBManager::get_last_error() {
    return "No error";
}
