cmake_minimum_required(VERSION 3.16)

project(raycaster C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/bin")

file(GLOB_RECURSE PROJECT_SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.c"
)
include_directories("${CMAKE_SOURCE_DIR}/include")

add_subdirectory("${CMAKE_SOURCE_DIR}/external/SDL2" "${CMAKE_BINARY_DIR}/SDL2_build")

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2-static)
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/external/SDL2/include"
)