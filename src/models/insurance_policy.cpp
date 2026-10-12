/**
 * @file insurance_policy.cpp
 * @brief Реализация методов модели InsurancePolicy.
 */

#include "insurance_policy.h"

std::string InsurancePolicy::type_to_string(PolicyType t) {
    return (t == PolicyType::OSAGO) ? "osago" : "kasko";
}

PolicyType InsurancePolicy::string_to_type(const std::string& s) {
    return (s == "kasko") ? PolicyType::KASKO : PolicyType::OSAGO;
}