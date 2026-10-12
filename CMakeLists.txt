cmake_minimum_required(VERSION 3.16)
project(FleetManager CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third_party
)

# Системный SQLite
find_package(SQLite3 REQUIRED)
include_directories(${SQLite3_INCLUDE_DIRS})

# Потоки для httplib
find_package(Threads REQUIRED)

set(SOURCES
    src/server/main.cpp
    src/utils/db.cpp
    src/utils/password.cpp
    src/models/user.cpp
    src/models/insurance_policy.cpp
    src/repositories/user_repository.cpp
    src/services/auth_service.cpp
    src/repositories/vehicle_repository.cpp
    src/repositories/driver_repository.cpp
    src/repositories/trip_sheet_repository.cpp
    src/repositories/fuel_repository.cpp
    src/repositories/policy_repository.cpp
)

add_executable(fleetmanager ${SOURCES})
target_link_libraries(fleetmanager PRIVATE
    SQLite::SQLite3
    Threads::Threads
    pthread
    dl
)