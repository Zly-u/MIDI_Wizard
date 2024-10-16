set(SUB_PROJECT_NAME "Core")

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SOURCES "*.cpp" "*.c" "*.hpp" "*.h")
add_library(${SUB_PROJECT_NAME} STATIC ${SOURCES})

# Allows for the source files from "src" folder to be accessible in anything that includes this library.
target_include_directories(${SUB_PROJECT_NAME} PUBLIC src)
target_include_directories(${SUB_PROJECT_NAME} PUBLIC libs)

################################################################

set(
    G_CORE_INCLUDE_DIRS

    ${CMAKE_SOURCE_DIR}/CORE
    ${CMAKE_SOURCE_DIR}/CORE/libs/imgui
    ${CMAKE_SOURCE_DIR}/CORE/libs/SDL/include
    ${CMAKE_SOURCE_DIR}/CORE/libs/SDL_image/include
    ${CMAKE_SOURCE_DIR}/CORE/libs/stb
    ${CMAKE_SOURCE_DIR}/CORE/libs/tinyfiledialogs

    CACHE INTERNAL ""
)
foreach(dir IN LISTS G_CORE_INCLUDE_DIRS)
    include_directories(${dir})
endforeach()

#libs
target_link_directories(
    ${SUB_PROJECT_NAME}
    PUBLIC ${CMAKE_SOURCE_DIR}/CORE/libs/SDL/lib/x64
    PUBLIC ${CMAKE_SOURCE_DIR}/CORE/libs/SDL_image/lib/x64
)

target_link_libraries(
    ${SUB_PROJECT_NAME}
    LINK_PUBLIC SDL2.lib
    LINK_PUBLIC SDL2main.lib
    LINK_PUBLIC SDL2_image.lib
)

set(
    SDL2_DLLS
    ${CMAKE_SOURCE_DIR}/CORE/libs/SDL/lib/x64/SDL2.dll
    ${CMAKE_SOURCE_DIR}/CORE/libs/SDL_image/lib/x64/SDL2_image.dll
)

################################################################

set_target_properties(
    ${SUB_PROJECT_NAME} PROPERTIES
    PREFIX ""
    OUTPUT_NAME "${SUB_PROJECT_NAME}"
)

install(
    TARGETS ${SUB_PROJECT_NAME}
    RUNTIME DESTINATION ${G_INSTALL_DIR}
    LIBRARY DESTINATION ${G_INSTALL_DIR}
    ARCHIVE DESTINATION ${G_INSTALL_DIR}
)

install(
    FILES ${SDL2_DLLS}
    DESTINATION ${G_INSTALL_DIR}
)