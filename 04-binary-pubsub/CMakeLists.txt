cmake_minimum_required(VERSION 3.16)
project(cpp_zmq_binary_pub LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBZMQ REQUIRED IMPORTED_TARGET libzmq)

add_executable(publisher publisher.cpp)
target_link_libraries(publisher PRIVATE PkgConfig::LIBZMQ)
