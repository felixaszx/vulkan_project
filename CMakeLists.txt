cmake_minimum_required(VERSION 3.20.0)
project(project VERSION 0.1.0)

set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_C_EXTENSIONS off)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/exe)

add_subdirectory(src)

add_executable(project main.cpp)

target_include_directories(project PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(project PUBLIC vk)
target_link_libraries(project PUBLIC window)