cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(game VERSION 1.0)

set(C_FLAGS "-Wall -g")
set(CMAKE_BUILD_TYPE Debug)
add_executable(game glad.c main.c gfx/shader.c src/chip8.c)

find_package(glfw3 3.4 REQUIRED)
find_package(OpenGL REQUIRED)

target_include_directories(game PRIVATE "./include/")
target_include_directories(game PUBLIC ${OPENGL_INCLUDE_DIR})
target_link_libraries(game "-framework Cocoa")
target_link_libraries(game "-framework OpenGL")
target_link_libraries(game "-framework IOKit")
target_link_libraries(game glfw ${OPENGL_gl_LIBRARY})
