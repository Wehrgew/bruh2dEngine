#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>

char* load_shader_source(const char* path)
{
    FILE* file = fopen(path, "rb");
    if (!file) {
        printf("Failed to open shader: %s\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* source = (char*)malloc(length + 1);
    if (!source) {
        fclose(file);
        return NULL;
    }

    fread(source, 1, length, file);
    source[length] = '\0';
    fclose(file);
    return source;
}

void check_shader_compile(GLuint shader, const char* type)
{
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        printf("ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s\n", type, infoLog);
    }
}

void check_program_link(GLuint program)
{
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        printf("ERROR::PROGRAM_LINKING_ERROR\n%s\n", infoLog);
    }
} 


GLuint create_shader_program(const char* vertexPath, const char* fragmentPath)
{
    char* vertexSrc   = load_shader_source(vertexPath);
    char* fragmentSrc = load_shader_source(fragmentPath);

    if (!vertexSrc || !fragmentSrc) {
        free(vertexSrc);
        free(fragmentSrc);
        return 0;
    }

    // Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const char**)&vertexSrc, NULL);
    glCompileShader(vertexShader);
    check_shader_compile(vertexShader, "VERTEX");

    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const char**)&fragmentSrc, NULL);
    glCompileShader(fragmentShader);
    check_shader_compile(fragmentShader, "FRAGMENT");

    // Program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    check_program_link(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    free(vertexSrc);
    free(fragmentSrc);

    return program;
}