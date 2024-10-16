cmake_minimum_required(VERSION 3.29)

set(PROJECT_NAME MIDI_Wizard)

set(CMAKE_CXX_STANDARD 23)

project(
    ${PROJECT_NAME}
    VERSION 1.0
    DESCRIPTION "Midi Visualizer and Editing App."
    LANGUAGES C CXX
)

################################################################

#set(BUILD_TYPE $<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>)
#set(G_INSTALL_DIR ${CMAKE_SOURCE_DIR}/__BUILD__/${BUILD_TYPE} CACHE INTERNAL "")
set(G_INSTALL_DIR ${CMAKE_SOURCE_DIR}/__BUILD__/ CACHE INTERNAL "")

################################################################

if(WIN32)

endif()

################################################################

add_subdirectory(APP)
add_subdirectory(CORE)

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_compile_options("-O3")
endif()