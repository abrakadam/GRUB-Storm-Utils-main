#ifndef NETWORK_INFO_H
#define NETWORK_INFO_H

#include <string>
#include <vector>

class network_info {
public:
    struct interface_info {
        std::string name;
        std::string ip_address;
        std::string netmask;
        std::string mac_address;
        bool is_up;
        std::string type;
    };
    
    struct connection_info {
        std::string protocol;
        std::string local_address;
        std::string local_port;
        std::string remote_address;
        std::string remote_port;
        std::string state;
    };
    
    static std::vector<interface_info> get_interface_info();
    static std::string get_mac_address(const std::string& interface_name);
    static std::vector<connection_info> get_connections();
    static std::vector<std::string> get_dns_servers();
    static std::string get_default_gateway();
    static bool check_internet_connection();
    static double get_connection_speed();
};

#endif // NETWORK_INFO_H
