#include "ui.h"
#include <iostream>
#include <limits>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

UI::UI(GRUBManager& mgr) : manager(mgr) {}

void UI::print_header() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    
    std::cout << "========================================\n";
    std::cout << "       GRUB Storm Utils v1.0           \n";
    std::cout << "========================================\n";
    
    std::string mode = manager.get_safety_mode() == SafetyMode::SAFE ? "SAFE" : "DANGEROUS";
    std::cout << "Current Mode: " << mode << "\n";
    std::cout << "Platform: " << (manager.is_windows_platform() ? "Windows" : "Linux") << "\n";
    
    if (manager.is_windows_platform()) {
        std::cout << "\n[WARNING] GRUB is Linux-only. Limited functionality on Windows.\n";
    }
    
    std::cout << "========================================\n\n";
}

void UI::print_menu() {
    if (manager.get_safety_mode() == SafetyMode::SAFE) {
        print_safe_menu();
    } else {
        print_dangerous_menu();
    }
}

void UI::print_safe_menu() {
    std::cout << "SAFE MODE - Read Only Operations:\n";
    std::cout << "1. View GRUB Entries\n";
    std::cout << "2. View GRUB Configuration\n";
    std::cout << "3. View Default Entry\n";
    std::cout << "4. Check GRUB Installation\n";
    std::cout << "5. Get GRUB Version\n";
    std::cout << "6. Switch to DANGEROUS Mode\n";
    std::cout << "0. Exit\n";
    std::cout << "\nSelect option: ";
}

void UI::print_dangerous_menu() {
    std::cout << "DANGEROUS MODE - Write Operations:\n";
    std::cout << "[WARNING] These operations modify GRUB configuration!\n\n";
    std::cout << "1. View GRUB Entries\n";
    std::cout << "2. View GRUB Configuration\n";
    std::cout << "3. View Default Entry\n";
    std::cout << "4. Set Default Entry\n";
    std::cout << "5. Add New Entry\n";
    std::cout << "6. Remove Entry\n";
    std::cout << "7. Change Timeout\n";
    std::cout << "8. Edit Configuration\n";
    std::cout << "9. Update GRUB (run update-grub)\n";
    std::cout << "0. Exit\n";
    std::cout << "M. Switch to SAFE Mode\n";
    std::cout << "\nSelect option: ";
}

void UI::wait_for_key() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void UI::view_entries() {
    print_header();
    std::cout << "=== GRUB Entries ===\n\n";
    
    auto entries = manager.get_entries();
    if (entries.empty()) {
        std::cout << "No entries found or cannot read config.\n";
    } else {
        for (size_t i = 0; i < entries.size(); ++i) {
            std::cout << i + 1 << ". " << entries[i].title << "\n";
            if (!entries[i].kernel.empty()) {
                std::cout << "   Kernel: " << entries[i].kernel << "\n";
            }
            if (!entries[i].initrd.empty()) {
                std::cout << "   Initrd: " << entries[i].initrd << "\n";
            }
            std::cout << "\n";
        }
    }
    
    wait_for_key();
}

void UI::view_config() {
    print_header();
    std::cout << "=== GRUB Configuration ===\n\n";
    
    std::string config = manager.get_config_content();
    std::cout << config << "\n";
    
    wait_for_key();
}

void UI::view_default() {
    print_header();
    std::cout << "=== Default Entry ===\n\n";
    
    std::string def = manager.get_default_entry();
    std::cout << "Default Entry: " << def << "\n";
    
    wait_for_key();
}

void UI::change_default() {
    print_header();
    std::cout << "=== Set Default Entry ===\n\n";
    
    auto entries = manager.get_entries();
    for (size_t i = 0; i < entries.size(); ++i) {
        std::cout << i + 1 << ". " << entries[i].title << "\n";
    }
    
    std::cout << "\nEnter entry number or title: ";
    std::string choice;
    std::cin >> choice;
    
    std::string entry_title;
    try {
        int num = std::stoi(choice);
        if (num > 0 && num <= (int)entries.size()) {
            entry_title = entries[num - 1].title;
        }
    } catch (...) {
        entry_title = choice;
    }
    
    if (manager.set_default_entry(entry_title)) {
        std::cout << "Default entry set to: " << entry_title << "\n";
    } else {
        std::cout << "Failed to set default entry.\n";
    }
    
    wait_for_key();
}

void UI::add_entry_ui() {
    print_header();
    std::cout << "=== Add New Entry ===\n\n";
    
    GRUBEntry entry;
    std::cout << "Enter entry title: ";
    std::cin.ignore();
    std::getline(std::cin, entry.title);
    
    std::cout << "Enter kernel path (or leave empty): ";
    std::getline(std::cin, entry.kernel);
    
    std::cout << "Enter initrd path (or leave empty): ";
    std::getline(std::cin, entry.initrd);
    
    if (manager.add_entry(entry)) {
        std::cout << "Entry added successfully!\n";
    } else {
        std::cout << "Failed to add entry.\n";
    }
    
    wait_for_key();
}

void UI::remove_entry_ui() {
    print_header();
    std::cout << "=== Remove Entry ===\n\n";
    
    auto entries = manager.get_entries();
    for (size_t i = 0; i < entries.size(); ++i) {
        std::cout << i + 1 << ". " << entries[i].title << "\n";
    }
    
    std::cout << "\nEnter entry number to remove: ";
    int choice;
    std::cin >> choice;
    
    if (choice > 0 && choice <= (int)entries.size()) {
        if (manager.remove_entry(entries[choice - 1].title)) {
            std::cout << "Entry removed successfully!\n";
        } else {
            std::cout << "Failed to remove entry.\n";
        }
    } else {
        std::cout << "Invalid selection.\n";
    }
    
    wait_for_key();
}

void UI::change_timeout() {
    print_header();
    std::cout << "=== Change Timeout ===\n\n";
    
    std::cout << "Enter new timeout (seconds): ";
    int timeout;
    std::cin >> timeout;
    
    if (manager.update_timeout(timeout)) {
        std::cout << "Timeout set to " << timeout << " seconds.\n";
    } else {
        std::cout << "Failed to set timeout.\n";
    }
    
    wait_for_key();
}

void UI::edit_config() {
    print_header();
    std::cout << "=== Edit Configuration ===\n\n";
    std::cout << "This will open the config in your default editor.\n";
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
    std::cin.get();
    
    std::string editor_cmd;
#ifdef _WIN32
    editor_cmd = "notepad C:\\grub\\grub.cfg";
#else
    editor_cmd = "sudo nano /boot/grub/grub.cfg";
#endif
    
    system(editor_cmd.c_str());
}

void UI::update_grub_ui() {
    print_header();
    std::cout << "=== Update GRUB ===\n\n";
    std::cout << "This will run update-grub command.\n";
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
    std::cin.get();
    
    if (manager.update_grub()) {
        std::cout << "GRUB updated successfully!\n";
    } else {
        std::cout << "Failed to update GRUB.\n";
    }
    
    wait_for_key();
}

void UI::change_mode() {
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
    
    wait_for_key();
}

void UI::run() {
    while (true) {
        print_header();
        print_menu();
        
        std::string choice;
        std::cin >> choice;
        
        if (choice == "0") {
            std::cout << "Goodbye!\n";
            break;
        } else if (choice == "1") {
            view_entries();
        } else if (choice == "2") {
            view_config();
        } else if (choice == "3") {
            view_default();
        } else if (choice == "4") {
            print_header();
            std::cout << "=== GRUB Installation Check ===\n\n";
            std::cout << "GRUB Installed: " << (manager.check_grub_installed() ? "Yes" : "No") << "\n";
            wait_for_key();
        } else if (choice == "5") {
            if (manager.get_safety_mode() == SafetyMode::SAFE) {
                print_header();
                std::cout << "=== GRUB Version ===\n\n";
                std::cout << manager.get_grub_version() << "\n";
                wait_for_key();
            } else {
                change_default();
            }
        } else if (choice == "6") {
            if (manager.get_safety_mode() == SafetyMode::DANGEROUS) {
                add_entry_ui();
            } else {
                change_mode();
            }
        } else if (choice == "7") {
            if (manager.get_safety_mode() == SafetyMode::DANGEROUS) {
                remove_entry_ui();
            }
        } else if (choice == "8") {
            if (manager.get_safety_mode() == SafetyMode::DANGEROUS) {
                edit_config();
            }
        } else if (choice == "9") {
            if (manager.get_safety_mode() == SafetyMode::DANGEROUS) {
                update_grub_ui();
            }
        } else if (choice == "M" || choice == "m") {
            if (manager.get_safety_mode() == SafetyMode::DANGEROUS) {
                change_mode();
            }
        } else {
            std::cout << "Invalid option. Press Enter to continue...";
            std::cin.ignore();
            std::cin.get();
        }
    }
}
