cmake_minimum_required(VERSION 3.20.0)
project(project VERSION 0.1.0)

set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_C_EXTENSIONS off)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/exe)

find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)

file(GLOB_RECURSE SRC src/*.cpp)
add_executable(project main.cpp ${SRC})

target_include_directories(project PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(project PUBLIC SDL2::SDL2)
target_link_libraries(project PUBLIC Vulkan::Vulkan)
