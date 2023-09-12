set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_C_EXTENSIONS off)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/exe)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_STATIC_LIBRARY_PREFIX "")

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

#
#
# vk library
#
#
file(GLOB_RECURSE SRC vk/*.cpp)
add_library(vk SHARED ${SRC})

target_include_directories(vk PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(vk PUBLIC Vulkan::Vulkan)

#
#
# window library
#
#
file(GLOB_RECURSE SRC window/*.cpp)
add_library(window SHARED ${SRC})

target_include_directories(window PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(window PUBLIC glfw)
target_link_libraries(window PUBLIC Vulkan::Vulkan)