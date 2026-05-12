#include "ui_advanced.h"
#include <iostream>
#include <limits>
#include <cstdlib>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

AdvancedUI::AdvancedUI(GRUBManager& mgr) : manager(mgr) {
    sys_info = SystemInfoCollector::collect();
}

void AdvancedUI::clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void AdvancedUI::print_separator() {
    std::cout << std::string(80, '=') << std::endl;
}

void AdvancedUI::print_header() {
    clear_screen();
    print_separator();
    std::cout << "           GRUB Storm Utils v1.0 - Advanced GRUB Manager" << std::endl;
    print_separator();
    
    std::string mode = manager.get_safety_mode() == SafetyMode::SAFE ? "SAFE" : "DANGEROUS";
    std::cout << "Mode: " << mode << " | ";
    std::cout << "Platform: " << sys_info.os_name << " ";
    std::cout << "(" << sys_info.architecture << ")" << std::endl;
    std::cout << "User: " << sys_info.username << "@" << sys_info.hostname << " | ";
    std::cout << "Distro: " << sys_info.distribution << std::endl;
    print_separator();
}

void AdvancedUI::print_menu() {
    std::cout << "\nMAIN MENU:\n";
    std::cout << "1. System Information\n";
    std::cout << "2. GRUB Information\n";
    std::cout << "3. View GRUB Entries\n";
    std::cout << "4. View Entry Details\n";
    std::cout << "5. View GRUB Configuration\n";
    std::cout << "6. View Default Entry\n";
    std::cout << "7. Terminal Mode\n";
    std::cout << "8. GRUB Operations";
    if (manager.get_safety_mode() == SafetyMode::DANGEROUS) {
        std::cout << " [WRITE]";
    }
    std::cout << "\n";
    std::cout << "9. Toggle Safety Mode (Current: " 
              << (manager.get_safety_mode() == SafetyMode::SAFE ? "SAFE" : "DANGEROUS") << ")\n";
    std::cout << "0. Exit\n";
    std::cout << "\nSelect option: ";
}

void AdvancedUI::print_system_info() {
    print_header();
    std::cout << "\n=== SYSTEM INFORMATION ===\n\n";
    
    std::cout << "Operating System: " << sys_info.os_name << "\n";
    std::cout << "OS Version: " << sys_info.os_version << "\n";
    std::cout << "Kernel Version: " << sys_info.kernel_version << "\n";
    std::cout << "Architecture: " << sys_info.architecture;
    if (sys_info.is_64bit) {
        std::cout << " (64-bit)";
    } else {
        std::cout << " (32-bit)";
    }
    std::cout << "\n";
    std::cout << "Distribution: " << sys_info.distribution << "\n";
    std::cout << "Hostname: " << sys_info.hostname << "\n";
    std::cout << "Username: " << sys_info.username << "\n\n";
    
    std::cout << "=== CPU INFORMATION ===\n\n";
    std::cout << "CPU Model: " << sys_info.cpu_model << "\n";
    std::cout << "CPU Cores: " << sys_info.cpu_cores << "\n\n";
    
    std::cout << "=== MEMORY INFORMATION ===\n\n";
    std::cout << "Total Memory: " << SystemInfoCollector::format_bytes(sys_info.total_memory) << "\n";
    std::cout << "Available Memory: " << SystemInfoCollector::format_bytes(sys_info.available_memory) << "\n\n";
    
    print_separator();
}

void AdvancedUI::print_grub_info() {
    print_header();
    std::cout << "\n=== GRUB INFORMATION ===\n\n";
    
    std::cout << "GRUB Version: " << manager.get_grub_version() << "\n";
    std::cout << "GRUB Installed: " << (manager.check_grub_installed() ? "Yes" : "No") << "\n";
    std::cout << "Default Entry: " << manager.get_default_entry() << "\n\n";
    
    auto entries = manager.get_entries();
    std::cout << "Total Boot Entries: " << entries.size() << "\n\n";
    
    std::cout << "=== CONFIGURATION PATHS ===\n\n";
    if (manager.is_windows_platform()) {
        std::cout << "Config: C:\\grub\\grub.cfg\n";
        std::cout << "Default: C:\\etc\\default\\grub\n";
    } else {
        std::cout << "Config: /boot/grub/grub.cfg\n";
        std::cout << "Default: /etc/default/grub\n";
    }
    
    print_separator();
}

void AdvancedUI::print_entries_detailed() {
    print_header();
    std::cout << "\n=== GRUB BOOT ENTRIES ===\n\n";
    
    auto entries = manager.get_entries();
    if (entries.empty()) {
        std::cout << "No entries found.\n";
    } else {
        for (size_t i = 0; i < entries.size(); ++i) {
            std::cout << "[" << i + 1 << "] " << entries[i].title << "\n";
            std::cout << "    UUID: " << (entries[i].uuid.empty() ? "N/A" : entries[i].uuid) << "\n";
            std::cout << "    Kernel: " << (entries[i].kernel.empty() ? "N/A" : entries[i].kernel) << "\n";
            std::cout << "    Initrd: " << (entries[i].initrd.empty() ? "N/A" : entries[i].initrd) << "\n";
            if (entries[i].is_default) {
                std::cout << "    [DEFAULT ENTRY]\n";
            }
            std::cout << "\n";
        }
    }
    
    print_separator();
}

void AdvancedUI::print_terminal_help() {
    std::cout << "\n=== TERMINAL MODE ===\n";
    std::cout << "Type commands to execute. Available commands:\n";
    std::cout << "  help     - Show this help\n";
    std::cout << "  clear    - Clear screen\n";
    std::cout << "  exit     - Exit terminal mode\n";
    std::cout << "  sysinfo  - Show system information\n";
    std::cout << "  grub     - Show GRUB information\n";
    std::cout << "  Any other command will be executed in shell\n";
    std::cout << "\nTerminal> ";
}

void AdvancedUI::view_system_info() {
    print_system_info();
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void AdvancedUI::view_grub_entries() {
    print_entries_detailed();
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void AdvancedUI::view_entry_details() {
    print_entries_detailed();
    
    auto entries = manager.get_entries();
    if (entries.empty()) {
        std::cout << "\nPress Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
        return;
    }
    
    std::cout << "\nEnter entry number for details (1-" << entries.size() << "): ";
    int choice;
    std::cin >> choice;
    
    if (choice > 0 && choice <= (int)entries.size()) {
        const auto& entry = entries[choice - 1];
        
        print_header();
        std::cout << "\n=== DETAILED ENTRY INFORMATION ===\n\n";
        std::cout << "Title: " << entry.title << "\n";
        std::cout << "UUID: " << (entry.uuid.empty() ? "Not specified" : entry.uuid) << "\n";
        std::cout << "Kernel: " << (entry.kernel.empty() ? "Not specified" : entry.kernel) << "\n";
        std::cout << "Initrd: " << (entry.initrd.empty() ? "Not specified" : entry.initrd) << "\n";
        std::cout << "Is Default: " << (entry.is_default ? "Yes" : "No") << "\n\n";
        
        std::cout << "=== ANALYSIS ===\n";
        if (!entry.kernel.empty()) {
            std::cout << "- Kernel specified: YES\n";
            if (entry.kernel.find("vmlinuz") != std::string::npos) {
                std::cout << "- Kernel type: Linux kernel\n";
            } else if (entry.kernel.find("bzImage") != std::string::npos) {
                std::cout << "- Kernel type: Generic kernel image\n";
            }
        } else {
            std::cout << "- Kernel specified: NO (may be chainloader or other boot method)\n";
        }
        
        if (!entry.initrd.empty()) {
            std::cout << "- Initrd specified: YES\n";
        } else {
            std::cout << "- Initrd specified: NO\n";
        }
        
        print_separator();
    }
    
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void AdvancedUI::view_grub_config() {
    print_header();
    std::cout << "\n=== GRUB CONFIGURATION ===\n\n";
    
    std::string config = manager.get_config_content();
    std::cout << config << "\n";
    
    print_separator();
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void AdvancedUI::view_default_entry() {
    print_header();
    std::cout << "\n=== DEFAULT BOOT ENTRY ===\n\n";
    
    std::string def = manager.get_default_entry();
    std::cout << "Current Default: " << def << "\n\n";
    std::cout << "This entry will be booted automatically after timeout.\n";
    
    if (manager.get_safety_mode() == SafetyMode::SAFE) {
        std::cout << "\n[SAFE MODE] Switch to DANGEROUS mode to change default entry.\n";
    }
    
    print_separator();
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void AdvancedUI::execute_terminal_command(const std::string& cmd) {
    if (cmd == "help") {
        print_terminal_help();
    } else if (cmd == "clear") {
        clear_screen();
        print_terminal_help();
    } else if (cmd == "exit") {
        return;
    } else if (cmd == "sysinfo") {
        clear_screen();
        print_system_info();
        print_terminal_help();
    } else if (cmd == "grub") {
        clear_screen();
        print_grub_info();
        print_terminal_help();
    } else if (cmd.empty()) {
        print_terminal_help();
    } else {
        std::cout << "\nExecuting: " << cmd << "\n";
        std::string result = SystemInfoCollector::execute_command(cmd);
        std::cout << result << "\n";
        print_terminal_help();
    }
}

void AdvancedUI::terminal_mode() {
    print_header();
    print_terminal_help();
    
    std::string cmd;
    while (true) {
        std::cout << "Terminal> ";
        std::getline(std::cin, cmd);
        
        if (cmd == "exit") {
            break;
        }
        
        execute_terminal_command(cmd);
    }
}

void AdvancedUI::grub_operations() {
    print_header();
    
    if (manager.get_safety_mode() == SafetyMode::SAFE) {
        std::cout << "\n[SAFE MODE] Write operations disabled.\n";
        std::cout << "Switch to DANGEROUS mode to perform GRUB operations.\n";
        std::cout << "\nPress Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
        return;
    }
    
    std::cout << "\n=== GRUB OPERATIONS (DANGEROUS MODE) ===\n\n";
    std::cout << "1. Set Default Entry\n";
    std::cout << "2. Add New Entry\n";
    std::cout << "3. Remove Entry\n";
    std::cout << "4. Change Timeout\n";
    std::cout << "5. Update GRUB (update-grub)\n";
    std::cout << "0. Back to Main Menu\n";
    std::cout << "\nSelect operation: ";
    
    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    switch (choice) {
        case 1: {
            print_entries_detailed();
            auto entries = manager.get_entries();
            std::cout << "\nEnter entry number to set as default: ";
            int entry_num;
            std::cin >> entry_num;
            if (entry_num > 0 && entry_num <= (int)entries.size()) {
                if (manager.set_default_entry(entries[entry_num - 1].title)) {
                    std::cout << "Default entry set successfully!\n";
                } else {
                    std::cout << "Failed to set default entry.\n";
                }
            }
            break;
        }
        case 2: {
            GRUBEntry entry;
            std::cout << "\nEnter entry title: ";
            std::getline(std::cin, entry.title);
            std::cout << "Enter kernel path (or empty): ";
            std::getline(std::cin, entry.kernel);
            std::cout << "Enter initrd path (or empty): ";
            std::getline(std::cin, entry.initrd);
            
            if (manager.add_entry(entry)) {
                std::cout << "Entry added successfully!\n";
            } else {
                std::cout << "Failed to add entry.\n";
            }
            break;
        }
        case 3: {
            print_entries_detailed();
            auto entries = manager.get_entries();
            std::cout << "\nEnter entry number to remove: ";
            int entry_num;
            std::cin >> entry_num;
            if (entry_num > 0 && entry_num <= (int)entries.size()) {
                if (manager.remove_entry(entries[entry_num - 1].title)) {
                    std::cout << "Entry removed successfully!\n";
                } else {
                    std::cout << "Failed to remove entry.\n";
                }
            }
            break;
        }
        case 4: {
            std::cout << "\nEnter new timeout (seconds): ";
            int timeout;
            std::cin >> timeout;
            if (manager.update_timeout(timeout)) {
                std::cout << "Timeout set to " << timeout << " seconds.\n";
            } else {
                std::cout << "Failed to set timeout.\n";
            }
            break;
        }
        case 5: {
            std::cout << "\nRunning update-grub...\n";
            if (manager.update_grub()) {
                std::cout << "GRUB updated successfully!\n";
            } else {
                std::cout << "Failed to update GRUB.\n";
            }
            break;
        }
        case 0:
            return;
        default:
            std::cout << "Invalid option.\n";
    }
    
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void AdvancedUI::change_safety_mode() {
    if (manager.get_safety_mode() == SafetyMode::SAFE) {
        std::cout << "\n[WARNING] You are about to switch to DANGEROUS mode.\n";
        std::cout << "This mode allows modifications to GRUB configuration.\n";
        std::cout << "Incorrect changes may make your system unbootable!\n";
        std::cout << "\nType 'DANGEROUS' to confirm: ";
        
        std::string confirm;
        std::cin >> confirm;
        
        if (confirm == "DANGEROUS") {
            manager.set_safety_mode(SafetyMode::DANGEROUS);
            std::cout << "Switched to DANGEROUS mode.\n";
        } else {
            std::cout << "Cancelled.\n";
        }
    } else {
        manager.set_safety_mode(SafetyMode::SAFE);
        std::cout << "Switched to SAFE mode.\n";
    }
    
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void AdvancedUI::run() {
    while (true) {
        print_header();
        print_menu();
        
        std::string choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        if (choice == "0") {
            std::cout << "\nGoodbye!\n";
            break;
        } else if (choice == "1") {
            view_system_info();
        } else if (choice == "2") {
            print_grub_info();
            std::cout << "\nPress Enter to continue...";
            std::cin.get();
        } else if (choice == "3") {
            view_grub_entries();
        } else if (choice == "4") {
            view_entry_details();
        } else if (choice == "5") {
            view_grub_config();
        } else if (choice == "6") {
            view_default_entry();
        } else if (choice == "7") {
            terminal_mode();
        } else if (choice == "8") {
            grub_operations();
        } else if (choice == "9") {
            change_safety_mode();
        } else {
            std::cout << "\nInvalid option. Press Enter to continue...";
            std::cin.get();
        }
    }
}
