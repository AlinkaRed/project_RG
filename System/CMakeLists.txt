cmake_minimum_required(VERSION 3.12)
project(System)

set(CMAKE_CXX_STANDARD 17)

add_executable(RadioControlSystem
    ../main.cpp
    src/ALARM.cpp
    src/Server.cpp
    src/Client.cpp
    src/SharedData.cpp
)

target_include_directories(RadioControlSystem PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ../Protocol/include
)

target_link_libraries(RadioControlSystem Protocol pthread rt)
target_compile_options(RadioControlSystem PRIVATE -Wall -Wextra)