cmake_minimum_required(VERSION 3.14)
project(My2DOpenGLEngine C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Поиск библиотек
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# ==================== БИБЛИОТЕКИ ====================

# GLAD
add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

# stb_image (header-only)
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE external/stb)

# ==================== ИСПОЛНЯЕМЫЙ ФАЙЛ ====================

add_executable(engine
    src/main.c
    src/engine.c
    src/demo_scene.c
    src/shader.c
    src/texture.c
    src/sprite_renderer.c
    src/camera2d.c
)

target_include_directories(engine PRIVATE
    external/glad/include
    external/stb
    external/cglm
)

target_link_libraries(engine PRIVATE
    glfw
    glad
    stb
    OpenGL::GL
    m
)

# ====================== КОПИРОВАНИЕ РЕСУРСОВ ======================

# Шейдеры
add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        ${CMAKE_BINARY_DIR}/shaders
    COMMENT "Copying GLSL shaders"
)

# Assets (текстуры)
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_BINARY_DIR}/assets
    COMMENT "Copying assets folder"
)

add_dependencies(engine copy_shaders copy_assets)

message(STATUS "Build directory: ${CMAKE_BINARY_DIR}")
