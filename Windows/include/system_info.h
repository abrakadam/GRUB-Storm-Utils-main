#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include <string>
#include <map>

// структура с информацией о системе
struct SystemInfo {
    std::string os_name;        // название операционной системы
    std::string os_version;     // версия ос
    std::string kernel_version; // версия ядра / nt-версия на windows
    std::string architecture;   // архитектура (x86, x86_64 и т.д.)
    std::string distribution;   // дистрибутив linux или «windows»
    std::string hostname;       // имя хоста
    std::string cpu_model;      // модель процессора
    int cpu_cores;              // количество ядер
    long total_memory;          // общий объём памяти (байты)
    long available_memory;      // доступный объём памяти (байты)
    std::string username;       // имя текущего пользователя
    bool is_64bit;              // признак 64-битной системы
};

// класс для сбора информации о системе
class SystemInfoCollector {
public:
    static SystemInfo collect();                                    // собрать всю информацию
    static std::string execute_command(const std::string& command); // выполнить команду и вернуть вывод
    static std::string get_distro_info();                           // определить дистрибутив
    static std::string get_cpu_info();                              // информация о процессоре
    static std::string get_memory_info();                           // информация о памяти
    static bool is_64bit_system();                                  // проверка разрядности
    static std::string format_bytes(long bytes);                    // форматировать байты в читаемый вид
};

#endif // SYSTEM_INFO_H
