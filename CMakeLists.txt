cmake_minimum_required(VERSION 3.20)

project(thread_pool CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)

find_package(Catch2 CONFIG REQUIRED)

# IF(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
#     MESSAGE(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
#     FILE(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/release/0.18/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake")
# ENDIF()

# INCLUDE(${CMAKE_BINARY_DIR}/conan.cmake)


add_executable(Test src/sarna/test.cpp)
target_link_libraries(Test PRIVATE Catch2::Catch2WithMain)
