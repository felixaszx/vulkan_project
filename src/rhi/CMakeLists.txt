cmake_minimum_required(VERSION 3.20.0)
project(vulkan_project VERSION 0.1.0)

set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_C_EXTENSIONS off)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/exe)

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE SRC *.cpp)
add_library(rhi SHARED ${SRC})

target_include_directories(rhi PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(rhi PUBLIC Vulkan::Vulkan)