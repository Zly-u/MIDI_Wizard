set(SUB_PROJECT_NAME "App")
set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SOURCES "*.cpp" "*.c" "*.hpp" "*.h")

add_executable(${SUB_PROJECT_NAME} ${SOURCES})

#include files from CORE project
include_directories(${CMAKE_SOURCE_DIR}/CORE)
include_directories(${CMAKE_SOURCE_DIR}/CORE/libs/imgui)
include_directories(${CMAKE_SOURCE_DIR}/CORE/libs/SDL/include)
include_directories(${CMAKE_SOURCE_DIR}/CORE/libs/SDL_image/include)
include_directories(${CMAKE_SOURCE_DIR}/CORE/libs/stb)
include_directories(${CMAKE_SOURCE_DIR}/CORE/libs/tinyfiledialogs)

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