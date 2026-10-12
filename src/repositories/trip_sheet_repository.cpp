/**
 * @file trip_sheet_repository.cpp
 * @brief Реализация репозитория путевых листов.
 */

#include "trip_sheet_repository.h"
#include <sqlite3.h>
#include <iostream>

TripSheetRepository::TripSheetRepository(DatabaseManager& db) : db_(db) {}

void TripSheetRepository::init_schema() {
    db_.execute(
        "CREATE TABLE IF NOT EXISTS trip_sheets ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  number TEXT NOT NULL UNIQUE,"
        "  date TEXT NOT NULL,"
        "  driver_id INTEGER,"
        "  vehicle_id INTEGER,"
        "  route TEXT,"
        "  mileage_start INTEGER DEFAULT 0,"
        "  mileage_end INTEGER DEFAULT 0,"
        "  fuel_consumption REAL DEFAULT 0,"
        "  closed INTEGER DEFAULT 0"
        ");"
    );
}

TripSheet TripSheetRepository::row_to_trip_sheet(void* stmt_void) {
    sqlite3_stmt* stmt = static_cast<sqlite3_stmt*>(stmt_void);
    TripSheet t;
    t.id = sqlite3_column_int(stmt, 0);
    t.number = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    t.date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    t.driver_id = sqlite3_column_int(stmt, 3);
    t.vehicle_id = sqlite3_column_int(stmt, 4);

    const unsigned char* route = sqlite3_column_text(stmt, 5);
    t.route = route ? reinterpret_cast<const char*>(route) : "";

    t.mileage_start = sqlite3_column_int(stmt, 6);
    t.mileage_end = sqlite3_column_int(stmt, 7);
    t.fuel_consumption = sqlite3_column_double(stmt, 8);
    t.closed = sqlite3_column_int(stmt, 9) != 0;
    return t;
}

int TripSheetRepository::create(const TripSheet& t) {
    const char* sql =
        "INSERT INTO trip_sheets "
        "(number, date, driver_id, vehicle_id, route, mileage_start) "
        "VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, t.number.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, t.date.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, t.driver_id);
    sqlite3_bind_int(stmt, 4, t.vehicle_id);
    sqlite3_bind_text(stmt, 5, t.route.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, t.mileage_start);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Insert error: " << sqlite3_errmsg(db_.handle()) << std::endl;
        return -1;
    }
    return static_cast<int>(sqlite3_last_insert_rowid(db_.handle()));
}

std::optional<TripSheet> TripSheetRepository::find_by_id(int id) {
    const char* sql =
        "SELECT id, number, date, driver_id, vehicle_id, route, "
        "mileage_start, mileage_end, fuel_consumption, closed "
        "FROM trip_sheets WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_int(stmt, 1, id);

    std::optional<TripSheet> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) result = row_to_trip_sheet(stmt);
    sqlite3_finalize(stmt);
    return result;
}

std::vector<TripSheet> TripSheetRepository::find_all() {
    const char* sql =
        "SELECT id, number, date, driver_id, vehicle_id, route, "
        "mileage_start, mileage_end, fuel_consumption, closed "
        "FROM trip_sheets;";
    sqlite3_stmt* stmt = nullptr;
    std::vector<TripSheet> result;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return result;
    while (sqlite3_step(stmt) == SQLITE_ROW) result.push_back(row_to_trip_sheet(stmt));
    sqlite3_finalize(stmt);
    return result;
}

bool TripSheetRepository::close(int id, int mileage_end, double fuel) {
    const char* sql =
        "UPDATE trip_sheets SET mileage_end = ?, fuel_consumption = ?, closed = 1 "
        "WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int(stmt, 1, mileage_end);
    sqlite3_bind_double(stmt, 2, fuel);
    sqlite3_bind_int(stmt, 3, id);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool TripSheetRepository::remove(int id) {
    const char* sql = "DELETE FROM trip_sheets WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}