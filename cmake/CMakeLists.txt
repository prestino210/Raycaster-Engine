cmake_minimum_required(VERSION 3.16)
project(raycaster C)

set(CMAKE_C_STANDARD 99)

# Set FetchContent dependencies folder
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/../build/lib)

# Set executable output folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../build/bin)

include(FetchContent)
FetchContent_Declare(
    SDL2
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/../external/SDL2
)
FetchContent_MakeAvailable(SDL2)

add_executable(raycaster ${CMAKE_SOURCE_DIR}/../main.c)
target_link_libraries(raycaster PRIVATE SDL2::SDL2 m)
