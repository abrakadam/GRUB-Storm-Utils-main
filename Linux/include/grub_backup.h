#ifndef GRUB_BACKUP_H
#define GRUB_BACKUP_H

#include <string>
#include <vector>

class grub_backup {
public:
    static bool create_backup(const std::string& backup_path);
    static bool restore_backup(const std::string& backup_file);
    static std::vector<std::string> list_backups(const std::string& backup_dir);
};

#endif // GRUB_BACKUP_H
