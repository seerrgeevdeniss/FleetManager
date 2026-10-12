/**
 * @file trip_sheet_repository.h
 * @brief Репозиторий путевых листов.
 */

#ifndef FLEETMANAGER_TRIP_SHEET_REPOSITORY_H
#define FLEETMANAGER_TRIP_SHEET_REPOSITORY_H

#include <optional>
#include <vector>
#include <string>
#include "../models/trip_sheet.h"
#include "../utils/db.h"

/**
 * @brief Репозиторий путевых листов.
 */
class TripSheetRepository {
public:
    explicit TripSheetRepository(DatabaseManager& db);

    void init_schema();

    int create(const TripSheet& t);
    std::optional<TripSheet> find_by_id(int id);
    std::vector<TripSheet> find_all();

    /// Закрыть путевой лист: проставить пробег и расход.
    bool close(int id, int mileage_end, double fuel);

    bool remove(int id);

private:
    DatabaseManager& db_;
    TripSheet row_to_trip_sheet(void* stmt_void);
};

#endif