cmake_minimum_required(VERSION 3.16)

project(Assignment4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT APPLE)
    message(FATAL_ERROR "This project targets macOS.")
endif()

find_library(OPENGL_FRAMEWORK OpenGL REQUIRED)
find_library(GLUT_FRAMEWORK GLUT REQUIRED)

add_library(common STATIC
    src/common/Vec3.cpp
    src/common/Camera.cpp
)

target_include_directories(common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
)

target_link_libraries(common PUBLIC
    ${OPENGL_FRAMEWORK}
    ${GLUT_FRAMEWORK}
)

target_compile_definitions(common PUBLIC
    GL_SILENCE_DEPRECATION
)

add_executable(part1
    src/part1/main.cpp
    src/part1/BezierPatch.cpp
    src/part1/Renderer.cpp
    src/part1/Input.cpp
)

target_include_directories(part1 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/part1
)

target_link_libraries(part1 PRIVATE common)

add_executable(part2
    src/part2/main.cpp
    src/part2/Mesh.cpp
    src/part2/Scene.cpp
    src/part2/Renderer.cpp
    src/part2/Picking.cpp
    src/part2/Input.cpp
    src/part2/SelfTest.cpp
)

target_include_directories(part2 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/part2
)

target_link_libraries(part2 PRIVATE common)
