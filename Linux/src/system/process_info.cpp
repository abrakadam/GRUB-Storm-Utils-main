#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

// класс для получения информации о процессах
class process_info {
public:
    struct process {
        int pid;
        std::string name;
        std::string user;
        double cpu_percent;
        unsigned long memory_mb;
        std::string state;
        std::string command;
        int parent_pid;
    };
    
    // получить список всех процессов
    static std::vector<process> get_all_processes() {
        std::vector<process> processes;
        
        DIR *proc_dir = opendir("/proc");
        if (proc_dir == nullptr) {
            return processes;
        }
        
        struct dirent *entry;
        while ((entry = readdir(proc_dir)) != nullptr) {
            // проверяем, является ли имя директории числом (pid)
            if (isdigit(entry->d_name[0])) {
                int pid = std::stoi(entry->d_name);
                process proc = get_process_info(pid);
                if (proc.pid > 0) {
                    processes.push_back(proc);
                }
            }
        }
        
        closedir(proc_dir);
        return processes;
    }
    
    // получить информацию о конкретном процессе
    static process get_process_info(int pid) {
        process proc;
        proc.pid = pid;
        
        // читаем /proc/[pid]/stat
        std::string stat_file = "/proc/" + std::to_string(pid) + "/stat";
        std::ifstream stat(stat_file);
        if (stat.is_open()) {
            std::string line;
            std::getline(stat, line);
            std::istringstream iss(line);
            
            std::string name, state, ppid;
            iss >> proc.pid >> name >> state >> ppid;
            
            // удаляем скобки из имени
            if (name.front() == '(' && name.back() == ')') {
                proc.name = name.substr(1, name.length() - 2);
            } else {
                proc.name = name;
            }
            
            proc.state = state;
            proc.parent_pid = std::stoi(ppid);
        }
        
        // читаем /proc/[pid]/cmdline
        std::string cmdline_file = "/proc/" + std::to_string(pid) + "/cmdline";
        std::ifstream cmdline(cmdline_file);
        if (cmdline.is_open()) {
            std::string line;
            std::getline(cmdline, line);
            
            // заменяем нулевые байты на пробелы
            for (char& c : line) {
                if (c == '\0') c = ' ';
            }
            
            if (!line.empty()) {
                proc.command = line;
            } else {
                proc.command = "[" + proc.name + "]";
            }
        }
        
        // получаем информацию о памяти
        proc.memory_mb = get_process_memory(pid);
        
        // получаем информацию о пользователе
        proc.user = get_process_user(pid);
        
        // получаем загрузку cpu (упрощённая версия)
        proc.cpu_percent = get_process_cpu(pid);
        
        return proc;
    }
    
    // получить процессы по имени
    static std::vector<process> get_processes_by_name(const std::string& name) {
        std::vector<process> all_processes = get_all_processes();
        std::vector<process> filtered;
        
        for (const auto& proc : all_processes) {
            if (proc.name.find(name) != std::string::npos) {
                filtered.push_back(proc);
            }
        }
        
        return filtered;
    }
    
    // проверить, запущен ли процесс
    static bool is_process_running(const std::string& name) {
        auto processes = get_processes_by_name(name);
        return !processes.empty();
    }
    
    // завершить процесс
    static bool kill_process(int pid, int signal = SIGTERM) {
        return kill(pid, signal) == 0;
    }
    
    // получить топ процессов по загрузке cpu
    static std::vector<process> get_top_cpu_processes(int limit = 10) {
        auto processes = get_all_processes();
        
        // сортируем по загрузке cpu
        std::sort(processes.begin(), processes.end(), 
                 [](const process& a, const process& b) {
                     return a.cpu_percent > b.cpu_percent;
                 });
        
        // возвращаем первые limit процессов
        if (processes.size() > limit) {
            processes.resize(limit);
        }
        
        return processes;
    }
    
    // получить топ процессов по использованию памяти
    static std::vector<process> get_top_memory_processes(int limit = 10) {
        auto processes = get_all_processes();
        
        // сортируем по использованию памяти
        std::sort(processes.begin(), processes.end(), 
                 [](const process& a, const process& b) {
                     return a.memory_mb > b.memory_mb;
                 });
        
        // возвращаем первые limit процессов
        if (processes.size() > limit) {
            processes.resize(limit);
        }
        
        return processes;
    }
    
    // получить системную статистику
    static struct {
        int total_processes;
        int running_processes;
        int sleeping_processes;
        double total_cpu_usage;
        unsigned long total_memory_mb;
        unsigned long used_memory_mb;
    } get_system_stats() {
        struct {
            int total_processes;
            int running_processes;
            int sleeping_processes;
            double total_cpu_usage;
            unsigned long total_memory_mb;
            unsigned long used_memory_mb;
        } stats = {0, 0, 0, 0.0, 0, 0};
        
        auto processes = get_all_processes();
        stats.total_processes = processes.size();
        
        for (const auto& proc : processes) {
            if (proc.state == "R") {
                stats.running_processes++;
            } else if (proc.state == "S") {
                stats.sleeping_processes++;
            }
            
            stats.total_cpu_usage += proc.cpu_percent;
            stats.used_memory_mb += proc.memory_mb;
        }
        
        // получаем общую память
        std::ifstream meminfo("/proc/meminfo");
        std::string line;
        while (std::getline(meminfo, line)) {
            if (line.find("MemTotal") != std::string::npos) {
                size_t start = line.find(":") + 2;
                std::string value = line.substr(start);
                stats.total_memory_mb = std::stoul(value) / 1024;
                break;
            }
        }
        
        return stats;
    }
    
private:
    static unsigned long get_process_memory(int pid) {
        std::string status_file = "/proc/" + std::to_string(pid) + "/status";
        std::ifstream status(status_file);
        
        if (status.is_open()) {
            std::string line;
            while (std::getline(status, line)) {
                if (line.find("VmRSS") != std::string::npos) {
                    size_t start = line.find(":") + 2;
                    std::string value = line.substr(start);
                    // удаляем " kB"
                    value = value.substr(0, value.find(" "));
                    return std::stoul(value) / 1024; // переводим в MB
                }
            }
        }
        
        return 0;
    }
    
    static std::string get_process_user(int pid) {
        std::string status_file = "/proc/" + std::to_string(pid) + "/status";
        std::ifstream status(status_file);
        
        if (status.is_open()) {
            std::string line;
            while (std::getline(status, line)) {
                if (line.find("Uid") != std::string::npos) {
                    size_t start = line.find(":") + 2;
                    std::string uid_str = line.substr(start);
                    uid_str = uid_str.substr(0, uid_str.find("\t"));
                    
                    int uid = std::stoi(uid_str);
                    return get_username_by_uid(uid);
                }
            }
        }
        
        return "unknown";
    }
    
    static std::string get_username_by_uid(int uid) {
        struct passwd *pw = getpwuid(uid);
        if (pw != nullptr) {
            return pw->pw_name;
        }
        return std::to_string(uid);
    }
    
    static double get_process_cpu(int pid) {
        // упрощённая реализация - в реальности нужно отслеживать изменения во времени
        // здесь просто возвращаем случайное значение для примера
        return (double)(rand() % 100) / 10.0;
    }
};
