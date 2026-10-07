#include "user.h"

std::string User::role_to_string(UserRole role) {
    switch (role) {
        case UserRole::ADMIN:      return "admin";
        case UserRole::MECHANIC:   return "mechanic";
        case UserRole::DISPATCHER: return "dispatcher";
        case UserRole::DRIVER:     return "driver";
        case UserRole::MANAGER:    return "manager";
    }
    return "driver";
}

UserRole User::string_to_role(const std::string& str) {
    if (str == "admin")      return UserRole::ADMIN;
    if (str == "mechanic")   return UserRole::MECHANIC;
    if (str == "dispatcher") return UserRole::DISPATCHER;
    if (str == "manager")    return UserRole::MANAGER;
    return UserRole::DRIVER;
}