cmake_minimum_required(VERSION 3.12)
project(RadioControlSystemTests)

set(CMAKE_CXX_STANDARD 17)

find_package(GTest REQUIRED)

add_library(Protocol_STATIC STATIC
    ../Protocol/src/SystemData.cpp
    ../Protocol/src/System.cpp
    ../Protocol/src/SET.cpp
    ../Protocol/src/GET.cpp
    ../System/src/ALARM.cpp
)

target_include_directories(Protocol_STATIC PUBLIC
    ../Protocol/include
    ../System/include
)

add_executable(RadioControlSystemTests
    test_system.cpp
)

target_include_directories(RadioControlSystemTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../Protocol/include
    ../System/include
)

target_link_libraries(RadioControlSystemTests 
    Protocol_STATIC
    GTest::gtest_main
    pthread
)

target_compile_options(RadioControlSystemTests PRIVATE -Wall -Wextra)

enable_testing()
add_test(NAME RadioControlSystemTests COMMAND RadioControlSystemTests)