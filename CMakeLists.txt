cmake_minimum_required(VERSION 3.5)

project(TokiTori LANGUAGES CXX)
include_directories(
    ${PROJECT_SOURCE_DIR}/3rdParty/SFML/include
    ${PROJECT_SOURCE_DIR}/3rdParty/pugixml/include
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/source
)

link_directories(${PROJECT_SOURCE_DIR}/3rdParty/SFML/lib)
link_libraries(sfml-audio sfml-graphics sfml-network sfml-system sfml-window)

link_directories(${PROJECT_SOURCE_DIR}/3rdParty/pugixml/lib)
link_libraries(pugixml)


file(GLOB all_SRCS
    "${PROJECT_SOURCE_DIR}/include/*.h"
    "${PROJECT_SOURCE_DIR}/source/*.cpp"
)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(TokiTori ${all_SRCS})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                                          COMMAND ${CMAKE_COMMAND} -E copy_directory
                                          ${CMAKE_SOURCE_DIR}/Resources $<TARGET_FILE_DIR:${PROJECT_NAME}>)
