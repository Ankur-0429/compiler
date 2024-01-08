cmake_minimum_required(VERSION 3.25)
project(compiler)

set(CMAKE_CXX_STANDARD 17)

add_executable(compiler src/main.cpp src/tokenization.h src/tokenization.cpp src/parser.cpp src/parser.h src/generation.cpp src/generation.h src/arenaAllocator.h)
