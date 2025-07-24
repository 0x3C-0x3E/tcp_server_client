cmake_minimum_required(VERSION 3.10)
project(tcp_project C)

set(CMAKE_C_STANDARD 11)

# Recursively gather all C and H files in /src
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.c src/*.h)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_compile_options(${PROJECT_NAME} PRIVATE -g -Wall)


