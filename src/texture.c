#define STB_IMAGE_IMPLEMENTATION

#include "texture.h"
#include <stb_image.h>
#include <stdio.h>



Texture load_texture(const char* path)
{
    Texture tex = {0};

    stbi_set_flip_vertically_on_load(1);

    unsigned char* data = stbi_load(path, &tex.width, &tex.height, &tex.channels, 0);
    if (!data) {
        printf("Ошибка загрузки текстуры: %s\n", path);
        return tex;
    }

    glGenTextures(1, &tex.id);
    glBindTexture(GL_TEXTURE_2D, tex.id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = (tex.channels == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, tex.width, tex.height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    printf("Texture loaded: %s (%dx%d, %d channels)\n", path, tex.width, tex.height, tex.channels);
    return tex;
}

void bind_texture(const Texture* texture, GLuint unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture->id);
}