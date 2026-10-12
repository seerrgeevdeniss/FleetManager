/**
 * @file policy_repository.h
 * @brief Репозиторий страховых полисов.
 */

#ifndef FLEETMANAGER_POLICY_REPOSITORY_H
#define FLEETMANAGER_POLICY_REPOSITORY_H

#include <optional>
#include <vector>
#include <string>
#include "../models/insurance_policy.h"
#include "../utils/db.h"

/**
 * @brief Репозиторий полисов ОСАГО и КАСКО.
 */
class PolicyRepository {
public:
    explicit PolicyRepository(DatabaseManager& db);

    void init_schema();

    int create(const InsurancePolicy& p);
    std::optional<InsurancePolicy> find_by_id(int id);
    std::vector<InsurancePolicy> find_all();

    /**
     * @brief Полисы, истекающие в ближайшие N дней от текущей даты.
     * @param days Количество дней (например, 30, 14, 7, 3).
     */
    std::vector<InsurancePolicy> find_expiring_soon(int days);

    bool remove(int id);

private:
    DatabaseManager& db_;
    InsurancePolicy row_to_policy(void* stmt_void);
};

#endif