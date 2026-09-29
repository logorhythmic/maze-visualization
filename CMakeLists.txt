cmake_minimum_required(VERSION 3.20)
project(Maze-Vis)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


option(ENABLE_ASAN "Enable AddressSanitizer" OFF)

# Source files
file(GLOB IMGUI_SOURCES "vendor/imgui/*.cpp")
file(GLOB MAIN_SOURCES "src/*.c" "src/*.cpp")
file(GLOB MAIN_FILE "main.c")

add_executable(${PROJECT_NAME} main.c ${IMGUI_SOURCES} ${MAIN_SOURCES} ${MAIN_FILE})
target_include_directories(${PROJECT_NAME} PRIVATE vendor/imgui)
target_include_directories(${PROJECT_NAME} PRIVATE include/)

if(EMSCRIPTEN)
    message(STATUS "Building for Web (Emscripten)")

    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
	# Optimization
	target_compile_options(${PROJECT_NAME} PRIVATE -Os)
	target_link_options(${PROJECT_NAME} PRIVATE
	  -Os
	  # "-sFILESYSTEM=0"
	  "-sINITIAL_MEMORY=67108864"  # Inital Memory - 64 MB
	  "-sALLOW_MEMORY_GROWTH=1"
	)
	

    # Use Emscripten built-in SDL3 port
    target_compile_options(${PROJECT_NAME} PRIVATE "-sUSE_SDL=3")
    target_link_options(${PROJECT_NAME} PRIVATE "-sUSE_SDL=3")
	target_link_options(${PROJECT_NAME} PRIVATE "--shell-file" "${CMAKE_CURRENT_SOURCE_DIR}/web/Maze-Vis.html")


else()
# Locate system libraries
find_package(SDL3 REQUIRED)
  # ASAN Enable
  if(ENABLE_ASAN)
	target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address -fno-omit-frame-pointer)
	target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
  endif()

  # Target configuration
  target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
endif()

