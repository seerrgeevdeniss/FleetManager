#ifndef FLEETMANAGER_DB_H
#define FLEETMANAGER_DB_H

#include <string>
#include <vector>
#include <sqlite3.h>

/**
 * @brief Класс для работы с базой данных SQLite.
 */
class DatabaseManager {
public:
    explicit DatabaseManager(const std::string& db_path);
    ~DatabaseManager();

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool execute(const std::string& sql);
    std::vector<std::vector<std::string>> query(const std::string& sql);
    sqlite3* handle() const { return db_; }

private:
    sqlite3* db_ = nullptr;
    std::string db_path_;
};

#endif