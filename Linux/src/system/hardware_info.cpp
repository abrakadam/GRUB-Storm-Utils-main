#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// класс для получения информации о железе
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
    
    // получить информацию о процессоре
    static cpu_info get_cpu_info() {
        cpu_info info;
        
        // читаем /proc/cpuinfo
        std::ifstream cpuinfo("/proc/cpuinfo");
        std::string line;
        
        while (std::getline(cpuinfo, line)) {
            if (line.find("model name") != std::string::npos) {
                size_t start = line.find(":") + 2;
                info.model = line.substr(start);
            }
            if (line.find("vendor_id") != std::string::npos) {
                size_t start = line.find(":") + 2;
                info.vendor = line.substr(start);
            }
            if (line.find("cpu cores") != std::string::npos) {
                size_t start = line.find(":") + 2;
                info.cores = std::stoi(line.substr(start));
            }
        }
        
        // получаем количество потоков
        info.threads = std::thread::hardware_concurrency();
        
        // получаем частоту
        std::ifstream freq_file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq");
        if (freq_file.is_open()) {
            std::string freq_str;
            std::getline(freq_file, freq_str);
            info.frequency = std::stod(freq_str) / 1000000.0; // переводим в GHz
        }
        
        // архитектура
        std::ifstream arch_file("/proc/cpuinfo");
        if (arch_file.is_open()) {
            info.architecture = "x86_64"; // по умолчанию
        }
        
        return info;
    }
    
    // получить информацию о памяти
    static memory_info get_memory_info() {
        memory_info info;
        
        std::ifstream meminfo("/proc/meminfo");
        std::string line;
        
        while (std::getline(meminfo, line)) {
            if (line.find("MemTotal") != std::string::npos) {
                size_t start = line.find(":") + 2;
                std::string value = line.substr(start);
                info.total_mb = std::stoul(value) / 1024; // переводим в MB
            }
            if (line.find("MemAvailable") != std::string::npos) {
                size_t start = line.find(":") + 2;
                std::string value = line.substr(start);
                info.available_mb = std::stoul(value) / 1024;
            }
        }
        
        info.used_mb = info.total_mb - info.available_mb;
        info.usage_percent = (double)info.used_mb / info.total_mb * 100.0;
        
        return info;
    }
    
    // получить информацию о дисках
    static std::vector<disk_info> get_disk_info() {
        std::vector<disk_info> disks;
        
        // читаем /proc/mounts
        std::ifstream mounts("/proc/mounts");
        std::string line;
        
        while (std::getline(mounts, line)) {
            std::istringstream iss(line);
            disk_info disk;
            
            iss >> disk.device;
            iss >> disk.mount_point;
            iss >> disk.filesystem;
            
            // пропускаем виртуальные файловые системы
            if (disk.device.substr(0, 7) == "/dev/sd" || disk.device.substr(0, 7) == "/dev/hd") {
                // получаем информацию о размере через statvfs
                struct statvfs vfs;
                if (statvfs(disk.mount_point.c_str(), &vfs) == 0) {
                    unsigned long block_size = vfs.f_bsize;
                    unsigned long total_blocks = vfs.f_blocks;
                    unsigned long free_blocks = vfs.f_bavail;
                    
                    disk.total_gb = (total_blocks * block_size) / (1024 * 1024 * 1024);
                    disk.free_gb = (free_blocks * block_size) / (1024 * 1024 * 1024);
                    disk.used_gb = disk.total_gb - disk.free_gb;
                    
                    disks.push_back(disk);
                }
            }
        }
        
        return disks;
    }
    
    // получить информацию о gpu
    static std::vector<gpu_info> get_gpu_info() {
        std::vector<gpu_info> gpus;
        
        // пробуем получить информацию через lspci
        FILE* pipe = popen("lspci | grep -i vga", "r");
        if (pipe) {
            char buffer[128];
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                std::string line(buffer);
                gpu_info gpu;
                
                // парсинг информации о gpu
                if (line.find("NVIDIA") != std::string::npos) {
                    gpu.vendor = "NVIDIA";
                } else if (line.find("AMD") != std::string::npos || line.find("Radeon") != std::string::npos) {
                    gpu.vendor = "AMD";
                } else if (line.find("Intel") != std::string::npos) {
                    gpu.vendor = "Intel";
                } else {
                    gpu.vendor = "Unknown";
                }
                
                // извлекаем модель
                size_t start = line.find(": ") + 2;
                if (start != std::string::npos) {
                    gpu.model = line.substr(start);
                    gpu.model.erase(gpu.model.find_last_not_of("\n") + 1);
                }
                
                gpus.push_back(gpu);
            }
            pclose(pipe);
        }
        
        return gpus;
    }
    
    // получить температуру системы
    static std::vector<std::pair<std::string, double>> get_temperature_info() {
        std::vector<std::pair<std::string, double>> temps;
        
        // читаем температуру из /sys/class/thermal/
        std::ifstream thermal_zone("/sys/class/thermal/thermal_zone0/temp");
        if (thermal_zone.is_open()) {
            std::string temp_str;
            std::getline(thermal_zone, temp_str);
            double temp = std::stod(temp_str) / 1000.0; // переводим в градусы Цельсия
            temps.push_back({"CPU", temp});
        }
        
        return temps;
    }
    
    // получить информацию о загрузке системы
    static double get_system_load() {
        std::ifstream loadavg("/proc/loadavg");
        if (loadavg.is_open()) {
            std::string load_str;
            std::getline(loadavg, load_str);
            size_t space_pos = loadavg.find(" ");
            if (space_pos != std::string::npos) {
                return std::stod(load_str.substr(0, space_pos));
            }
        }
        return 0.0;
    }
};
