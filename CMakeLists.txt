cmake_minimum_required(VERSION 3.14)
project(My2DOpenGLEngine C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Поиск библиотек
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# GLAD
add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

# stb_image (header-only)
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE external/stb)

# Основной исполняемый файл
add_executable(engine
    src/main.c
    src/shader.c
)

target_include_directories(engine PRIVATE
    external/glad/include
    external/stb
)

target_link_libraries(engine PRIVATE
    glfw
    glad
    stb
    OpenGL::GL
    m
)

# ====================== КОПИРОВАНИЕ ШЕЙДЕРОВ ======================
add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        ${CMAKE_BINARY_DIR}/shaders
    COMMENT "Copying GLSL shaders into build directory"
)

# Чтобы шейдеры копировались перед сборкой engine
add_dependencies(engine copy_shaders)

# Опционально: выводим сообщение, где лежит исполняемый файл
message(STATUS "Build directory: ${CMAKE_BINARY_DIR}")