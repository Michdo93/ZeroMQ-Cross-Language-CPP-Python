cmake_minimum_required(VERSION 3.16)
project(cpp_zmq_json_server LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBZMQ REQUIRED IMPORTED_TARGET libzmq)
find_package(nlohmann_json 3.2 REQUIRED)

add_executable(server server.cpp)
target_link_libraries(server PRIVATE PkgConfig::LIBZMQ nlohmann_json::nlohmann_json)
