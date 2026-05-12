#ifndef HARDWARE_INFO_H
#define HARDWARE_INFO_H

#include <string>
#include <vector>

class hardware_info {
public:
    struct cpu_info {
        std::string model;
        std::string vendor;
        int cores;
        int threads;
        double frequency;
        std::string architecture;
    };
    
    struct memory_info {
        unsigned long total_mb;
        unsigned long available_mb;
        unsigned long used_mb;
        double usage_percent;
    };
    
    struct disk_info {
        std::string device;
        std::string mount_point;
        unsigned long total_gb;
        unsigned long used_gb;
        unsigned long free_gb;
        std::string filesystem;
    };
    
    struct gpu_info {
        std::string vendor;
        std::string model;
        std::string driver;
        unsigned long memory_mb;
    };
    
    static cpu_info get_cpu_info();
    static memory_info get_memory_info();
    static std::vector<disk_info> get_disk_info();
    static std::vector<gpu_info> get_gpu_info();
    static std::vector<std::pair<std::string, double>> get_temperature_info();
    static double get_system_load();
};

#endif // HARDWARE_INFO_H
