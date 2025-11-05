cmake_minimum_required(VERSION 3.12)
project(RadioControlSystemTests)

set(CMAKE_CXX_STANDARD 17)

# Ищем установленный Google Test
find_package(GTest REQUIRED)

# Компилируем исходные файлы Protocol и System напрямую
add_library(Protocol_STATIC STATIC
    ../Protocol/src/SystemData.cpp
    ../Protocol/src/System.cpp
    ../Protocol/src/SET.cpp
    ../Protocol/src/GET.cpp
    ../System/src/ALARM.cpp
)

# Указываем include директории для библиотеки
target_include_directories(Protocol_STATIC PUBLIC
    ../Protocol/include
    ../System/include
)

# Добавляем исполняемый файл тестов
add_executable(RadioControlSystemTests
    test_system.cpp
)

# Указываем include директории для тестов
target_include_directories(RadioControlSystemTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../Protocol/include
    ../System/include
)

# Связываем с библиотеками
target_link_libraries(RadioControlSystemTests 
    Protocol_STATIC
    GTest::gtest_main
    pthread
)

target_compile_options(RadioControlSystemTests PRIVATE -Wall -Wextra)

enable_testing()
add_test(NAME RadioControlSystemTests COMMAND RadioControlSystemTests)