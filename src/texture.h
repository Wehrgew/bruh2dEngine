#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

typedef struct {
    GLuint id;
    int width;
    int height;
    int channels;
} Texture;

Texture load_texture(const char* path);
void bind_texture(const Texture* texture, GLuint unit); 

#endif