/**
 * @file db.h
 * @brief Класс DatabaseManager — обёртка над SQLite.
 */
#ifndef FLEETMANAGER_AUTH_SERVICE_H
#define FLEETMANAGER_AUTH_SERVICE_H

#include <optional>
#include <string>
#include <unordered_map>
#include "../models/user.h"
#include "../repositories/user_repository.h"

/**
 * @brief Сервис аутентификации и разграничения доступа.
 */
class AuthService {
public:
    explicit AuthService(UserRepository& user_repo);

    /// Результат попытки входа.
    struct LoginResult {
        bool success = false;
        std::string message;
        std::optional<User> user;
    };

    /**
     * @brief Попытка входа в систему.
     * @param login Логин.
     * @param password Пароль в открытом виде.
     * @return Результат входа.
     */
    LoginResult login(const std::string& login, const std::string& password);

    /// Смена пароля (с проверкой старого).
    bool change_password(int user_id,
                         const std::string& old_password,
                         const std::string& new_password);

    /// Регистрация нового пользователя.
    int register_user(const std::string& login,
                      const std::string& password,
                      UserRole role);

    /// Сбрасывает счётчик неудачных попыток (например, после успешного входа).
    void reset_attempts(const std::string& login);

private:
    static constexpr int MAX_ATTEMPTS = 5;
    UserRepository& user_repo_;
    std::unordered_map<std::string, int> failed_attempts_;
};

#endif