cmake_minimum_required(VERSION 3.12)
project(RadioControlSystem)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(Protocol)
add_subdirectory(System)

find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating Doxygen documentation"
        VERBATIM
    )
endif()