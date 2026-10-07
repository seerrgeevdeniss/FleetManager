#include <iostream>
#include <filesystem>
#include "utils/db.h"
#include "repositories/user_repository.h"
#include "services/auth_service.h"

namespace fs = std::filesystem;

int main() {
    try {
        // Путь к БД относительно корня проекта
        fs::path exe_path = fs::canonical("/proc/self/exe");
        fs::path project_root = exe_path.parent_path().parent_path().parent_path();
        fs::path db_path = project_root / "data" / "fleetmanager.db";
        fs::create_directories(db_path.parent_path());

        DatabaseManager db(db_path.string());
        UserRepository user_repo(db);
        user_repo.init_schema();

        AuthService auth(user_repo);

        // Создаём админа, если его нет
        if (!user_repo.find_by_login("admin").has_value()) {
            auth.register_user("admin", "admin123", UserRole::ADMIN);
            std::cout << "Создан пользователь admin / admin123" << std::endl;
        }

        // Проверяем вход
        auto res1 = auth.login("admin", "admin123");
        std::cout << "Вход верный: " << res1.message
                  << " | роль: " << (res1.user ? User::role_to_string(res1.user->role) : "-")
                  << std::endl;

        auto res2 = auth.login("admin", "wrong");
        std::cout << "Вход неверный: " << res2.message << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}