cmake_minimum_required(VERSION 3.26)
project(FlatDawg C)

set(CMAKE_C_STANDARD 99)

set(SDL_PATH "C:/SDL2/include")
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(FlatDawg src/main.c
)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
