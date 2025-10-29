cmake_minimum_required(VERSION 3.12)
project(RadioControlSystem)

set(CMAKE_CXX_STANDARD 17)

add_executable(RadioControlSystem
    main.cpp
    ALARM.cpp
    Protocol/SystemData.cpp
    Protocol/System.cpp
    Protocol/SET.cpp
    Protocol/GET.cpp
)

add_custom_target(docs
    COMMAND doxygen Doxyfile
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating documentation"
)