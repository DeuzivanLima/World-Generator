cmake_minimum_required(VERSION 3.19)
project(World-Generator VERSION 0.0.1)

add_executable(${PROJECT_NAME} 
    src/main.cpp
)

# - Subdirectorys
add_subdirectory(deps/raylib)

# - Taget Insertions
target_include_directories(${PROJECT_NAME} PRIVATE
    include)
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -pedantic)
target_link_libraries(${PROJECT_NAME} PRIVATE
    raylib)