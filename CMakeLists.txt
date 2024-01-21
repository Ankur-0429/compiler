cmake_minimum_required(VERSION 3.22.1)
project(compiler)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/cmake-build-debug)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LLVM_DIR /opt/homebrew/opt/llvm/lib/cmake/llvm)
find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(hydro src/main.cpp include/tokenization.h src/tokenization.cpp src/parser.cpp include/parser.h src/generation.cpp include/generation.h include/arenaAllocator.h)

llvm_map_components_to_libnames(llvm_libs support core irreader)
target_link_libraries(hydro ${llvm_libs})

project(try_googletest
        VERSION 0.0.1
        DESCRIPTION "Trying googletest"
        LANGUAGES CXX
        )
include(CTest)
add_subdirectory(external)
add_subdirectory(tests)