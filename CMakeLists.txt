cmake_minimum_required(VERSION 3.21)

project(colibry-settings
    VERSION 0.2.0
    DESCRIPTION "Settings for the Colibry"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(Colibry_BUILD_TESTS "Build unit tests" ON)
option(Colibry_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

set(_qt_components Widgets Network)
if(Colibry_BUILD_TESTS)
    list(APPEND _qt_components Test)
endif()
find_package(Qt6 6.4 REQUIRED COMPONENTS ${_qt_components})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(ColibryApps)

if(Colibry_BUILD_TESTS)
    enable_testing()
endif()

add_subdirectory(common)
add_library(gus_settings_core STATIC
    src/core/SystemBackend.h
    src/core/SystemBackend.cpp
    src/core/SettingsApplier.h
    src/core/SettingsApplier.cpp
    src/pages/SettingsPage.h
    src/pages/SettingsPage.cpp
    src/pages/AppearancePage.h
    src/pages/AppearancePage.cpp
    src/pages/LanguageRegionPage.h
    src/pages/LanguageRegionPage.cpp
    src/pages/DisplayPage.h
    src/pages/DisplayPage.cpp
    src/pages/SoundPage.h
    src/pages/SoundPage.cpp
    src/pages/NetworkPage.h
    src/pages/NetworkPage.cpp
    src/pages/DateTimePage.h
    src/pages/DateTimePage.cpp
    src/pages/AboutPage.h
    src/pages/AboutPage.cpp
    src/MainWindow.h
    src/MainWindow.cpp
)
target_include_directories(gus_settings_core PUBLIC src)
target_link_libraries(gus_settings_core PUBLIC colibry_common Qt6::Widgets Qt6::Network)
target_compile_definitions(gus_settings_core PUBLIC Colibry_SETTINGS_VERSION="${PROJECT_VERSION}")
colibry_target_defaults(gus_settings_core)

add_executable(colibry-settings src/main.cpp)
target_link_libraries(colibry-settings PRIVATE gus_settings_core)
colibry_target_defaults(colibry-settings)

colibry_install_app(colibry-settings
    ${CMAKE_CURRENT_SOURCE_DIR}/packaging/colibry-settings.desktop
    ${CMAKE_CURRENT_SOURCE_DIR}/packaging/colibry-settings.svg)

if(Colibry_BUILD_TESTS)
    colibry_add_test(settings_ui SOURCES tests/test_ui.cpp LINK gus_settings_core)
endif()
