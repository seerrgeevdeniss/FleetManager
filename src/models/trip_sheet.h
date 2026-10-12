/**
 * @file trip_sheet.h
 * @brief Модель путевого листа.
 */

#ifndef FLEETMANAGER_TRIP_SHEET_H
#define FLEETMANAGER_TRIP_SHEET_H

#include <string>

/**
 * @brief Модель путевого листа (ТЗ 4.2.5).
 */
struct TripSheet {
    int id = 0;                     ///< Уникальный идентификатор
    std::string number;             ///< Номер путевого листа
    std::string date;               ///< Дата (YYYY-MM-DD)
    int driver_id = 0;              ///< Идентификатор водителя
    int vehicle_id = 0;             ///< Идентификатор ТС
    std::string route;              ///< Маршрут
    int mileage_start = 0;          ///< Одометр на начало, км
    int mileage_end = 0;            ///< Одометр на конец, км
    double fuel_consumption = 0.0;  ///< Расход топлива, л
    bool closed = false;            ///< Закрыт ли путевой лист
};

#endif // FLEETMANAGER_TRIP_SHEET_H