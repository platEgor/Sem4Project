cmake_minimum_required(VERSION 3.22)

project(Sem4Project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_PREFIX_PATH "C:/SFML")

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

add_executable(Sem4Project
    src/main.cpp
    src/render/DrawPrimitives.cpp
    src/render/SetupGuides.cpp
    src/core/MathUtils.cpp
    src/core/Format.cpp
    src/physics/OrbitMath.cpp
    src/physics/OrbitPrediction.cpp
    src/ui/UiTheme.cpp
    src/ui/PopUpMessageLayer.cpp
    src/ui/SetupScreen.cpp
    src/render/OrbitTrail.cpp
    src/render/ApsisMarkerRenderer.cpp
    src/render/EarthTextureSwitcher.cpp
    src/storage/OrbitStorage.cpp
)

target_include_directories(Sem4Project PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(Sem4Project
    PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
)