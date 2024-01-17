cmake_minimum_required(VERSION 3.25)
project(compiler)

set(CMAKE_CXX_STANDARD 17)

set(LLVM_DIR /opt/homebrew/opt/llvm/lib/cmake/llvm)
find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(compiler src/main.cpp src/tokenization.h src/tokenization.cpp src/parser.cpp src/parser.h src/generation.cpp src/generation.h src/arenaAllocator.h)

llvm_map_components_to_libnames(llvm_libs support core irreader)
target_link_libraries(compiler ${llvm_libs})