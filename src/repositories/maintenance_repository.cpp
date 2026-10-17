/**
 * @file maintenance_repository.cpp
 * @brief Реализация репозитория ТО и техосмотров.
 */

#include "maintenance_repository.h"
#include <sqlite3.h>
#include <iostream>

MaintenanceRepository::MaintenanceRepository(DatabaseManager& db) : db_(db) {}

void MaintenanceRepository::init_schema() {
    db_.execute(
        "CREATE TABLE IF NOT EXISTS maintenance ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  vehicle_id INTEGER NOT NULL,"
        "  kind TEXT,"
        "  date TEXT,"
        "  mileage INTEGER DEFAULT 0,"
        "  result TEXT,"
        "  next_date TEXT"
        ");"
    );
}

Maintenance MaintenanceRepository::row_to_maintenance(void* stmt_void) {
    sqlite3_stmt* stmt = static_cast<sqlite3_stmt*>(stmt_void);
    Maintenance m;
    m.id = sqlite3_column_int(stmt, 0);
    m.vehicle_id = sqlite3_column_int(stmt, 1);
    const unsigned char* k = sqlite3_column_text(stmt, 2);
    m.kind = k ? reinterpret_cast<const char*>(k) : "";
    const unsigned char* d = sqlite3_column_text(stmt, 3);
    m.date = d ? reinterpret_cast<const char*>(d) : "";
    m.mileage = sqlite3_column_int(stmt, 4);
    const unsigned char* r = sqlite3_column_text(stmt, 5);
    m.result = r ? reinterpret_cast<const char*>(r) : "";
    const unsigned char* n = sqlite3_column_text(stmt, 6);
    m.next_date = n ? reinterpret_cast<const char*>(n) : "";
    return m;
}

int MaintenanceRepository::create(const Maintenance& m) {
    const char* sql =
        "INSERT INTO maintenance (vehicle_id, kind, date, mileage, result, next_date) "
        "VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;

    sqlite3_bind_int(stmt, 1, m.vehicle_id);
    sqlite3_bind_text(stmt, 2, m.kind.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, m.date.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, m.mileage);
    sqlite3_bind_text(stmt, 5, m.result.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, m.next_date.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Insert error: " << sqlite3_errmsg(db_.handle()) << std::endl;
        return -1;
    }
    return static_cast<int>(sqlite3_last_insert_rowid(db_.handle()));
}

std::optional<Maintenance> MaintenanceRepository::find_by_id(int id) {
    const char* sql =
        "SELECT id, vehicle_id, kind, date, mileage, result, next_date "
        "FROM maintenance WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_int(stmt, 1, id);

    std::optional<Maintenance> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) result = row_to_maintenance(stmt);
    sqlite3_finalize(stmt);
    return result;
}

std::vector<Maintenance> MaintenanceRepository::find_all() {
    const char* sql =
        "SELECT id, vehicle_id, kind, date, mileage, result, next_date FROM maintenance;";
    sqlite3_stmt* stmt = nullptr;
    std::vector<Maintenance> result;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return result;
    while (sqlite3_step(stmt) == SQLITE_ROW) result.push_back(row_to_maintenance(stmt));
    sqlite3_finalize(stmt);
    return result;
}

std::vector<Maintenance> MaintenanceRepository::find_upcoming(int days) {
    const char* sql =
        "SELECT id, vehicle_id, kind, date, mileage, result, next_date "
        "FROM maintenance "
        "WHERE date(next_date) BETWEEN date('now') AND date('now', '+' || ? || ' days');";
    sqlite3_stmt* stmt = nullptr;
    std::vector<Maintenance> result;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return result;
    sqlite3_bind_int(stmt, 1, days);
    while (sqlite3_step(stmt) == SQLITE_ROW) result.push_back(row_to_maintenance(stmt));
    sqlite3_finalize(stmt);
    return result;
}

bool MaintenanceRepository::update(const Maintenance& m) {
    const char* sql =
        "UPDATE maintenance SET "
        "vehicle_id=?, kind=?, date=?, mileage=?, result=?, next_date=? WHERE id=?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int(stmt, 1, m.vehicle_id);
    sqlite3_bind_text(stmt, 2, m.kind.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, m.date.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, m.mileage);
    sqlite3_bind_text(stmt, 5, m.result.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, m.next_date.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 7, m.id);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool MaintenanceRepository::remove(int id) {
    const char* sql = "DELETE FROM maintenance WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}