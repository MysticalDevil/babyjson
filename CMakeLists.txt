cmake_minimum_required(VERSION 3.29)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(COMPILE_OPTIONS -Wall -Wextra -Werror)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(babyjson LANGUAGES CXX)

set(SOURCES src/main.cpp
        src/babyjson.cpp
        src/helpers.cpp
)

add_executable(babyjson ${SOURCES})

target_include_directories(babyjson
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)
