set(SUB_PROJECT_NAME "App")
set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SOURCES "*.cpp" "*.c" "*.hpp" "*.h")

add_executable(${SUB_PROJECT_NAME} ${SOURCES})

target_include_directories(${SUB_PROJECT_NAME} PUBLIC src)

set(
    G_APP_INCLUDE_DIRS

    ${CMAKE_SOURCE_DIR}/APP
    ${CMAKE_SOURCE_DIR}/APP/libs/imgui

    CACHE INTERNAL ""
)

#include files from APP project
foreach(dir IN LISTS G_APP_INCLUDE_DIRS)
    include_directories(${dir})
endforeach()

################################################################

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(${SUB_PROJECT_NAME} PRIVATE _DEBUG=1)
    target_compile_definitions(${SUB_PROJECT_NAME} PRIVATE MTR_ENABLED=1)
endif()

################################################################

#include files from CORE project
foreach(dir IN LISTS G_CORE_INCLUDE_DIRS)
    include_directories(${dir})
endforeach()

target_link_libraries(${SUB_PROJECT_NAME} PUBLIC "Core")

################################################################

install(
    TARGETS ${SUB_PROJECT_NAME}
    RUNTIME DESTINATION ${G_INSTALL_DIR}
    LIBRARY DESTINATION ${G_INSTALL_DIR}
    ARCHIVE DESTINATION ${G_INSTALL_DIR}
)
install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/APP/res
    DESTINATION ${G_INSTALL_DIR}
)