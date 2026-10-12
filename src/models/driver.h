/**
 * @file driver.h
 * @brief Модель водителя.
 */

#ifndef FLEETMANAGER_DRIVER_H
#define FLEETMANAGER_DRIVER_H

#include <string>

/**
 * @brief Модель водителя.
 *
 * Содержит атрибуты водителя согласно ТЗ (раздел 4.2.3).
 */
struct Driver {
    int id = 0;                       ///< Уникальный идентификатор
    std::string fio;                  ///< ФИО
    std::string license_number;       ///< Номер водительского удостоверения
    int experience = 0;               ///< Стаж, лет
    std::string categories;           ///< Категории (B, C, D, ...)
    std::string license_valid_until;  ///< Срок действия прав (YYYY-MM-DD)
};

#endif // FLEETMANAGER_DRIVER_H