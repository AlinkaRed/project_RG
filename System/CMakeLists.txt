# Библиотека System с поддержкой демона
add_library(System
    src/Alarm.cpp
    src/Server.cpp  
    src/Client.cpp
    src/SharedData.cpp
    src/ServerDaemon.cpp
)

target_include_directories(System PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../Protocol/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../DaemonLib/include
)

target_link_libraries(System Protocol DaemonLib pthread rt)
target_compile_options(System PRIVATE -Wall -Wextra)