#ifndef FLEETMANAGER_USER_H
#define FLEETMANAGER_USER_H

#include <string>

/**
 * @brief Роли пользователей системы.
 */
enum class UserRole {
    ADMIN,        ///< Администратор
    MECHANIC,     ///< Механик
    DISPATCHER,   ///< Диспетчер
    DRIVER,       ///< Водитель
    MANAGER       ///< Руководитель
};

/**
 * @brief Модель пользователя системы.
 */
struct User {
    int id = 0;                 ///< Уникальный идентификатор
    std::string login;          ///< Логин
    std::string password_hash;  ///< Хеш пароля
    UserRole role = UserRole::DRIVER;  ///< Роль

    /// Преобразует роль в строку для хранения в БД.
    static std::string role_to_string(UserRole role);

    /// Преобразует строку из БД в роль.
    static UserRole string_to_role(const std::string& str);
};

#endif