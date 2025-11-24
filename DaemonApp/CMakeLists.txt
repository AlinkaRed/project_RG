# Приложение демона
add_executable(radio-server
    src/DaemonApp.cpp
)

target_include_directories(radio-server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../System/include
)

target_link_libraries(radio-server System DaemonLib pthread rt)
target_compile_options(radio-server PRIVATE -Wall -Wextra)