/**
 * @file policy_repository.cpp
 * @brief Реализация репозитория страховых полисов.
 */

#include "policy_repository.h"
#include <sqlite3.h>
#include <iostream>
#include <ctime>

PolicyRepository::PolicyRepository(DatabaseManager& db) : db_(db) {}

void PolicyRepository::init_schema() {
    db_.execute(
        "CREATE TABLE IF NOT EXISTS policies ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  vehicle_id INTEGER NOT NULL,"
        "  type TEXT NOT NULL,"
        "  number TEXT NOT NULL UNIQUE,"
        "  company TEXT,"
        "  valid_from TEXT,"
        "  valid_until TEXT,"
        "  cost REAL DEFAULT 0"
        ");"
    );
}

InsurancePolicy PolicyRepository::row_to_policy(void* stmt_void) {
    sqlite3_stmt* stmt = static_cast<sqlite3_stmt*>(stmt_void);
    InsurancePolicy p;
    p.id = sqlite3_column_int(stmt, 0);
    p.vehicle_id = sqlite3_column_int(stmt, 1);
    p.type = InsurancePolicy::string_to_type(
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
    p.number = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

    const unsigned char* comp = sqlite3_column_text(stmt, 4);
    p.company = comp ? reinterpret_cast<const char*>(comp) : "";

    const unsigned char* from = sqlite3_column_text(stmt, 5);
    p.valid_from = from ? reinterpret_cast<const char*>(from) : "";

    p.valid_until = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
    p.cost = sqlite3_column_double(stmt, 7);
    return p;
}

int PolicyRepository::create(const InsurancePolicy& p) {
    const char* sql =
        "INSERT INTO policies "
        "(vehicle_id, type, number, company, valid_from, valid_until, cost) "
        "VALUES (?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;

    std::string type_str = InsurancePolicy::type_to_string(p.type);

    sqlite3_bind_int(stmt, 1, p.vehicle_id);
    sqlite3_bind_text(stmt, 2, type_str.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, p.number.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, p.company.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, p.valid_from.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, p.valid_until.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 7, p.cost);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Insert error: " << sqlite3_errmsg(db_.handle()) << std::endl;
        return -1;
    }
    return static_cast<int>(sqlite3_last_insert_rowid(db_.handle()));
}

std::optional<InsurancePolicy> PolicyRepository::find_by_id(int id) {
    const char* sql =
        "SELECT id, vehicle_id, type, number, company, valid_from, valid_until, cost "
        "FROM policies WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_int(stmt, 1, id);

    std::optional<InsurancePolicy> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) result = row_to_policy(stmt);
    sqlite3_finalize(stmt);
    return result;
}

std::vector<InsurancePolicy> PolicyRepository::find_all() {
    const char* sql =
        "SELECT id, vehicle_id, type, number, company, valid_from, valid_until, cost "
        "FROM policies;";
    sqlite3_stmt* stmt = nullptr;
    std::vector<InsurancePolicy> result;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return result;
    while (sqlite3_step(stmt) == SQLITE_ROW) result.push_back(row_to_policy(stmt));
    sqlite3_finalize(stmt);
    return result;
}

std::vector<InsurancePolicy> PolicyRepository::find_expiring_soon(int days) {
    const char* sql =
        "SELECT id, vehicle_id, type, number, company, valid_from, valid_until, cost "
        "FROM policies "
        "WHERE date(valid_until) BETWEEN date('now') AND date('now', '+' || ? || ' days');";
    sqlite3_stmt* stmt = nullptr;
    std::vector<InsurancePolicy> result;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return result;

    sqlite3_bind_int(stmt, 1, days);
    while (sqlite3_step(stmt) == SQLITE_ROW) result.push_back(row_to_policy(stmt));
    sqlite3_finalize(stmt);
    return result;
}

bool PolicyRepository::remove(int id) {
    const char* sql = "DELETE FROM policies WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}