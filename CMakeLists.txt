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
    src/common/BezierPatch.cpp
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

# Core-profile helpers used by the Part 3 programs (OpenGL 4.1 / GLSL 4.10).
add_library(core_gl STATIC
    src/common/Mat4.cpp
    src/common/ViewSetup.cpp
    src/common/ShaderProgram.cpp
    src/common/GpuMesh.cpp
)

target_link_libraries(core_gl PUBLIC common)

# Shaders and models are read at run time from the source tree.
target_compile_definitions(core_gl PRIVATE
    ASSIGNMENT4_SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders/"
    ASSIGNMENT4_MODEL_DIR="${CMAKE_CURRENT_SOURCE_DIR}/models/"
)

set(PART3_2D_SHADERS shaders/part3_2d.vert shaders/part3_2d.frag)
set_source_files_properties(${PART3_2D_SHADERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_executable(part3_2d
    src/part3_2d/main.cpp
    src/part3_2d/BezierMesh.cpp
    src/part3_2d/Texture2D.cpp
    src/part3_2d/Renderer.cpp
    src/part3_2d/Input.cpp
    ${PART3_2D_SHADERS}
)

target_include_directories(part3_2d PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/part3_2d
)

target_link_libraries(part3_2d PRIVATE core_gl)

set(PART3_3D_SHADERS shaders/part3_3d.vert shaders/part3_3d.frag)
set_source_files_properties(${PART3_3D_SHADERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_executable(part3_3d
    src/part3_3d/main.cpp
    src/part3_3d/SmfLoader.cpp
    src/part3_3d/Renderer.cpp
    src/part3_3d/Input.cpp
    ${PART3_3D_SHADERS}
)

target_include_directories(part3_3d PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/part3_3d
)

target_link_libraries(part3_3d PRIVATE core_gl)
