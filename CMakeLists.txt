cmake_minimum_required(VERSION 3.26)

project(NoitaSurvivor LANGUAGES CXX)

if (NOT MSVC)
    message(FATAL_ERROR "This target is intentionally MSVC-only.")
endif ()

if (MSVC)
    set(CMAKE_MAP_IMPORTED_CONFIG_DEV RelWithDebInfo Release Debug "")
endif ()

find_package(Python3 REQUIRED COMPONENTS Interpreter)

find_package(SFML 3.0.2 EXACT COMPONENTS System Window Graphics Audio CONFIG REQUIRED)
find_package(box2d 3.1.1 EXACT CONFIG REQUIRED)
find_package(fmt 10.2.1 EXACT CONFIG REQUIRED)

find_package(ImGui-SFML CONFIG REQUIRED)
find_package(spdlog 1.13.0 EXACT CONFIG REQUIRED)

find_package(magic_enum CONFIG REQUIRED)

get_target_property(NOITA_IMGUI_SFML_LINK_LIBRARIES ImGui-SFML::ImGui-SFML INTERFACE_LINK_LIBRARIES)
if (NOITA_IMGUI_SFML_LINK_LIBRARIES MATCHES "(^|;)sfml-(graphics|window|system)(;|$)")
    message(FATAL_ERROR "ImGui-SFML is using SFML 2-style CMake targets. Install or provide an ImGui-SFML package that links SFML 3 targets for SFML 3.0.2.")
endif ()

add_executable(NoitaSurvivor)

#add_custom_target(NoitaSurvivor_PreBuildCheck
#        COMMAND "python" "${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_code.py"
#        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
#        COMMENT "Running pre-build tasks..."
#        VERBATIM
#        USES_TERMINAL
#)

set(NOITA_BUILD_TIMER_FILE "${CMAKE_CURRENT_BINARY_DIR}/.noita_build_timer.txt")

add_custom_target(NoitaSurvivor_PreBuildCheck
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/scripts/build_timer.py" start "${NOITA_BUILD_TIMER_FILE}"
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_code.py"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Running pre-build tasks..."
        VERBATIM
        USES_TERMINAL
)

add_dependencies(NoitaSurvivor NoitaSurvivor_PreBuildCheck)

add_custom_command(TARGET NoitaSurvivor POST_BUILD
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/scripts/build_timer.py" end "${NOITA_BUILD_TIMER_FILE}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Build time report"
        VERBATIM
        USES_TERMINAL
)

set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT NoitaSurvivor)

target_compile_features(NoitaSurvivor PRIVATE cxx_std_20)

file(GLOB NOITA_ROOT_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

file(GLOB_RECURSE NOITA_SRC_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE NOITA_HEADERS CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

list(FILTER NOITA_SRC_SOURCES EXCLUDE REGEX "/src/meta/Meta_initComponentInitGen_example\\.cpp$")

set(NOITA_SOURCES
        ${NOITA_ROOT_SOURCES}
        ${NOITA_SRC_SOURCES}
)

target_sources(NoitaSurvivor PRIVATE
        ${NOITA_SOURCES}
        ${NOITA_HEADERS}
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "Source Files" FILES
        ${NOITA_SOURCES}
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "Header Files" FILES
        ${NOITA_HEADERS}
)

target_include_directories(NoitaSurvivor PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

#target_compile_definitions(NoitaSurvivor PRIVATE
#        UNICODE
#        _UNICODE
#        _CONSOLE
#        $<$<CONFIG:Debug>:_DEBUG>
#        $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
#        WIN32_LEAN_AND_MEAN
#        NOMINMAX
#)

target_compile_definitions(NoitaSurvivor PRIVATE
        UNICODE
        _UNICODE
        _CONSOLE
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
        $<$<CONFIG:RelWithDebInfo>:NDEBUG>
        $<$<CONFIG:MinSizeRel>:NDEBUG>
        $<$<CONFIG:Dev>:NOITA_DEV>
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)

#target_compile_options(NoitaSurvivor PRIVATE
#        /MP
#        /permissive-
#        /sdl
#        /W3
#        /wd5260
#        /wd5105
#        /utf-8
#        $<$<CONFIG:Release>:/Zi>
#)

target_compile_options(NoitaSurvivor PRIVATE
        /MP
        /permissive-
        /sdl
        /W3
        # /W4
        /wd5260
        /wd5105
        /utf-8
#        $<$<CONFIG:Debug>:/analyze>
#        $<$<CONFIG:Debug>:/analyze:external->
#        $<$<CONFIG:Dev>:/analyze>
#        $<$<CONFIG:Dev>:/analyze:external->
        $<$<CONFIG:Release>:/Zi>
        $<$<CONFIG:Dev>:/Od>
        $<$<CONFIG:Dev>:/Zi>
        $<$<CONFIG:Dev>:/Ob0>
)

target_link_libraries(NoitaSurvivor PRIVATE
        box2d::box2d
        fmt::fmt
        ImGui-SFML::ImGui-SFML
        spdlog::spdlog
        SFML::Audio
        SFML::Graphics
        SFML::Window
        SFML::System
        dbghelp
        gdi32
        opengl32
        winmm
        magic_enum::magic_enum
)

target_link_options(NoitaSurvivor PRIVATE
        $<$<CONFIG:Release>:/DEBUG>
        $<$<CONFIG:Debug>:/DEBUG>
        $<$<CONFIG:Debug>:/INCREMENTAL>
        $<$<CONFIG:Dev>:/DEBUG>
        $<$<CONFIG:Dev>:/INCREMENTAL>
)

set_property(TARGET NoitaSurvivor PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
)

set_property(TARGET NoitaSurvivor PROPERTY
        INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE
)

set_property(TARGET NoitaSurvivor PROPERTY
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

set_property(TARGET NoitaSurvivor PROPERTY
        VS_DEBUGGER_ENVIRONMENT "PATH=${CMAKE_CURRENT_SOURCE_DIR}/bin;%PATH%"
)

target_precompile_headers(NoitaSurvivor PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h"
)