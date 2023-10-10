set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_C_EXTENSIONS off)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/exe)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_STATIC_LIBRARY_PREFIX "")

find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(Assimp REQUIRED)

file(GLOB_RECURSE SRC */*.cpp *.cpp)
add_library(graphics SHARED ${SRC})

target_include_directories(graphics PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(graphics PUBLIC SDL2::SDL2)
target_link_libraries(graphics PUBLIC Vulkan::Vulkan)
target_link_libraries(graphics PUBLIC assimp)