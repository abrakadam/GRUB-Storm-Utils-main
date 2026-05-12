#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/statvfs.h>

// класс для получения информации о сети
class network_info {
public:
    struct interface_info {
        std::string name;
        std::string ip_address;
        std::string netmask;
        std::string mac_address;
        bool is_up;
        std::string type; // ethernet, wifi, loopback
    };
    
    struct connection_info {
        std::string protocol;
        std::string local_address;
        std::string local_port;
        std::string remote_address;
        std::string remote_port;
        std::string state;
    };
    
    // получить информацию о сетевых интерфейсах
    static std::vector<interface_info> get_interface_info() {
        std::vector<interface_info> interfaces;
        
        struct ifaddrs *ifaddrs_ptr;
        if (getifaddrs(&ifaddrs_ptr) == -1) {
            return interfaces;
        }
        
        for (struct ifaddrs *ifa = ifaddrs_ptr; ifa != nullptr; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == nullptr) continue;
            
            interface_info info;
            info.name = ifa->ifa_name;
            info.is_up = (ifa->ifa_flags & IFF_UP) != 0;
            
            // определяем тип интерфейса
            if (info.name == "lo") {
                info.type = "loopback";
            } else if (info.name.find("wl") != std::string::npos) {
                info.type = "wifi";
            } else if (info.name.find("eth") != std::string::npos || info.name.find("en") != std::string::npos) {
                info.type = "ethernet";
            } else {
                info.type = "unknown";
            }
            
            // получаем ip адрес
            if (ifa->ifa_addr->sa_family == AF_INET) {
                struct sockaddr_in* addr_in = (struct sockaddr_in*)ifa->ifa_addr;
                char ip_str[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(addr_in->sin_addr), ip_str, INET_ADDRSTRLEN);
                info.ip_address = ip_str;
                
                // получаем маску подсети
                if (ifa->ifa_netmask != nullptr) {
                    struct sockaddr_in* mask_in = (struct sockaddr_in*)ifa->ifa_netmask;
                    char mask_str[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &(mask_in->sin_addr), mask_str, INET_ADDRSTRLEN);
                    info.netmask = mask_str;
                }
            }
            
            // получаем mac адрес
            info.mac_address = get_mac_address(info.name);
            
            // добавляем только уникальные интерфейсы
            bool found = false;
            for (const auto& existing : interfaces) {
                if (existing.name == info.name) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                interfaces.push_back(info);
            }
        }
        
        freeifaddrs(ifaddrs_ptr);
        return interfaces;
    }
    
    // получить mac адрес интерфейса
    static std::string get_mac_address(const std::string& interface_name) {
        std::ifstream mac_file("/sys/class/net/" + interface_name + "/address");
        if (mac_file.is_open()) {
            std::string mac;
            std::getline(mac_file, mac);
            return mac;
        }
        return "unknown";
    }
    
    // получить информацию о подключениях
    static std::vector<connection_info> get_connections() {
        std::vector<connection_info> connections;
        
        // читаем tcp подключения
        connections = get_connections_from_file("/proc/net/tcp", "TCP");
        
        // читаем udp подключения
        auto udp_connections = get_connections_from_file("/proc/net/udp", "UDP");
        connections.insert(connections.end(), udp_connections.begin(), udp_connections.end());
        
        return connections;
    }
    
    // получить информацию о dns
    static std::vector<std::string> get_dns_servers() {
        std::vector<std::string> dns_servers;
        
        std::ifstream resolv_file("/etc/resolv.conf");
        std::string line;
        
        while (std::getline(resolv_file, line)) {
            if (line.find("nameserver") != std::string::npos) {
                size_t start = line.find("nameserver") + 10;
                std::string dns = line.substr(start);
                // удаляем пробелы
                dns.erase(0, dns.find_first_not_of(" \t"));
                dns.erase(dns.find_last_not_of(" \t") + 1);
                dns_servers.push_back(dns);
            }
        }
        
        return dns_servers;
    }
    
    // получить информацию о шлюзе
    static std::string get_default_gateway() {
        std::ifstream route_file("/proc/net/route");
        std::string line;
        
        while (std::getline(route_file, line)) {
            if (line.find("00000000") != std::string::npos) {
                std::istringstream iss(line);
                std::string interface, destination, gateway;
                iss >> interface >> destination >> gateway;
                
                // конвертируем hex в ip
                if (gateway != "00000000") {
                    return hex_to_ip(gateway);
                }
            }
        }
        
        return "unknown";
    }
    
    // проверить подключение к интернету
    static bool check_internet_connection() {
        // пробуем пинговать google dns
        int result = system("ping -c 1 8.8.8.8 > /dev/null 2>&1");
        return result == 0;
    }
    
    // получить скорость соединения (примерная реализация)
    static double get_connection_speed() {
        // здесь должна быть реальная реализация измерения скорости
        // пока возвращаем заглушку
        return 100.0; // Mbps
    }
    
private:
    static std::vector<connection_info> get_connections_from_file(const std::string& file_path, const std::string& protocol) {
        std::vector<connection_info> connections;
        
        std::ifstream file(file_path);
        if (!file.is_open()) {
            return connections;
        }
        
        std::string line;
        // пропускаем заголовок
        std::getline(file, line);
        
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string local_addr, remote_addr, state;
            
            iss >> local_addr >> remote_addr >> state;
            
            connection_info conn;
            conn.protocol = protocol;
            
            // парсим локальный адрес
            if (local_addr.find(":") != std::string::npos) {
                size_t colon_pos = local_addr.find(":");
                conn.local_address = hex_to_ip(local_addr.substr(0, colon_pos));
                conn.local_port = std::to_string(std::stoi(local_addr.substr(colon_pos + 1), nullptr, 16));
            }
            
            // парсим удаленный адрес
            if (remote_addr.find(":") != std::string::npos) {
                size_t colon_pos = remote_addr.find(":");
                conn.remote_address = hex_to_ip(remote_addr.substr(0, colon_pos));
                conn.remote_port = std::to_string(std::stoi(remote_addr.substr(colon_pos + 1), nullptr, 16));
            }
            
            // конвертируем состояние
            int state_int = std::stoi(state, nullptr, 16);
            conn.state = get_tcp_state_string(state_int);
            
            connections.push_back(conn);
        }
        
        return connections;
    }
    
    static std::string hex_to_ip(const std::string& hex) {
        if (hex.length() != 8) return "unknown";
        
        // конвертируем hex в little-endian
        unsigned long addr = std::stoul(hex, nullptr, 16);
        
        std::string ip;
        for (int i = 0; i < 4; i++) {
            ip += std::to_string(addr & 0xFF);
            addr >>= 8;
            if (i < 3) ip += ".";
        }
        
        return ip;
    }
    
    static std::string get_tcp_state_string(int state) {
        switch (state) {
            case 0x01: return "ESTABLISHED";
            case 0x02: return "SYN_SENT";
            case 0x03: return "SYN_RECV";
            case 0x04: return "FIN_WAIT1";
            case 0x05: return "FIN_WAIT2";
            case 0x06: return "TIME_WAIT";
            case 0x07: return "CLOSE";
            case 0x08: return "CLOSE_WAIT";
            case 0x09: return "LAST_ACK";
            case 0x0A: return "LISTEN";
            case 0x0B: return "CLOSING";
            default: return "UNKNOWN";
        }
    }
};
