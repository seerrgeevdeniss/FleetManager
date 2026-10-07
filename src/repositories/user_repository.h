#ifndef FLEETMANAGER_USER_REPOSITORY_H
#define FLEETMANAGER_USER_REPOSITORY_H

#include <optional>
#include <string>
#include "../models/user.h"
#include "../utils/db.h"

/**
 * @brief Репозиторий для работы с таблицей users.
 */
class UserRepository {
public:
    explicit UserRepository(DatabaseManager& db);

    /// Создаёт таблицу users, если её нет.
    void init_schema();

    /// Добавляет нового пользователя. Возвращает id или -1 при ошибке.
    int create(const std::string& login, const std::string& password, UserRole role);

    /// Ищет пользователя по логину.
    std::optional<User> find_by_login(const std::string& login);

    /// Ищет пользователя по id.
    std::optional<User> find_by_id(int id);

    /// Обновляет пароль пользователя.
    bool update_password(int user_id, const std::string& new_password);

    /// Удаляет пользователя по логину (для тестов).
    bool remove_by_login(const std::string& login);

private:
    DatabaseManager& db_;
};

#endif