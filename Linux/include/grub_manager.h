#ifndef GRUB_MANAGER_H
#define GRUB_MANAGER_H

#include <string>
#include <vector>
#include <map>

// режим безопасности: safe — только чтение, dangerous — запись разрешена
enum class SafetyMode {
    SAFE,
    DANGEROUS
};

// структура одной записи загрузки grub
struct GRUBEntry {
    std::string title;    // отображаемое название записи
    std::string uuid;     // uuid раздела
    std::string kernel;   // путь к образу ядра
    std::string initrd;   // путь к образу initrd
    bool is_default;      // является ли запись загрузочной по умолчанию
};

// основной класс управления загрузчиком grub
class GRUBManager {
private:
    SafetyMode current_mode;       // текущий режим безопасности
    std::string grub_config_path;  // путь к grub.cfg
    std::string grub_default_path; // путь к /etc/default/grub
    bool is_windows;               // признак windows-платформы

    // вспомогательные методы проверки прав
    bool check_sudo_windows();
    bool check_sudo_linux();

    // резервное копирование и восстановление конфигурации
    bool backup_config();
    bool restore_config();

public:
    GRUBManager();

    void set_safety_mode(SafetyMode mode);
    SafetyMode get_safety_mode() const;

    // безопасные операции (только чтение)
    std::vector<GRUBEntry> get_entries();
    std::string get_default_entry();
    std::string get_config_content();
    std::string get_grub_version();
    bool check_grub_installed();

    // опасные операции (требуют режима DANGEROUS)
    bool set_default_entry(const std::string& entry);
    bool add_entry(const GRUBEntry& entry);
    bool remove_entry(const std::string& title);
    bool update_timeout(int timeout);
    bool update_config(const std::string& new_config);
    bool update_grub();

    // вспомогательные методы
    std::string get_last_error();
    bool is_windows_platform() const;
};

#endif // GRUB_MANAGER_H
