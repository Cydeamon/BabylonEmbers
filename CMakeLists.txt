cmake_minimum_required(VERSION 3.5.0)
project(TowerDefender VERSION 0.3.1)
set (CMAKE_CXX_STANDARD 17)
set (PROJECT_LABEL "Tower Defender")

# Dependencies
set(RAYLIB_VERSION 5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) 
if (NOT raylib_FOUND) 
    include(FetchContent)
    FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
endif()

add_subdirectory(Sources/Engine/Vendor/Box2D)

# Project
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Bin)

add_executable(
    ${PROJECT_NAME} 
    Sources/Main.cpp 
    resource.rc
    Sources/Config.h
    Sources/Engine/Engine.h
    Sources/Engine/Engine.cpp
    Sources/Engine/GameObjects/GameObject.h
    Sources/Engine/GameObjects/GameObject.cpp
    Sources/Engine/GameObjects/PhysicsRectangle.cpp
    Sources/Engine/GameObjects/PhysicsRectangle.cpp
    Sources/Game/Brick.h
    Sources/Game/Brick.cpp
    Sources/Game/Player.h
    Sources/Game/Player.cpp
    Sources/Game/TowerTop.h
    Sources/Game/TowerTop.cpp
    Sources/Game/Game.h
    Sources/Game/Game.cpp
)

target_include_directories(
    ${PROJECT_NAME} PUBLIC 
    Sources/
    Sources/Engine/Vendor/Physac/src/
    Sources/Engine/Vendor/Box2D/include/
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}_${CMAKE_BUILD_TYPE}")
target_link_libraries(${PROJECT_NAME} raylib box2d)
configure_file("${PROJECT_SOURCE_DIR}/Sources/Config.h.in" "${PROJECT_SOURCE_DIR}/Sources/Config.h")