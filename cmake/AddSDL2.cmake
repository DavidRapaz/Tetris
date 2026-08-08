# Provides the SDL2, SDL2_image and SDL2_ttf imported targets.
#
# TETRIS_FETCH_SDL=ON (default on MSVC)
#   CMake downloads the official prebuilt development packages into the build
#   tree (<build>/_deps) and creates the imported targets from them. Nothing SDL
#   related has to be committed to the repository.
#
# TETRIS_FETCH_SDL=OFF
#   The libraries are looked up on the system instead (vcpkg, apt, brew, ...).
#
# Either way the following targets exist afterwards:
#   SDL2::SDL2  SDL2::SDL2main  SDL2_image::SDL2_image  SDL2_ttf::SDL2_ttf

include_guard(GLOBAL)
include(FetchContent)

set(TETRIS_SDL2_VERSION "2.28.4" CACHE STRING "SDL2 version to download")
set(TETRIS_SDL2_IMAGE_VERSION "2.6.3" CACHE STRING "SDL2_image version to download")
set(TETRIS_SDL2_TTF_VERSION "2.20.2" CACHE STRING "SDL2_ttf version to download")

# SHA256 of the prebuilt packages, so a tampered or truncated download fails
# loudly instead of producing a broken build. Update these together with the
# versions above.
set(TETRIS_SDL2_SHA256 "fd77706dfd5c8ca1e0e3275b6e8f581c440025367747bcb151cad878d651e093")
set(TETRIS_SDL2_IMAGE_SHA256 "5e54fa63d36358c2a310cc8ec1f5fda524c3011b1f7af1ebeadc58fef0c55ed9")
set(TETRIS_SDL2_TTF_SHA256 "887b2b9d53c23f199c0d358a9d518d14f5cfd5245cde678747faa77535e60b5b")

# Downloads and unpacks one "<name>-devel-<version>-VC" package, returning the
# directory it was unpacked into.
function(_tetris_fetch_sdl_package name url hash root_var)
    FetchContent_Declare(${name}
        URL "${url}"
        URL_HASH "SHA256=${hash}"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(${name})
    set(${root_var} "${${name}_SOURCE_DIR}" PARENT_SCOPE)
endfunction()

# Creates the imported target for a library of an unpacked VC package. The
# packages ship their own CMake config files, but those still ask for
# cmake_minimum_required(VERSION 3.0), which CMake 4 refuses to run, so the
# targets are declared here instead.
function(_tetris_add_imported_sdl_library target root basename arch_subdir)
    set(_implib "${root}/lib/${arch_subdir}/${basename}.lib")
    set(_dll "${root}/lib/${arch_subdir}/${basename}.dll")

    if(NOT EXISTS "${_implib}")
        message(FATAL_ERROR "Expected ${_implib} in the downloaded package, but it is missing.")
    endif()

    if(EXISTS "${_dll}")
        add_library(${target} SHARED IMPORTED GLOBAL)
        set_target_properties(${target} PROPERTIES
            IMPORTED_IMPLIB "${_implib}"
            IMPORTED_LOCATION "${_dll}"
            INTERFACE_INCLUDE_DIRECTORIES "${root}/include"
        )
    else()
        # SDL2main is a static library without a runtime component.
        add_library(${target} STATIC IMPORTED GLOBAL)
        set_target_properties(${target} PROPERTIES
            IMPORTED_LOCATION "${_implib}"
            INTERFACE_INCLUDE_DIRECTORIES "${root}/include"
        )
    endif()
endfunction()

function(tetris_provide_sdl)
    if(NOT TETRIS_FETCH_SDL)
        find_package(SDL2 REQUIRED)
        find_package(SDL2_image REQUIRED)
        find_package(SDL2_ttf REQUIRED)
        return()
    endif()

    if(NOT MSVC)
        message(FATAL_ERROR
            "TETRIS_FETCH_SDL only supports MSVC, because it uses the prebuilt "
            "Visual C++ packages from libsdl.org. Install SDL2, SDL2_image and "
            "SDL2_ttf with your package manager and configure with "
            "-DTETRIS_FETCH_SDL=OFF.")
    endif()

    if(CMAKE_VS_PLATFORM_NAME MATCHES "ARM" OR CMAKE_SYSTEM_PROCESSOR MATCHES "^([Aa][Rr][Mm]|aarch64)")
        message(FATAL_ERROR
            "The prebuilt SDL packages only ship x86 and x64 binaries. Build for "
            "x64 or configure with -DTETRIS_FETCH_SDL=OFF.")
    endif()

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(_arch "x64")
    else()
        set(_arch "x86")
    endif()

    _tetris_fetch_sdl_package(sdl2
        "https://github.com/libsdl-org/SDL/releases/download/release-${TETRIS_SDL2_VERSION}/SDL2-devel-${TETRIS_SDL2_VERSION}-VC.zip"
        "${TETRIS_SDL2_SHA256}" _sdl2_root)
    _tetris_fetch_sdl_package(sdl2_image
        "https://github.com/libsdl-org/SDL_image/releases/download/release-${TETRIS_SDL2_IMAGE_VERSION}/SDL2_image-devel-${TETRIS_SDL2_IMAGE_VERSION}-VC.zip"
        "${TETRIS_SDL2_IMAGE_SHA256}" _sdl2_image_root)
    _tetris_fetch_sdl_package(sdl2_ttf
        "https://github.com/libsdl-org/SDL_ttf/releases/download/release-${TETRIS_SDL2_TTF_VERSION}/SDL2_ttf-devel-${TETRIS_SDL2_TTF_VERSION}-VC.zip"
        "${TETRIS_SDL2_TTF_SHA256}" _sdl2_ttf_root)

    _tetris_add_imported_sdl_library(SDL2::SDL2 "${_sdl2_root}" "SDL2" "${_arch}")
    _tetris_add_imported_sdl_library(SDL2::SDL2main "${_sdl2_root}" "SDL2main" "${_arch}")
    _tetris_add_imported_sdl_library(SDL2_image::SDL2_image "${_sdl2_image_root}" "SDL2_image" "${_arch}")
    _tetris_add_imported_sdl_library(SDL2_ttf::SDL2_ttf "${_sdl2_ttf_root}" "SDL2_ttf" "${_arch}")

    message(STATUS "Using downloaded SDL2 ${TETRIS_SDL2_VERSION}, "
                   "SDL2_image ${TETRIS_SDL2_IMAGE_VERSION} and "
                   "SDL2_ttf ${TETRIS_SDL2_TTF_VERSION} (${_arch})")
endfunction()
