#include "grub_manager.h"
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

// класс для создания и восстановления резервных копий grub
class grub_backup {
public:
    // создать резервную копию конфигурации grub
    static bool create_backup(const std::string& backup_path) {
        std::ifstream config_file("/boot/grub/grub.cfg");
        if (!config_file.is_open()) {
            return false;
        }
        
        // получаем текущую дату и время для имени файла
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream timestamp;
        timestamp << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
        
        std::string full_path = backup_path + "/grub_backup_" + timestamp.str() + ".cfg";
        
        std::ofstream backup_file(full_path);
        if (!backup_file.is_open()) {
            return false;
        }
        
        backup_file << config_file.rdbuf();
        backup_file.close();
        config_file.close();
        
        return true;
    }
    
    // восстановить из резервной копии
    static bool restore_backup(const std::string& backup_file) {
        std::ifstream backup(backup_file);
        if (!backup.is_open()) {
            return false;
        }
        
        // создаём резервную копию текущей конфигурации перед восстановлением
        create_backup("/boot/grub/backups");
        
        std::ofstream config("/boot/grub/grub.cfg");
        if (!config.is_open()) {
            return false;
        }
        
        config << backup.rdbuf();
        backup.close();
        config.close();
        
        return true;
    }
    
    // список всех резервных копий
    static std::vector<std::string> list_backups(const std::string& backup_dir) {
        std::vector<std::string> backups;
        // здесь должна быть реализация поиска файлов
        // для простоты оставим заглушку
        return backups;
    }
};
