/**
 * @file fuel_repository.h
 * @brief Репозиторий заправок топливом.
 */

#ifndef FLEETMANAGER_FUEL_REPOSITORY_H
#define FLEETMANAGER_FUEL_REPOSITORY_H

#include <optional>
#include <vector>
#include <string>
#include "../models/fuel_record.h"
#include "../utils/db.h"

/**
 * @brief Репозиторий записей о заправках.
 */
class FuelRepository {
public:
    explicit FuelRepository(DatabaseManager& db);

    void init_schema();

    int create(const FuelRecord& f);
    std::optional<FuelRecord> find_by_id(int id);
    std::vector<FuelRecord> find_all();

    /// Общая сумма литров за период по ТС (YYYY-MM-DD).
    double total_volume(int vehicle_id,
                        const std::string& date_from,
                        const std::string& date_to);

    /// Общая стоимость за период по ТС.
    double total_cost(int vehicle_id,
                      const std::string& date_from,
                      const std::string& date_to);

    bool remove(int id);

private:
    DatabaseManager& db_;
    FuelRecord row_to_fuel_record(void* stmt_void);
};

#endif