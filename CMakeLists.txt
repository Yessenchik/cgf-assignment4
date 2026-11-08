
cmake_minimum_required(VERSION 3.16)

project(Assignment4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT APPLE)
    message(FATAL_ERROR "This project is configured for macOS.")
endif()

find_library(OPENGL_FRAMEWORK OpenGL REQUIRED)
find_library(GLUT_FRAMEWORK GLUT REQUIRED)

add_executable(part1 src/part1.cpp)

target_link_libraries(part1 PRIVATE
    ${OPENGL_FRAMEWORK}
    ${GLUT_FRAMEWORK}
)

target_compile_definitions(part1 PRIVATE
    GL_SILENCE_DEPRECATION
)

