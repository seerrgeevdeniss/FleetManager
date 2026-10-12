/**
 * @file driver_repository.cpp
 * @brief Реализация репозитория водителей.
 */

#include "driver_repository.h"
#include <sqlite3.h>
#include <iostream>

DriverRepository::DriverRepository(DatabaseManager& db) : db_(db) {}

void DriverRepository::init_schema() {
    db_.execute(
        "CREATE TABLE IF NOT EXISTS drivers ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  fio TEXT NOT NULL,"
        "  license_number TEXT NOT NULL UNIQUE,"
        "  experience INTEGER DEFAULT 0,"
        "  categories TEXT,"
        "  license_valid_until TEXT"
        ");"
    );
}

Driver DriverRepository::row_to_driver(void* stmt_void) {
    sqlite3_stmt* stmt = static_cast<sqlite3_stmt*>(stmt_void);
    Driver d;
    d.id = sqlite3_column_int(stmt, 0);
    d.fio = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    d.license_number = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    d.experience = sqlite3_column_int(stmt, 3);

    const unsigned char* cat = sqlite3_column_text(stmt, 4);
    d.categories = cat ? reinterpret_cast<const char*>(cat) : "";

    const unsigned char* until = sqlite3_column_text(stmt, 5);
    d.license_valid_until = until ? reinterpret_cast<const char*>(until) : "";

    return d;
}

int DriverRepository::create(const Driver& d) {
    const char* sql =
        "INSERT INTO drivers "
        "(fio, license_number, experience, categories, license_valid_until) "
        "VALUES (?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare error: " << sqlite3_errmsg(db_.handle()) << std::endl;
        return -1;
    }

    sqlite3_bind_text(stmt, 1, d.fio.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, d.license_number.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, d.experience);
    sqlite3_bind_text(stmt, 4, d.categories.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, d.license_valid_until.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "Insert error: " << sqlite3_errmsg(db_.handle()) << std::endl;
        return -1;
    }
    return static_cast<int>(sqlite3_last_insert_rowid(db_.handle()));
}

std::optional<Driver> DriverRepository::find_by_id(int id) {
    const char* sql =
        "SELECT id, fio, license_number, experience, categories, license_valid_until "
        "FROM drivers WHERE id = ?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }
    sqlite3_bind_int(stmt, 1, id);

    std::optional<Driver> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        result = row_to_driver(stmt);
    }
    sqlite3_finalize(stmt);
    return result;
}

std::vector<Driver> DriverRepository::find_all() {
    const char* sql =
        "SELECT id, fio, license_number, experience, categories, license_valid_until "
        "FROM drivers;";

    sqlite3_stmt* stmt = nullptr;
    std::vector<Driver> result;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        result.push_back(row_to_driver(stmt));
    }
    sqlite3_finalize(stmt);
    return result;
}

bool DriverRepository::update(const Driver& d) {
    const char* sql =
        "UPDATE drivers SET "
        "fio=?, license_number=?, experience=?, categories=?, license_valid_until=? "
        "WHERE id=?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, d.fio.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, d.license_number.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, d.experience);
    sqlite3_bind_text(stmt, 4, d.categories.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, d.license_valid_until.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, d.id);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool DriverRepository::remove(int id) {
    const char* sql = "DELETE FROM drivers WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    sqlite3_bind_int(stmt, 1, id);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}