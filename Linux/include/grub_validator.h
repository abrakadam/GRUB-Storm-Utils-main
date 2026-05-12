#ifndef GRUB_VALIDATOR_H
#define GRUB_VALIDATOR_H

#include <string>
#include <vector>

class grub_validator {
public:
    bool validate_config(const std::string& config_path = "/boot/grub/grub.cfg");
    bool validate_line_syntax(const std::string& line);
    bool validate_paths(const std::string& line);
    void validate_key_parameters(const std::string& line, int line_number);
    std::vector<std::string> get_errors() const;
    std::vector<std::string> get_warnings() const;
    bool fix_basic_errors(const std::string& config_path = "/boot/grub/grub.cfg");

private:
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

#endif // GRUB_VALIDATOR_H
