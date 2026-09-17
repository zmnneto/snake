cmake_minimum_required(VERSION 3.20)
project(snakeRaylib)

set(CMAKE_CXX_STANDARD 17)

add_executable(snakeRaylib main.cpp)

# Locate and link Raylib
find_package(raylib REQUIRED)
target_link_libraries(snakeRaylib PRIVATE raylib)