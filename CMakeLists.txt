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

# Core library (graphics-independent) for reuse in tests
add_library(PathfindingCore STATIC
    src/Grid/GridModel.cpp
    src/Algorithms/BFS.cpp
    src/Algorithms/DFS.cpp
    src/Algorithms/Dijkstra.cpp
    src/Algorithms/AStar.cpp
)

target_include_directories(PathfindingCore PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/Core
    ${CMAKE_SOURCE_DIR}/include/Grid
    ${CMAKE_SOURCE_DIR}/include/Algorithms
)

if(MSVC)
    target_compile_options(PathfindingCore PRIVATE /W4)
else()
    target_compile_options(PathfindingCore PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Main executable
add_executable(PathfindingVisualizer
    src/main.cpp
)

target_include_directories(PathfindingVisualizer PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(PathfindingVisualizer PRIVATE PathfindingCore raylib)

if(MSVC)
    target_compile_options(PathfindingVisualizer PRIVATE /W4)
else()
    target_compile_options(PathfindingVisualizer PRIVATE -Wall -Wextra -Wpedantic)
endif()