/**
 * @file main.cpp
 * @brief Точка входа приложения FleetManager.
 *
 * Инициализирует подключение к базе данных SQLite, создаёт схемы таблиц
 * и демонстрирует работу репозиториев (пользователи, транспорт, водители).
 *
 * @author Сергеев Д.М.
 * @date 2026
 */
/**
 * @file main.cpp
 * @brief Точка входа приложения FleetManager.
 *
 * День 8: демонстрация CRUD-операций для транспортных средств и водителей.
 */
/**
 * @file main.cpp
 * @brief Точка входа приложения FleetManager.
 *
 * День 9: демонстрация работы путевых листов, расхода топлива и полисов.
 */

/**
 * @file main.cpp
 * @brief HTTP-сервер приложения FleetManager.
 *
 * День 10: реализация REST API + отдача статики из web/.
 */

/**
 * @file main.cpp
 * @brief HTTP-сервер приложения FleetManager.
 */

/**
 * @file main.cpp
 * @brief HTTP-сервер приложения FleetManager.
 */

#include <iostream>
#include <filesystem>
#include <sstream>
#include <string>
#include <cstdlib>

#include "httplib.h"
#include "utils/db.h"
#include "repositories/user_repository.h"
#include "repositories/vehicle_repository.h"
#include "repositories/driver_repository.h"
#include "repositories/trip_sheet_repository.h"
#include "repositories/fuel_repository.h"
#include "repositories/policy_repository.h"
#include "services/auth_service.h"

namespace fs = std::filesystem;

static std::string json_escape(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 8);
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:   out += c;
        }
    }
    return out;
}

static std::string vehicle_to_json(const Vehicle& v) {
    std::ostringstream o;
    o << "{"
      << "\"id\":" << v.id << ","
      << "\"gos_number\":\"" << json_escape(v.gos_number) << "\","
      << "\"brand\":\"" << json_escape(v.brand) << "\","
      << "\"model\":\"" << json_escape(v.model) << "\","
      << "\"year\":" << v.year << ","
      << "\"vin\":\"" << json_escape(v.vin) << "\","
      << "\"type\":\"" << json_escape(v.type) << "\","
      << "\"mileage\":" << v.mileage << ","
      << "\"state\":\"" << json_escape(v.state) << "\","
      << "\"driver_id\":" << v.driver_id
      << "}";
    return o.str();
}

static std::string driver_to_json(const Driver& d) {
    std::ostringstream o;
    o << "{"
      << "\"id\":" << d.id << ","
      << "\"fio\":\"" << json_escape(d.fio) << "\","
      << "\"license_number\":\"" << json_escape(d.license_number) << "\","
      << "\"experience\":" << d.experience << ","
      << "\"categories\":\"" << json_escape(d.categories) << "\","
      << "\"license_valid_until\":\"" << json_escape(d.license_valid_until) << "\""
      << "}";
    return o.str();
}

static std::string trip_to_json(const TripSheet& t) {
    std::ostringstream o;
    o << "{"
      << "\"id\":" << t.id << ","
      << "\"number\":\"" << json_escape(t.number) << "\","
      << "\"date\":\"" << json_escape(t.date) << "\","
      << "\"driver_id\":" << t.driver_id << ","
      << "\"vehicle_id\":" << t.vehicle_id << ","
      << "\"route\":\"" << json_escape(t.route) << "\","
      << "\"mileage_start\":" << t.mileage_start << ","
      << "\"mileage_end\":" << t.mileage_end << ","
      << "\"fuel_consumption\":" << t.fuel_consumption << ","
      << "\"closed\":" << (t.closed ? "true" : "false")
      << "}";
    return o.str();
}

static std::string policy_to_json(const InsurancePolicy& p) {
    std::ostringstream o;
    o << "{"
      << "\"id\":" << p.id << ","
      << "\"vehicle_id\":" << p.vehicle_id << ","
      << "\"type\":\"" << InsurancePolicy::type_to_string(p.type) << "\","
      << "\"number\":\"" << json_escape(p.number) << "\","
      << "\"company\":\"" << json_escape(p.company) << "\","
      << "\"valid_from\":\"" << json_escape(p.valid_from) << "\","
      << "\"valid_until\":\"" << json_escape(p.valid_until) << "\","
      << "\"cost\":" << p.cost
      << "}";
    return o.str();
}

int main() {
    try {
        fs::path exe_path = fs::canonical("/proc/self/exe");
        fs::path project_root = exe_path.parent_path().parent_path().parent_path();
        fs::path db_path = project_root / "data" / "fleetmanager.db";
        fs::create_directories(db_path.parent_path());

        DatabaseManager db(db_path.string());

        UserRepository user_repo(db);
        VehicleRepository vehicle_repo(db);
        DriverRepository driver_repo(db);
        TripSheetRepository trip_repo(db);
        FuelRepository fuel_repo(db);
        PolicyRepository policy_repo(db);
        AuthService auth(user_repo);

        user_repo.init_schema();
        vehicle_repo.init_schema();
        driver_repo.init_schema();
        trip_repo.init_schema();
        fuel_repo.init_schema();
        policy_repo.init_schema();

        if (!user_repo.find_by_login("admin").has_value()) {
            auth.register_user("admin", "admin123", UserRole::ADMIN);
            std::cout << "Создан пользователь admin / admin123" << std::endl;
        }

        httplib::Server srv;

        fs::path web_root = project_root / "web";
        srv.set_mount_point("/", web_root.string());

        // --- Login ---
        srv.Post("/api/login", [&](const httplib::Request& req, httplib::Response& res) {
            auto login = req.get_param_value("login");
            auto password = req.get_param_value("password");
            auto result = auth.login(login, password);

            std::ostringstream o;
            if (result.success && result.user) {
                o << "{\"success\":true,\"role\":\""
                  << User::role_to_string(result.user->role)
                  << "\",\"message\":\"" << json_escape(result.message) << "\"}";
            } else {
                o << "{\"success\":false,\"message\":\"" << json_escape(result.message) << "\"}";
            }
            res.set_content(o.str(), "application/json");
        });

        // --- Register ---
        srv.Post("/api/register", [&](const httplib::Request& req, httplib::Response& res) {
            auto login = req.get_param_value("login");
            auto password = req.get_param_value("password");
            int id = auth.register_user(login, password, UserRole::DRIVER);

            std::ostringstream o;
            if (id > 0) o << "{\"success\":true,\"id\":" << id << "}";
            else o << "{\"success\":false,\"message\":\"Логин занят или пароль < 6 символов\"}";
            res.set_content(o.str(), "application/json");
        });

        // --- GET /api/vehicles ---
        srv.Get("/api/vehicles", [&](const httplib::Request&, httplib::Response& res) {
            std::ostringstream o;
            o << "[";
            bool first = true;
            for (const auto& v : vehicle_repo.find_all()) {
                if (!first) o << ",";
                first = false;
                o << vehicle_to_json(v);
            }
            o << "]";
            res.set_content(o.str(), "application/json");
        });

        // --- POST /api/vehicles (create) ---
        srv.Post("/api/vehicles", [&](const httplib::Request& req, httplib::Response& res) {
            Vehicle v;
            v.gos_number = req.get_param_value("gos_number");
            v.brand = req.get_param_value("brand");
            v.model = req.get_param_value("model");
            v.year = std::atoi(req.get_param_value("year").c_str());
            v.vin = req.get_param_value("vin");
            v.type = req.get_param_value("type");
            v.mileage = std::atoi(req.get_param_value("mileage").c_str());
            v.state = req.get_param_value("state");
            v.driver_id = std::atoi(req.get_param_value("driver_id").c_str());

            int id = vehicle_repo.create(v);
            std::ostringstream o;
            if (id > 0) o << "{\"success\":true,\"id\":" << id << "}";
            else o << "{\"success\":false,\"message\":\"Ошибка создания ТС\"}";
            res.set_content(o.str(), "application/json");
        });

        // --- POST /api/vehicles/delete ---
        srv.Post("/api/vehicles/delete", [&](const httplib::Request& req, httplib::Response& res) {
            int id = std::atoi(req.get_param_value("id").c_str());
            bool ok = vehicle_repo.remove(id);

            std::ostringstream o;
            if (ok) o << "{\"success\":true}";
            else o << "{\"success\":false,\"message\":\"Не удалось удалить ТС\"}";
            res.set_content(o.str(), "application/json");
        });

        // --- GET /api/drivers ---
        srv.Get("/api/drivers", [&](const httplib::Request&, httplib::Response& res) {
            std::ostringstream o;
            o << "[";
            bool first = true;
            for (const auto& d : driver_repo.find_all()) {
                if (!first) o << ",";
                first = false;
                o << driver_to_json(d);
            }
            o << "]";
            res.set_content(o.str(), "application/json");
        });

        // --- POST /api/drivers (create) ---
        srv.Post("/api/drivers", [&](const httplib::Request& req, httplib::Response& res) {
            Driver d;
            d.fio = req.get_param_value("fio");
            d.license_number = req.get_param_value("license_number");
            d.experience = std::atoi(req.get_param_value("experience").c_str());
            d.categories = req.get_param_value("categories");
            d.license_valid_until = req.get_param_value("license_valid_until");

            int id = driver_repo.create(d);
            std::ostringstream o;
            if (id > 0) o << "{\"success\":true,\"id\":" << id << "}";
            else o << "{\"success\":false,\"message\":\"Ошибка создания водителя\"}";
            res.set_content(o.str(), "application/json");
        });

        // --- GET /api/trip_sheets ---
        srv.Get("/api/trip_sheets", [&](const httplib::Request&, httplib::Response& res) {
            std::ostringstream o;
            o << "[";
            bool first = true;
            for (const auto& t : trip_repo.find_all()) {
                if (!first) o << ",";
                first = false;
                o << trip_to_json(t);
            }
            o << "]";
            res.set_content(o.str(), "application/json");
        });

        // --- GET /api/policies/expiring ---
        srv.Get("/api/policies/expiring", [&](const httplib::Request&, httplib::Response& res) {
            std::ostringstream o;
            o << "[";
            bool first = true;
            for (const auto& p : policy_repo.find_expiring_soon(30)) {
                if (!first) o << ",";
                first = false;
                o << policy_to_json(p);
            }
            o << "]";
            res.set_content(o.str(), "application/json");
        });

        std::cout << "Сервер запущен на http://localhost:8081" << std::endl;
        std::cout << "Корень проекта: " << project_root << std::endl;
        std::cout << "Статика из: " << web_root << std::endl;

        if (!srv.listen("0.0.0.0", 8081)) {
            std::cerr << "Не удалось запустить сервер (порт занят?)" << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}