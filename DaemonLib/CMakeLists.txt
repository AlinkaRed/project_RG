# Динамическая библиотека для демона
add_library(DaemonLib SHARED
    src/DaemonBase.cpp
)

target_include_directories(DaemonLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(DaemonLib PRIVATE -Wall -Wextra)