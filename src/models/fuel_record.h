/**
 * @file fuel_record.h
 * @brief Модель записи о заправке.
 */

#ifndef FLEETMANAGER_FUEL_RECORD_H
#define FLEETMANAGER_FUEL_RECORD_H

#include <string>

/**
 * @brief Модель заправки топливом (ТЗ 4.2.6).
 */
struct FuelRecord {
    int id = 0;              ///< Уникальный идентификатор
    int vehicle_id = 0;      ///< Идентификатор ТС
    int driver_id = 0;       ///< Идентификатор водителя
    std::string date;        ///< Дата заправки (YYYY-MM-DD)
    double volume = 0.0;     ///< Объём, л
    double cost = 0.0;       ///< Стоимость, руб.
    std::string station;     ///< АЗС
};

#endif // FLEETMANAGER_FUEL_RECORD_H