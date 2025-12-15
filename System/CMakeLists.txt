cmake_minimum_required(VERSION 3.10)
project(System)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/Alarm.cpp
    src/Client.cpp
    src/Server.cpp
    src/ServerDaemon.cpp
    src/SharedData.cpp
    src/MONITOR.cpp
)

set(HEADERS
    include/Alarm.h
    include/Client.h
    include/Server.h
    include/ServerDaemon.h
    include/SharedData.h
    include/MONITOR.h
)

add_library(System STATIC ${SOURCES} ${HEADERS})

target_include_directories(System PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../Protocol/include
)

# Зависимости
target_link_libraries(System Protocol DaemonLib)