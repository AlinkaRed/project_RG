# Клиентское приложение
add_executable(radio-client
    src/ClientApp.cpp
)

target_include_directories(radio-client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../System/include
)

target_link_libraries(radio-client System pthread rt)
target_compile_options(radio-client PRIVATE -Wall -Wextra)