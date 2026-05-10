cmake_minimum_required(VERSION 3.26)

project(NoitaSurvivor LANGUAGES CXX)

if(NOT MSVC)
    message(FATAL_ERROR "This first CMake migration target is intentionally MSVC-only.")
endif()

set(BOOST_1_86_ROOT "E:/C++ sources/boost/boost_1_86_0" CACHE PATH "Boost 1.86 root")
set(BOOST_1_86_LIB_DIR "${BOOST_1_86_ROOT}/stage/lib" CACHE PATH "Boost 1.86 library directory")

add_library(BoostLocale::BoostLocale UNKNOWN IMPORTED)
set_target_properties(BoostLocale::BoostLocale PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${BOOST_1_86_ROOT}"
    IMPORTED_LOCATION_DEBUG "${BOOST_1_86_LIB_DIR}/libboost_locale-vc143-mt-gd-x64-1_86.lib"
    IMPORTED_LOCATION_RELEASE "${BOOST_1_86_LIB_DIR}/libboost_locale-vc143-mt-x64-1_86.lib"
    IMPORTED_LOCATION_RELWITHDEBINFO "${BOOST_1_86_LIB_DIR}/libboost_locale-vc143-mt-x64-1_86.lib"
    IMPORTED_LOCATION_MINSIZEREL "${BOOST_1_86_LIB_DIR}/libboost_locale-vc143-mt-x64-1_86.lib"
)

find_package(SFML 2.6 COMPONENTS system window graphics audio CONFIG REQUIRED)
find_package(box2d 2 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_executable(NoitaSurvivor)

target_compile_features(NoitaSurvivor PRIVATE cxx_std_20)

target_sources(NoitaSurvivor PRIVATE
    main.cpp
    src/game/Components/EntityFactory.cpp
    src/game/Contact/ContactFilter.cpp
    src/game/Contact/ContactListener.cpp
    src/game/Game.cpp
    src/game/Services/GameService.cpp
    src/game/Services/PhysicsBodyService.cpp
    src/game/Services/PlayerService.cpp
    src/game/Spells/Modifiers/Modifiers.cpp
    src/game/Spells/Projectiles/Projectiles.cpp
    src/game/Spells/Shot.cpp
    src/game/Spells/Spell.cpp
    src/game/Spells/SpellBlock.cpp
    src/game/Systems/EntityDestroySystem.cpp
    src/game/Systems/GameStateSystem.cpp
    src/game/Systems/GameSystem.cpp
    src/game/Systems/PhysicsSystem.cpp
    src/game/Systems/RenderSystem.cpp
    src/game/Wands/Wand.cpp
    src/global/AssetManager.cpp
    src/global/DataManager.cpp
    src/global/SpriteManager.cpp
    src/gui/game/SpellInventory.cpp
    src/gui/game/SpellSelector.cpp
    src/gui/shapes/NRoundRectShape.cpp
    src/gui/HealthBar.cpp
    src/gui/MaterialBar.cpp
    src/gui/NButton.cpp
    src/gui/NSceneManager.cpp
    src/gui/NText.cpp
    src/gui/NWidget.cpp
    src/gui/NWindow.cpp
    src/gui/Renderer.cpp
    src/meta/ComponentMeta.cpp
    src/meta/EnumMeta.cpp
    src/meta/generated/Meta_initComponentInitGen.cpp
    src/scenes/GameScene.cpp
    src/scenes/MenuScene.cpp
    src/utils/Debug.cpp
    src/utils/NString.cpp
)

target_include_directories(NoitaSurvivor PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_definitions(NoitaSurvivor PRIVATE
    _CONSOLE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

target_compile_options(NoitaSurvivor PRIVATE
    /permissive-
    /sdl
    /W3
    /wd5260
    /wd5105
)

target_link_libraries(NoitaSurvivor PRIVATE
    BoostLocale::BoostLocale
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

set_property(TARGET NoitaSurvivor PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
)

set_property(TARGET NoitaSurvivor PROPERTY
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

set_property(TARGET NoitaSurvivor PROPERTY
    VS_DEBUGGER_ENVIRONMENT "PATH=${CMAKE_CURRENT_SOURCE_DIR}/bin;%PATH%"
)
