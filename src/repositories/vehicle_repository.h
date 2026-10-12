/**
 * @file vehicle_repository.h
 * @brief Репозиторий для работы с таблицей транспортных средств.
 */

#ifndef FLEETMANAGER_VEHICLE_REPOSITORY_H
#define FLEETMANAGER_VEHICLE_REPOSITORY_H

#include <optional>
#include <vector>
#include <string>
#include "../models/vehicle.h"
#include "../utils/db.h"

/**
 * @brief Репозиторий транспортных средств.
 *
 * Обеспечивает CRUD-операции над таблицей vehicles.
 * Все запросы параметризованы (защита от SQL-инъекций).
 */
class VehicleRepository {
public:
    /**
     * @brief Конструктор.
     * @param db Ссылка на менеджер базы данных.
     */
    explicit VehicleRepository(DatabaseManager& db);

    /// Создаёт таблицу vehicles, если её нет.
    void init_schema();

    /**
     * @brief Создать новое ТС.
     * @param v Объект ТС без id.
     * @return id созданной записи или -1 при ошибке.
     */
    int create(const Vehicle& v);

    /**
     * @brief Найти ТС по id.
     * @param id Идентификатор.
     * @return Объект или std::nullopt, если не найдено.
     */
    std::optional<Vehicle> find_by_id(int id);

    /// Получить все ТС.
    std::vector<Vehicle> find_all();

    /**
     * @brief Поиск ТС с фильтрами.
     * @param type Тип ТС (пустая строка — без фильтра).
     * @param state Состояние (пустая строка — без фильтра).
     */
    std::vector<Vehicle> search(const std::string& type,
                                const std::string& state);

    /**
     * @brief Обновить ТС.
     * @param v Объект с заполненным id.
     * @return true при успехе.
     */
    bool update(const Vehicle& v);

    /**
     * @brief Удалить ТС по id.
     * @param id Идентификатор.
     * @return true при успехе.
     */
    bool remove(int id);

private:
    DatabaseManager& db_;   ///< Ссылка на БД

    /**
     * @brief Преобразует текущую строку sqlite3_stmt в объект Vehicle.
     */
    Vehicle row_to_vehicle(void* stmt_void);
};

#endif // FLEETMANAGER_VEHICLE_REPOSITORY_H