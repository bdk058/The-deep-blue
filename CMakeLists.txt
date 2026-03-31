cmake_minimum_required(VERSION 3.10)
project(DeepSea)

set(CMAKE_CXX_STANDARD 17)

add_executable(main
    src/main.cpp
    extern/glad/src/glad.c
)

target_include_directories(main PRIVATE
    ${CMAKE_SOURCE_DIR}/extern/glad/include
    ${CMAKE_SOURCE_DIR}/extern/glfw/include
    ${CMAKE_SOURCE_DIR}/extern/glm
)

target_link_libraries(main
    ${CMAKE_SOURCE_DIR}/extern/glfw/lib-mingw-w64/libglfw3.a
    opengl32
    gdi32
)