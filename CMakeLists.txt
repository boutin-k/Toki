cmake_minimum_required(VERSION 3.5)

project(TokiTori LANGUAGES CXX)
include_directories(
    ${PROJECT_SOURCE_DIR}/../SFML-lib-2.5.1/include
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/source
)

link_directories(${PROJECT_SOURCE_DIR}/../SFML-lib-2.5.1/lib)
link_libraries(sfml-audio sfml-graphics sfml-network sfml-system sfml-window)

file(GLOB all_SRCS
    "${PROJECT_SOURCE_DIR}/include/*.h"
    "${PROJECT_SOURCE_DIR}/include/tools/*.h
    "${PROJECT_SOURCE_DIR}/source/*.cpp"
    "${PROJECT_SOURCE_DIR}/source/tools/*.cpp
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(TokiTori ${all_SRCS})
