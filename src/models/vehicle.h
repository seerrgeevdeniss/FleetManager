/**
 * @file vehicle.h
 * @brief Модель транспортного средства.
 */

#ifndef FLEETMANAGER_VEHICLE_H
#define FLEETMANAGER_VEHICLE_H

#include <string>

/**
 * @brief Модель транспортного средства.
 *
 * Содержит основные атрибуты ТС согласно ТЗ (раздел 4.2.2).
 */
struct Vehicle {
    int id = 0;                 ///< Уникальный идентификатор
    std::string gos_number;     ///< Госномер
    std::string brand;          ///< Марка
    std::string model;          ///< Модель
    int year = 0;               ///< Год выпуска
    std::string vin;            ///< VIN-номер
    std::string type;           ///< Тип (грузовой, легковой, спецтехника)
    int mileage = 0;            ///< Пробег, км
    std::string state;          ///< Состояние (в эксплуатации, на ремонте, списано)
    int driver_id = 0;          ///< Закреплённый водитель (0 = нет)
};

#endif // FLEETMANAGER_VEHICLE_H