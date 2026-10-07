#include "db.h"
#include <stdexcept>
#include <iostream>

DatabaseManager::DatabaseManager(const std::string& db_path) : db_path_(db_path) {
    int rc = sqlite3_open(db_path.c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::string err = sqlite3_errmsg(db_);
        sqlite3_close(db_);
        db_ = nullptr;
        throw std::runtime_error("Не удалось открыть БД: " + err);
    }
}

DatabaseManager::~DatabaseManager() {
    if (db_) sqlite3_close(db_);
}

bool DatabaseManager::execute(const std::string& sql) {
    char* err_msg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << (err_msg ? err_msg : "unknown") << std::endl;
        sqlite3_free(err_msg);
        return false;
    }
    return true;
}

std::vector<std::vector<std::string>> DatabaseManager::query(const std::string& sql) {
    std::vector<std::vector<std::string>> rows;
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare error: " << sqlite3_errmsg(db_) << std::endl;
        return rows;
    }

    int cols = sqlite3_column_count(stmt);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::vector<std::string> row;
        for (int i = 0; i < cols; ++i) {
            const unsigned char* text = sqlite3_column_text(stmt, i);
            row.push_back(text ? reinterpret_cast<const char*>(text) : "");
        }
        rows.push_back(std::move(row));
    }

    sqlite3_finalize(stmt);
    return rows;
}