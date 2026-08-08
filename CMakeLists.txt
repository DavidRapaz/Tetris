cmake_minimum_required(VERSION 3.21)

project(Tetris VERSION 1.0.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(TETRIS_FETCH_SDL "Download SDL2, SDL2_image and SDL2_ttf into the build tree" ${MSVC})

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

include(AddSDL2)
tetris_provide_sdl()

add_executable(Tetris
    Tetris.cpp
    Color.h
    Shape.h
    State.h
    classes/Piece.cpp
    classes/Piece.h
    classes/Renderer.cpp
    classes/Renderer.h
    classes/screen/Game.cpp
    classes/screen/Game.h
    classes/screen/Menu.cpp
    classes/screen/Menu.h
    classes/screen/Screen.h
)

target_compile_features(Tetris PRIVATE cxx_std_17)

# SDL_ttf.h pulls in SDL_main.h, which renames main() to SDL_main(), so
# SDL2main has to be linked even though this is a console application.
target_link_libraries(Tetris PRIVATE
    SDL2::SDL2main
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
)

# /permissive- matches the ConformanceMode of the old Visual Studio project.
# No /utf-8 here: some sources still use ISO-8859-1 characters in comments.
target_compile_options(Tetris PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/permissive->)

# The game loads its assets through relative paths, so they have to sit next to
# the executable and the debugger has to start there.
add_custom_command(TARGET Tetris POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:Tetris>/assets"
    COMMENT "Copying assets next to the executable"
    VERBATIM
)

if(WIN32)
    add_custom_command(TARGET Tetris POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_RUNTIME_DLLS:Tetris>" "$<TARGET_FILE_DIR:Tetris>"
        COMMAND_EXPAND_LISTS
        COMMENT "Copying the SDL DLLs next to the executable"
    )
endif()

set_target_properties(Tetris PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Tetris>"
)

set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT Tetris)

get_target_property(_tetris_sources Tetris SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${_tetris_sources})

install(TARGETS Tetris RUNTIME DESTINATION .)
install(DIRECTORY assets DESTINATION .)
if(WIN32)
    install(FILES $<TARGET_RUNTIME_DLLS:Tetris> DESTINATION .)
endif()
