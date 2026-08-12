cmake_minimum_required(VERSION 4.0)
project(imgui_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Locate system libraries
find_package(SDL3 REQUIRED)

option(ENABLE_ASAN "Enable AddressSanitizer" OFF)

# Source files
file(GLOB IMGUI_SOURCES "vendor/imgui/*.cpp")
file(GLOB MAIN_SOURCES "src/*.c" "src/*.cpp")
file(GLOB MAIN_FILE "main.c")
add_executable(${PROJECT_NAME} main.c ${IMGUI_SOURCES} ${MAIN_SOURCES} ${MAIN_FILE})

# ASAN Enable
if(ENABLE_ASAN)
  target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address -fno-omit-frame-pointer)
  target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
endif()

# Target configuration
target_include_directories(${PROJECT_NAME} PRIVATE vendor/imgui)
target_include_directories(${PROJECT_NAME} PRIVATE include/)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

