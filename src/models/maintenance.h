/**
 * @file maintenance.h
 * @brief Модель записи о ТО/техосмотре.
 */

#ifndef FLEETMANAGER_MAINTENANCE_H
#define FLEETMANAGER_MAINTENANCE_H

#include <string>

/**
 * @brief Запись о пройденном ТО или техосмотре (ТЗ 4.2.4).
 */
struct Maintenance {
    int id = 0;
    int vehicle_id = 0;      ///< Идентификатор ТС
    std::string kind;        ///< Вид (ТО / техосмотр)
    std::string date;        ///< Дата проведения (YYYY-MM-DD)
    int mileage = 0;         ///< Пробег на момент ТО
    std::string result;      ///< Результат
    std::string next_date;   ///< Дата следующего ТО (YYYY-MM-DD)
};

#endif