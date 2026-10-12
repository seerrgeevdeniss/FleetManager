/**
 * @file insurance_policy.h
 * @brief Модель страхового полиса (ОСАГО / КАСКО).
 */

#ifndef FLEETMANAGER_INSURANCE_POLICY_H
#define FLEETMANAGER_INSURANCE_POLICY_H

#include <string>

/**
 * @brief Тип полиса.
 */
enum class PolicyType {
    OSAGO,   ///< ОСАГО
    KASKO    ///< КАСКО
};

/**
 * @brief Модель страхового полиса (ТЗ 4.2.7).
 */
struct InsurancePolicy {
    int id = 0;                    ///< Уникальный идентификатор
    int vehicle_id = 0;            ///< Идентификатор ТС
    PolicyType type = PolicyType::OSAGO;  ///< Тип полиса
    std::string number;            ///< Номер полиса
    std::string company;           ///< Страховая компания
    std::string valid_from;        ///< Дата начала действия (YYYY-MM-DD)
    std::string valid_until;       ///< Дата окончания действия (YYYY-MM-DD)
    double cost = 0.0;             ///< Стоимость, руб.

    /// Преобразует тип полиса в строку для БД.
    static std::string type_to_string(PolicyType t);

    /// Преобразует строку из БД в тип полиса.
    static PolicyType string_to_type(const std::string& s);
};

#endif // FLEETMANAGER_INSURANCE_POLICY_H