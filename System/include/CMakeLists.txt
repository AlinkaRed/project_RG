cmake_minimum_required(VERSION 3.12)
project(RadioControlSystem)

set(CMAKE_CXX_STANDARD 17)

add_library(Protocol
    ../Protocol/src/SystemData.cpp
    ../Protocol/src/System.cpp
    ../Protocol/src/SET.cpp
    ../Protocol/src/GET.cpp
)

add_executable(RadioControlSystem
    ../main.cpp
    ALARM.cpp
    Server.cpp
    Client.cpp
    SharedData.cpp
)

target_link_libraries(RadioControlSystem Protocol)

target_link_libraries(RadioControlSystem pthread rt)

find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating Doxygen documentation"
        VERBATIM
    )
endif()