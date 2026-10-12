/**
 * @file driver_repository.h
 * @brief Репозиторий для работы с таблицей водителей.
 */

#ifndef FLEETMANAGER_DRIVER_REPOSITORY_H
#define FLEETMANAGER_DRIVER_REPOSITORY_H

#include <optional>
#include <vector>
#include <string>
#include "../models/driver.h"
#include "../utils/db.h"

/**
 * @brief Репозиторий водителей.
 *
 * Обеспечивает CRUD-операции над таблицей drivers.
 * Все запросы параметризованы (защита от SQL-инъекций).
 */
class DriverRepository {
public:
    /**
     * @brief Конструктор.
     * @param db Ссылка на менеджер базы данных.
     */
    explicit DriverRepository(DatabaseManager& db);

    /// Создаёт таблицу drivers, если её нет.
    void init_schema();

    /**
     * @brief Создать нового водителя.
     * @param d Объект водителя без id.
     * @return id созданной записи или -1 при ошибке.
     */
    int create(const Driver& d);

    /**
     * @brief Найти водителя по id.
     * @param id Идентификатор.
     * @return Объект или std::nullopt, если не найдено.
     */
    std::optional<Driver> find_by_id(int id);

    /// Получить всех водителей.
    std::vector<Driver> find_all();

    /**
     * @brief Обновить данные водителя.
     * @param d Объект с заполненным id.
     * @return true при успехе.
     */
    bool update(const Driver& d);

    /**
     * @brief Удалить водителя по id.
     * @param id Идентификатор.
     * @return true при успехе.
     */
    bool remove(int id);

private:
    DatabaseManager& db_;   ///< Ссылка на БД

    /**
     * @brief Преобразует текущую строку sqlite3_stmt в объект Driver.
     */
    Driver row_to_driver(void* stmt_void);
};

#endif // FLEETMANAGER_DRIVER_REPOSITORY_H