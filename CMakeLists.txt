cmake_minimum_required(VERSION 3.28)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(oxygen-compiler VERSION 1.0.0)

file(GLOB_RECURSE SOURCES src/*.cpp)

include_directories(include/)

add_executable(oxygen ${SOURCES})
