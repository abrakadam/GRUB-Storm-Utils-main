#ifndef PROCESS_INFO_H
#define PROCESS_INFO_H

#include <string>
#include <vector>

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
    
    struct system_stats {
        int total_processes;
        int running_processes;
        int sleeping_processes;
        double total_cpu_usage;
        unsigned long total_memory_mb;
        unsigned long used_memory_mb;
    };
    
    static std::vector<process> get_all_processes();
    static process get_process_info(int pid);
    static std::vector<process> get_processes_by_name(const std::string& name);
    static bool is_process_running(const std::string& name);
    static bool kill_process(int pid, int signal = 15);
    static std::vector<process> get_top_cpu_processes(int limit = 10);
    static std::vector<process> get_top_memory_processes(int limit = 10);
    static system_stats get_system_stats();
};

#endif // PROCESS_INFO_H
