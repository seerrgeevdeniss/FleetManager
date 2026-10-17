/**
 * @file maintenance_repository.h
 * @brief Репозиторий ТО и техосмотров.
 */

#ifndef FLEETMANAGER_MAINTENANCE_REPOSITORY_H
#define FLEETMANAGER_MAINTENANCE_REPOSITORY_H

#include <optional>
#include <vector>
#include <string>
#include "../models/maintenance.h"
#include "../utils/db.h"

/**
 * @brief Репозиторий ТО и техосмотров.
 */
class MaintenanceRepository {
public:
    explicit MaintenanceRepository(DatabaseManager& db);

    void init_schema();
    int create(const Maintenance& m);
    std::optional<Maintenance> find_by_id(int id);
    std::vector<Maintenance> find_all();

    /// Ближайшие ТО в течение N дней (по полю next_date).
    std::vector<Maintenance> find_upcoming(int days);

    bool update(const Maintenance& m);
    bool remove(int id);

private:
    DatabaseManager& db_;
    Maintenance row_to_maintenance(void* stmt_void);
};

#endif