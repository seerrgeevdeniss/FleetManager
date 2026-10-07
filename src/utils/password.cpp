#include "password.h"
#include <functional>
#include <sstream>
#include <iomanip>

std::string hash_password(const std::string& password) {
    // Добавляем "соль", чтобы одинаковые пароли давали разные хеши
    std::string salted = "fleetmanager_salt_v1:" + password;
    std::size_t h = std::hash<std::string>{}(salted);

    std::ostringstream oss;
    oss << std::hex << h;
    return oss.str();
}

bool verify_password(const std::string& password, const std::string& hash) {
    return hash_password(password) == hash;
}