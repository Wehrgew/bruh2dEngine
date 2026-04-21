#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

// Основная функция, которую мы будем использовать
GLuint create_shader_program(const char* vertexPath, const char* fragmentPath);

#endif // SHADER_H