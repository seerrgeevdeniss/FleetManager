/**
 * @file vehicle_repository.cpp
 * @brief Реализация репозитория транспортных средств.
 */

#include "vehicle_repository.h"
#include <sqlite3.h>
#include <iostream>

VehicleRepository::VehicleRepository(DatabaseManager& db) : db_(db) {}

void VehicleRepository::init_schema() {
    db_.execute(
        "CREATE TABLE IF NOT EXISTS vehicles ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  gos_number TEXT NOT NULL UNIQUE,"
        "  brand TEXT NOT NULL,"
        "  model TEXT NOT NULL,"
        "  year INTEGER,"
        "  vin TEXT,"
        "  type TEXT,"
        "  mileage INTEGER DEFAULT 0,"
        "  state TEXT DEFAULT 'в эксплуатации',"
        "  driver_id INTEGER DEFAULT 0"
        ");"
    );
}

Vehicle VehicleRepository::row_to_vehicle(void* stmt_void) {
    sqlite3_stmt* stmt = static_cast<sqlite3_stmt*>(stmt_void);
    Vehicle v;
    v.id = sqlite3_column_int(stmt, 0);
    v.gos_number = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    v.brand = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    v.model = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    v.year = sqlite3_column_int(stmt, 4);

    const unsigned char* vin = sqlite3_column_text(stmt, 5);
    v.vin = vin ? reinterpret_cast<const char*>(vin) : "";

    const unsigned char* type = sqlite3_column_text(stmt, 6);
    v.type = type ? reinterpret_cast<const char*>(type) : "";

    v.mileage = sqlite3_column_int(stmt, 7);

    const unsigned char* state = sqlite3_column_text(stmt, 8);
    v.state = state ? reinterpret_cast<const char*>(state) : "";

    v.driver_id = sqlite3_column_int(stmt, 9);
    return v;
}

int VehicleRepository::create(const Vehicle& v) {
    const char* sql =
        "INSERT INTO vehicles "
        "(gos_number, brand, model, year, vin, type, mileage, state, driver_id) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare error: " << sqlite3_errmsg(db_.handle()) << std::endl;
        return -1;
    }

    sqlite3_bind_text(stmt, 1, v.gos_number.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, v.brand.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, v.model.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, v.year);
    sqlite3_bind_text(stmt, 5, v.vin.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, v.type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 7, v.mileage);
    sqlite3_bind_text(stmt, 8, v.state.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 9, v.driver_id);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "Insert error: " << sqlite3_errmsg(db_.handle()) << std::endl;
        return -1;
    }
    return static_cast<int>(sqlite3_last_insert_rowid(db_.handle()));
}

std::optional<Vehicle> VehicleRepository::find_by_id(int id) {
    const char* sql =
        "SELECT id, gos_number, brand, model, year, vin, type, mileage, state, driver_id "
        "FROM vehicles WHERE id = ?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }
    sqlite3_bind_int(stmt, 1, id);

    std::optional<Vehicle> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        result = row_to_vehicle(stmt);
    }
    sqlite3_finalize(stmt);
    return result;
}

std::vector<Vehicle> VehicleRepository::find_all() {
    const char* sql =
        "SELECT id, gos_number, brand, model, year, vin, type, mileage, state, driver_id "
        "FROM vehicles;";

    sqlite3_stmt* stmt = nullptr;
    std::vector<Vehicle> result;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        result.push_back(row_to_vehicle(stmt));
    }
    sqlite3_finalize(stmt);
    return result;
}

std::vector<Vehicle> VehicleRepository::search(const std::string& type,
                                               const std::string& state) {
    std::string sql =
        "SELECT id, gos_number, brand, model, year, vin, type, mileage, state, driver_id "
        "FROM vehicles WHERE 1=1";
    if (!type.empty())  sql += " AND type = ?";
    if (!state.empty()) sql += " AND state = ?";

    sqlite3_stmt* stmt = nullptr;
    std::vector<Vehicle> result;
    if (sqlite3_prepare_v2(db_.handle(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    int idx = 1;
    if (!type.empty())  sqlite3_bind_text(stmt, idx++, type.c_str(), -1, SQLITE_TRANSIENT);
    if (!state.empty()) sqlite3_bind_text(stmt, idx++, state.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        result.push_back(row_to_vehicle(stmt));
    }
    sqlite3_finalize(stmt);
    return result;
}

bool VehicleRepository::update(const Vehicle& v) {
    const char* sql =
        "UPDATE vehicles SET "
        "gos_number=?, brand=?, model=?, year=?, vin=?, type=?, mileage=?, state=?, driver_id=? "
        "WHERE id=?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, v.gos_number.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, v.brand.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, v.model.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, v.year);
    sqlite3_bind_text(stmt, 5, v.vin.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, v.type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 7, v.mileage);
    sqlite3_bind_text(stmt, 8, v.state.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 9, v.driver_id);
    sqlite3_bind_int(stmt, 10, v.id);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool VehicleRepository::remove(int id) {
    const char* sql = "DELETE FROM vehicles WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    sqlite3_bind_int(stmt, 1, id);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}