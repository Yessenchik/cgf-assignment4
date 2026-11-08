
cmake_minimum_required(VERSION 3.16)

project(Assignment4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT APPLE)
    message(FATAL_ERROR "This project targets macOS.")
endif()

find_library(OPENGL_FRAMEWORK OpenGL REQUIRED)
find_library(GLUT_FRAMEWORK GLUT REQUIRED)

add_executable(part1
    src/main.cpp
    src/common/Vec3.cpp
    src/common/Camera.cpp
    src/part1/BezierPatch.cpp
    src/part1/Renderer.cpp
    src/part1/Input.cpp
)

target_include_directories(part1 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/part1
)

target_link_libraries(part1 PRIVATE
    ${OPENGL_FRAMEWORK}
    ${GLUT_FRAMEWORK}
)

target_compile_definitions(part1 PRIVATE
    GL_SILENCE_DEPRECATION
)
