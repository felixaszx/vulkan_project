cmake_minimum_required(VERSION 3.20.0)
project(vulkan_project VERSION 0.1.0)

set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_C_EXTENSIONS off)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/exe)

file(GLOB_RECURSE SRC src/*.cpp)
add_executable(vulkan_project ${SRC})