cmake_minimum_required(VERSION 3.12)
project(Protocol)

set(CMAKE_CXX_STANDARD 17)

add_library(Protocol STATIC
    src/SystemData.cpp
    src/System.cpp
    src/SET.cpp
    src/GET.cpp
)

target_include_directories(Protocol PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(Protocol PRIVATE -Wall -Wextra)