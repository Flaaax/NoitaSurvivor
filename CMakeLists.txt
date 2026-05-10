cmake_minimum_required(VERSION 3.26)

project(NoitaSurvivor LANGUAGES CXX)

if(NOT MSVC)
    message(FATAL_ERROR "This first CMake migration target is intentionally MSVC-only.")
endif()

find_package(boost_locale 1.89.0 EXACT CONFIG REQUIRED)
find_package(SFML 2.6.1 EXACT COMPONENTS system window graphics audio CONFIG REQUIRED)
find_package(box2d 2.4.1 EXACT CONFIG REQUIRED)
find_package(fmt 10.2.1 EXACT CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)
find_package(nlohmann_json 3.11.3 EXACT CONFIG REQUIRED)
find_package(spdlog 1.13.0 EXACT CONFIG REQUIRED)

add_executable(NoitaSurvivor)
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

target_compile_definitions(NoitaSurvivor PRIVATE
    UNICODE
    _UNICODE
    _CONSOLE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

target_compile_options(NoitaSurvivor PRIVATE
    /MP
    /permissive-
    /sdl
    /W3
    /wd5260
    /wd5105
    $<$<CONFIG:Release>:/Zi>
)

target_link_libraries(NoitaSurvivor PRIVATE
    Boost::locale
    ImGui-SFML::ImGui-SFML
    GLEW::GLEW
    box2d::box2d
    fmt::fmt
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    sfml-audio
    sfml-graphics
    sfml-window
    sfml-system
    dbghelp
    gdi32
    opengl32
    winmm
)

target_link_options(NoitaSurvivor PRIVATE
    $<$<CONFIG:Debug>:/INCREMENTAL:NO>
    $<$<CONFIG:Release>:/DEBUG>
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
