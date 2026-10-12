/**
 * @file db.h
 * @brief Класс DatabaseManager — обёртка над SQLite.
 */
#ifndef FLEETMANAGER_PASSWORD_H
#define FLEETMANAGER_PASSWORD_H

#include <string>

/**
 * @brief Упрощённое хеширование пароля на основе std::hash.
 *
 * ВНИМАНИЕ: не подходит для продакшена. В реальном проекте нужно
 * использовать bcrypt, argon2 или scrypt.
 *
 * @param password Пароль в открытом виде.
 * @return Строка-хеш (hex).
 */
std::string hash_password(const std::string& password);

/**
 * @brief Проверяет, соответствует ли пароль хешу.
 */
bool verify_password(const std::string& password, const std::string& hash);

#endif