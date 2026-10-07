cmake_minimum_required(VERSION 3.16)
project(FleetManager CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third_party
)

set(SQLite3_INCLUDE_DIRS "$ENV{HOME}/local/include")
set(SQLite3_LIBRARIES "$ENV{HOME}/local/lib/libsqlite3.so")
include_directories(${SQLite3_INCLUDE_DIRS})

set(SOURCES
    src/server/main.cpp
    src/utils/db.cpp
    src/utils/password.cpp
    src/models/user.cpp
    src/repositories/user_repository.cpp
    src/services/auth_service.cpp
)

add_executable(fleetmanager ${SOURCES})
target_link_libraries(fleetmanager PRIVATE ${SQLite3_LIBRARIES})