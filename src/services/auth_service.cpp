#include "auth_service.h"
#include "../utils/password.h"

AuthService::AuthService(UserRepository& user_repo) : user_repo_(user_repo) {}

AuthService::LoginResult AuthService::login(const std::string& login,
                                            const std::string& password) {
    LoginResult result;

    // Проверка блокировки
    auto it = failed_attempts_.find(login);
    if (it != failed_attempts_.end() && it->second >= MAX_ATTEMPTS) {
        result.message = "Учётная запись заблокирована. Обратитесь к администратору.";
        return result;
    }

    auto user_opt = user_repo_.find_by_login(login);
    if (!user_opt.has_value()) {
        failed_attempts_[login]++;
        result.message = "Неверный логин или пароль.";
        return result;
    }

    if (!verify_password(password, user_opt->password_hash)) {
        failed_attempts_[login]++;
        int left = MAX_ATTEMPTS - failed_attempts_[login];
        result.message = "Неверный логин или пароль. Осталось попыток: " +
                         std::to_string(left > 0 ? left : 0);
        return result;
    }

    // Успех
    failed_attempts_.erase(login);
    result.success = true;
    result.user = user_opt;
    result.message = "Вход выполнен успешно.";
    return result;
}

bool AuthService::change_password(int user_id,
                                  const std::string& old_password,
                                  const std::string& new_password) {
    auto user_opt = user_repo_.find_by_id(user_id);
    if (!user_opt.has_value()) return false;

    if (!verify_password(old_password, user_opt->password_hash)) return false;

    if (new_password.size() < 6) return false;  // минимальная длина

    return user_repo_.update_password(user_id, new_password);
}

int AuthService::register_user(const std::string& login,
                               const std::string& password,
                               UserRole role) {
    if (login.empty() || password.size() < 6) return -1;
    if (user_repo_.find_by_login(login).has_value()) return -1;  // уже существует
    return user_repo_.create(login, password, role);
}

void AuthService::reset_attempts(const std::string& login) {
    failed_attempts_.erase(login);
}