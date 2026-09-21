cmake_minimum_required(VERSION 3.16)

project(PathfindingVisualizer
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)

FetchContent_MakeAvailable(raylib)

add_executable(PathfindingVisualizer
    src/main.cpp
)

target_link_libraries(PathfindingVisualizer PRIVATE raylib)

if(MSVC)
    target_compile_options(PathfindingVisualizer PRIVATE /W4)
else()
    target_compile_options(PathfindingVisualizer PRIVATE -Wall -Wextra -Wpedantic)
endif()