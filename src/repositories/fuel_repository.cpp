/**
 * @file fuel_repository.cpp
 * @brief Реализация репозитория заправок.
 */

#include "fuel_repository.h"
#include <sqlite3.h>
#include <iostream>

FuelRepository::FuelRepository(DatabaseManager& db) : db_(db) {}

void FuelRepository::init_schema() {
    db_.execute(
        "CREATE TABLE IF NOT EXISTS fuel_records ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  vehicle_id INTEGER NOT NULL,"
        "  driver_id INTEGER,"
        "  date TEXT NOT NULL,"
        "  volume REAL DEFAULT 0,"
        "  cost REAL DEFAULT 0,"
        "  station TEXT"
        ");"
    );
}

FuelRecord FuelRepository::row_to_fuel_record(void* stmt_void) {
    sqlite3_stmt* stmt = static_cast<sqlite3_stmt*>(stmt_void);
    FuelRecord f;
    f.id = sqlite3_column_int(stmt, 0);
    f.vehicle_id = sqlite3_column_int(stmt, 1);
    f.driver_id = sqlite3_column_int(stmt, 2);
    f.date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    f.volume = sqlite3_column_double(stmt, 4);
    f.cost = sqlite3_column_double(stmt, 5);

    const unsigned char* st = sqlite3_column_text(stmt, 6);
    f.station = st ? reinterpret_cast<const char*>(st) : "";
    return f;
}

int FuelRepository::create(const FuelRecord& f) {
    const char* sql =
        "INSERT INTO fuel_records (vehicle_id, driver_id, date, volume, cost, station) "
        "VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;

    sqlite3_bind_int(stmt, 1, f.vehicle_id);
    sqlite3_bind_int(stmt, 2, f.driver_id);
    sqlite3_bind_text(stmt, 3, f.date.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 4, f.volume);
    sqlite3_bind_double(stmt, 5, f.cost);
    sqlite3_bind_text(stmt, 6, f.station.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Insert error: " << sqlite3_errmsg(db_.handle()) << std::endl;
        return -1;
    }
    return static_cast<int>(sqlite3_last_insert_rowid(db_.handle()));
}

std::optional<FuelRecord> FuelRepository::find_by_id(int id) {
    const char* sql =
        "SELECT id, vehicle_id, driver_id, date, volume, cost, station "
        "FROM fuel_records WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_int(stmt, 1, id);

    std::optional<FuelRecord> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) result = row_to_fuel_record(stmt);
    sqlite3_finalize(stmt);
    return result;
}

std::vector<FuelRecord> FuelRepository::find_all() {
    const char* sql =
        "SELECT id, vehicle_id, driver_id, date, volume, cost, station FROM fuel_records;";
    sqlite3_stmt* stmt = nullptr;
    std::vector<FuelRecord> result;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return result;
    while (sqlite3_step(stmt) == SQLITE_ROW) result.push_back(row_to_fuel_record(stmt));
    sqlite3_finalize(stmt);
    return result;
}

double FuelRepository::total_volume(int vehicle_id,
                                    const std::string& date_from,
                                    const std::string& date_to) {
    const char* sql =
        "SELECT COALESCE(SUM(volume), 0) FROM fuel_records "
        "WHERE vehicle_id = ? AND date BETWEEN ? AND ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return 0.0;

    sqlite3_bind_int(stmt, 1, vehicle_id);
    sqlite3_bind_text(stmt, 2, date_from.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, date_to.c_str(), -1, SQLITE_TRANSIENT);

    double result = 0.0;
    if (sqlite3_step(stmt) == SQLITE_ROW) result = sqlite3_column_double(stmt, 0);
    sqlite3_finalize(stmt);
    return result;
}

double FuelRepository::total_cost(int vehicle_id,
                                  const std::string& date_from,
                                  const std::string& date_to) {
    const char* sql =
        "SELECT COALESCE(SUM(cost), 0) FROM fuel_records "
        "WHERE vehicle_id = ? AND date BETWEEN ? AND ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return 0.0;

    sqlite3_bind_int(stmt, 1, vehicle_id);
    sqlite3_bind_text(stmt, 2, date_from.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, date_to.c_str(), -1, SQLITE_TRANSIENT);

    double result = 0.0;
    if (sqlite3_step(stmt) == SQLITE_ROW) result = sqlite3_column_double(stmt, 0);
    sqlite3_finalize(stmt);
    return result;
}

bool FuelRepository::remove(int id) {
    const char* sql = "DELETE FROM fuel_records WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}