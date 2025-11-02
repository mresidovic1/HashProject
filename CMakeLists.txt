cmake_minimum_required(VERSION 3.10)
project(HashProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Sve src fajlove osim main.cpp
file(GLOB_RECURSE ALL_SOURCES src/*.cpp)
list(FILTER ALL_SOURCES EXCLUDE REGEX ".*main\\.cpp$")
set(SOURCES_NO_MAIN ${ALL_SOURCES})

# Demo program
add_executable(PasswordSecurityDemo
    src/main.cpp
    ${SOURCES_NO_MAIN}
)

# Test program
file(GLOB_RECURSE TEST_SOURCES tests/*.cpp)
add_executable(test_password_hash
    ${TEST_SOURCES}
    ${SOURCES_NO_MAIN}
)
