cmake_minimum_required(VERSION 3.15)
project(AdvancedHashingModels CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /O2)
else()
    add_compile_options(-Wall -Wextra -O3 -march=native)
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCES
    src/main.cpp
    src/basic_perfect_hash.cpp
    src/basic_crypto_hash.cpp
    src/bdz_mphf.cpp
    src/ultimate_hybrid.cpp
    src/adaptive_security_hybrid.cpp
    src/parallel_verification_hybrid.cpp
    src/cache_partitioned_hybrid.cpp
    src/ultra_low_memory_hybrid.cpp
    src/two_path_hybrid.cpp
)

add_executable(hash_project ${SOURCES})

# Enable Link Time Optimization
if(NOT MSVC)
    set_target_properties(hash_project PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()
