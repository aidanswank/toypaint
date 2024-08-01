cmake_minimum_required(VERSION 3.23)

# Set the project name
set(PROJECT_NAME simple_paint)
project(${PROJECT_NAME})

# Set C++17 as the standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find SDL2
find_package(SDL2 REQUIRED)

# Hardcode the paths for SDL2_ttf
set(SDL2_TTF_INCLUDE_DIRS /opt/homebrew/Cellar/sdl2_ttf/2.22.0/include/SDL2)
set(SDL2_TTF_LIBRARIES /opt/homebrew/Cellar/sdl2_ttf/2.22.0/lib/libSDL2_ttf.dylib)

message(STATUS "SDL2_INCLUDE_DIR: ${SDL2_INCLUDE_DIRS}")
message(STATUS "SDL2_LIBRARY: ${SDL2_LIBRARIES}")
message(STATUS "SDL2_TTF_INCLUDE_DIR: ${SDL2_TTF_INCLUDE_DIRS}")
message(STATUS "SDL2_TTF_LIBRARY: ${SDL2_TTF_LIBRARIES}")

add_executable(${PROJECT_NAME} main.cpp)

include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES})
