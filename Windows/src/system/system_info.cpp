#include "system_info.h"
#include <fstream>
#include <sstream>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#endif

std::string SystemInfoCollector::execute_command(const std::string& command) {
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return "Error executing command";
    
    char buffer[128];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }
    pclose(pipe);
    
    // Remove trailing newline
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    
    return result;
}

std::string SystemInfoCollector::get_distro_info() {
#ifdef _WIN32
    return "Windows";
#else
    // Try lsb_release first
    std::string distro = execute_command("lsb_release -ds 2>/dev/null");
    if (!distro.empty() && distro.find("Command not found") == std::string::npos) {
        return distro;
    }
    
    // Try /etc/os-release
    std::ifstream file("/etc/os-release");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("PRETTY_NAME=") == 0) {
                size_t start = line.find('"') + 1;
                size_t end = line.find('"', start);
                if (start != std::string::npos && end != std::string::npos) {
                    return line.substr(start, end - start);
                }
            }
        }
    }
    
    // Try /etc/issue
    file.open("/etc/issue");
    if (file.is_open()) {
        std::string line;
        std::getline(file, line);
        if (!line.empty()) {
            return line;
        }
    }
    
    return "Unknown Linux";
#endif
}

std::string SystemInfoCollector::get_cpu_info() {
#ifdef _WIN32
    return "Unknown CPU";
#else
    std::ifstream file("/proc/cpuinfo");
    if (!file.is_open()) return "Unknown";
    
    std::string line;
    std::string model_name;
    int cores = 0;
    
    while (std::getline(file, line)) {
        if (line.find("model name") == 0) {
            size_t start = line.find(':') + 2;
            if (start != std::string::npos) {
                model_name = line.substr(start);
            }
        }
        if (line.find("processor") == 0) {
            cores++;
        }
    }
    
    if (!model_name.empty()) {
        return model_name + " (" + std::to_string(cores) + " cores)";
    }
    
    return "Unknown CPU";
#endif
}

std::string SystemInfoCollector::get_memory_info() {
#ifdef _WIN32
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    
    long total = status.ullTotalPhys / (1024 * 1024);
    long avail = status.ullAvailPhys / (1024 * 1024);
    
    return format_bytes(total * 1024 * 1024) + " total, " + 
           format_bytes(avail * 1024 * 1024) + " available";
#else
    struct sysinfo info;
    if (sysinfo(&info) != 0) {
        return "Unknown";
    }
    
    long total = info.totalram * info.mem_unit / (1024 * 1024);
    long avail = info.freeram * info.mem_unit / (1024 * 1024);
    
    return format_bytes(total * 1024 * 1024) + " total, " + 
           format_bytes(avail * 1024 * 1024) + " available";
#endif
}

bool SystemInfoCollector::is_64bit_system() {
#ifdef _WIN32
    #ifdef _WIN64
    return true;
    #else
    BOOL is64bit = FALSE;
    IsWow64Process(GetCurrentProcess(), &is64bit);
    return is64bit;
    #endif
#else
    return sizeof(void*) == 8;
#endif
}

std::string SystemInfoCollector::format_bytes(long bytes) {
    const long GB = 1024 * 1024 * 1024;
    const long MB = 1024 * 1024;
    const long KB = 1024;
    
    if (bytes >= GB) {
        return std::to_string(bytes / GB) + " GB";
    } else if (bytes >= MB) {
        return std::to_string(bytes / MB) + " MB";
    } else if (bytes >= KB) {
        return std::to_string(bytes / KB) + " KB";
    } else {
        return std::to_string(bytes) + " B";
    }
}

SystemInfo SystemInfoCollector::collect() {
    SystemInfo info;
    
#ifdef _WIN32
    info.os_name = "Windows";
    info.os_version = execute_command("ver");
    info.kernel_version = "Windows NT";
    info.architecture = is_64bit_system() ? "x86_64" : "x86";
    info.distribution = "Windows";
    
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        info.hostname = hostname;
    }
    
    info.cpu_model = get_cpu_info();
    info.cpu_cores = 0;
    info.total_memory = 0;
    info.available_memory = 0;
    
    char username[256];
    DWORD size = sizeof(username);
    if (GetUserNameA(username, &size)) {
        info.username = username;
    }
    
    info.is_64bit = is_64bit_system();
#else
    struct utsname uname_data;
    if (uname(&uname_data) == 0) {
        info.os_name = uname_data.sysname;
        info.kernel_version = uname_data.release;
        info.architecture = uname_data.machine;
        info.hostname = uname_data.nodename;
    }
    
    info.os_version = execute_command("uname -v");
    info.distribution = get_distro_info();
    info.cpu_model = get_cpu_info();
    
    // Count CPU cores
    info.cpu_cores = sysconf(_SC_NPROCESSORS_ONLN);
    
    // Memory info
    struct sysinfo mem_info;
    if (sysinfo(&mem_info) == 0) {
        info.total_memory = mem_info.totalram * mem_info.mem_unit;
        info.available_memory = mem_info.freeram * mem_info.mem_unit;
    }
    
    // Username
    char* user = getenv("USER");
    if (user) {
        info.username = user;
    }
    
    info.is_64bit = is_64bit_system();
#endif
    
    return info;
}
