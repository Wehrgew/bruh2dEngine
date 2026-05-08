#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "texture.h"
#include "camera2d.h"
#include <glad/glad.h>
#include <cglm.h>
#include <stddef.h>

typedef struct {
    GLuint shaderProgram;
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    GLint loc_model;
    GLint loc_color;
    GLint loc_texture;
    GLint loc_projection;
    GLint loc_view;
    int viewport_width;
    int viewport_height;
    Camera2D camera;
    float* batch_vertices;
    unsigned int* batch_indices;
    size_t batch_count;
    size_t batch_capacity;
    GLuint texture_slots[8];
    size_t texture_slot_count;
    unsigned int stats_draw_calls;
    unsigned int stats_flushes;
    unsigned int stats_submitted_sprites;
} SpriteRenderer;

SpriteRenderer* sprite_renderer_create(GLuint shaderProgram);
void sprite_renderer_destroy(SpriteRenderer* renderer);
void sprite_renderer_set_viewport_size(SpriteRenderer* renderer, int width, int height);
void sprite_renderer_set_camera(SpriteRenderer* renderer, const Camera2D* camera);
void sprite_renderer_begin_batch(SpriteRenderer* renderer);
void sprite_renderer_submit(SpriteRenderer* renderer,
                            const Texture* texture,
                            vec2 position,
                            vec2 size,
                            float rotation,
                            vec4 color);
void sprite_renderer_flush(SpriteRenderer* renderer);
void sprite_renderer_reset_stats(SpriteRenderer* renderer);
unsigned int sprite_renderer_get_draw_calls(const SpriteRenderer* renderer);
unsigned int sprite_renderer_get_flushes(const SpriteRenderer* renderer);
unsigned int sprite_renderer_get_submitted_sprites(const SpriteRenderer* renderer);

void sprite_renderer_draw(SpriteRenderer* renderer,
                          Texture* texture,
                          vec2 position,
                          vec2 size,
                          float rotation,
                          vec4 color);

#endif 
