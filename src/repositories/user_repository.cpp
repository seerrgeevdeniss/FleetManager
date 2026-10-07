#include "user_repository.h"
#include "../utils/password.h"
#include <sqlite3.h>
#include <iostream>

UserRepository::UserRepository(DatabaseManager& db) : db_(db) {}

void UserRepository::init_schema() {
    db_.execute(
        "CREATE TABLE IF NOT EXISTS users ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  login TEXT NOT NULL UNIQUE,"
        "  password_hash TEXT NOT NULL,"
        "  role TEXT NOT NULL DEFAULT 'driver'"
        ");"
    );
}

int UserRepository::create(const std::string& login,
                           const std::string& password,
                           UserRole role) {
    const char* sql = "INSERT INTO users (login, password_hash, role) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare error: " << sqlite3_errmsg(db_.handle()) << std::endl;
        return -1;
    }

    std::string hash = hash_password(password);
    std::string role_str = User::role_to_string(role);

    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, hash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, role_str.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "Insert error: " << sqlite3_errmsg(db_.handle()) << std::endl;
        return -1;
    }

    return static_cast<int>(sqlite3_last_insert_rowid(db_.handle()));
}

std::optional<User> UserRepository::find_by_login(const std::string& login) {
    const char* sql = "SELECT id, login, password_hash, role FROM users WHERE login = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }

    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<User> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        User u;
        u.id = sqlite3_column_int(stmt, 0);
        u.login = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        u.password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        u.role = User::string_to_role(
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
        result = u;
    }

    sqlite3_finalize(stmt);
    return result;
}

std::optional<User> UserRepository::find_by_id(int id) {
    const char* sql = "SELECT id, login, password_hash, role FROM users WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }
    sqlite3_bind_int(stmt, 1, id);

    std::optional<User> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        User u;
        u.id = sqlite3_column_int(stmt, 0);
        u.login = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        u.password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        u.role = User::string_to_role(
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
        result = u;
    }
    sqlite3_finalize(stmt);
    return result;
}

bool UserRepository::update_password(int user_id, const std::string& new_password) {
    const char* sql = "UPDATE users SET password_hash = ? WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    std::string hash = hash_password(new_password);
    sqlite3_bind_text(stmt, 1, hash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, user_id);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool UserRepository::remove_by_login(const std::string& login) {
    const char* sql = "DELETE FROM users WHERE login = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}