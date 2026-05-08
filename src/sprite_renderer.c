#include "sprite_renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

enum {
    SPRITE_BATCH_CAPACITY = 1000,
    SPRITE_FLOATS_PER_VERTEX = 9,  // x,y,u,v,r,g,b,a,texIndex
    SPRITE_VERTICES_PER_QUAD = 4,
    SPRITE_INDICES_PER_QUAD = 6,
    SPRITE_TEXTURE_SLOT_CAPACITY = 8
};

static void sprite_renderer_upload_camera_matrices(SpriteRenderer* renderer)
{
    mat4 projection = GLM_MAT4_IDENTITY_INIT;
    mat4 view = GLM_MAT4_IDENTITY_INIT;

    glm_ortho(0.0f,
              (float)renderer->viewport_width,
              0.0f,
              (float)renderer->viewport_height,
              -1.0f,
              1.0f,
              projection);
    camera2d_get_view_matrix(&renderer->camera, view);

    glUniformMatrix4fv(renderer->loc_projection, 1, GL_FALSE, (const float*)projection);
    glUniformMatrix4fv(renderer->loc_view, 1, GL_FALSE, (const float*)view);
}

static int sprite_renderer_find_texture_slot(SpriteRenderer* renderer, GLuint texture_id)
{
    for (size_t i = 0; i < renderer->texture_slot_count; ++i) {
        if (renderer->texture_slots[i] == texture_id) {
            return (int)i;
        }
    }
    return -1;
}

SpriteRenderer* sprite_renderer_create(GLuint shaderProgram)
{
    SpriteRenderer* sr = (SpriteRenderer*)malloc(sizeof(SpriteRenderer));
    if (!sr) return NULL;
    memset(sr, 0, sizeof(*sr));

    sr->shaderProgram = shaderProgram;

    // Получаем локации uniform'ов
    sr->loc_model   = glGetUniformLocation(shaderProgram, "model");
    sr->loc_color   = glGetUniformLocation(shaderProgram, "spriteColor");
    sr->loc_texture = glGetUniformLocation(shaderProgram, "uTextures");
    sr->loc_projection = glGetUniformLocation(shaderProgram, "projection");
    sr->loc_view = glGetUniformLocation(shaderProgram, "view");
    sr->viewport_width = 1;
    sr->viewport_height = 1;
    camera2d_init(&sr->camera, 0.0f, 0.0f, 1.0f);

    sr->batch_capacity = SPRITE_BATCH_CAPACITY;
    sr->batch_vertices = (float*)malloc(sr->batch_capacity *
                                        SPRITE_VERTICES_PER_QUAD *
                                        SPRITE_FLOATS_PER_VERTEX *
                                        sizeof(float));
    sr->batch_indices = (unsigned int*)malloc(sr->batch_capacity *
                                               SPRITE_INDICES_PER_QUAD *
                                               sizeof(unsigned int));
    if (!sr->batch_vertices || !sr->batch_indices) {
        free(sr->batch_vertices);
        free(sr->batch_indices);
        free(sr);
        return NULL;
    }

    printf("Uniform locations: model=%d, spriteColor=%d, ourTexture=%d, projection=%d, view=%d\n",
           sr->loc_model, sr->loc_color, sr->loc_texture, sr->loc_projection, sr->loc_view);

    // Создаём VAO/VBO/EBO
    glGenVertexArrays(1, &sr->VAO);
    glGenBuffers(1, &sr->VBO);
    glGenBuffers(1, &sr->EBO);

    glBindVertexArray(sr->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, sr->VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 sr->batch_capacity * SPRITE_VERTICES_PER_QUAD * SPRITE_FLOATS_PER_VERTEX * sizeof(float),
                 NULL,
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sr->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sr->batch_capacity * SPRITE_INDICES_PER_QUAD * sizeof(unsigned int),
                 NULL,
                 GL_DYNAMIC_DRAW);

    // Position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, SPRITE_FLOATS_PER_VERTEX * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, SPRITE_FLOATS_PER_VERTEX * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Color
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, SPRITE_FLOATS_PER_VERTEX * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, SPRITE_FLOATS_PER_VERTEX * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);

    printf("SpriteRenderer created successfully!\n");
    return sr;
}

void sprite_renderer_set_viewport_size(SpriteRenderer* renderer, int width, int height)
{
    if (!renderer) {
        return;
    }
    renderer->viewport_width = (width > 0) ? width : 1;
    renderer->viewport_height = (height > 0) ? height : 1;
}

void sprite_renderer_set_camera(SpriteRenderer* renderer, const Camera2D* camera)
{
    if (!renderer || !camera) {
        return;
    }
    renderer->camera = *camera;
}

void sprite_renderer_begin_batch(SpriteRenderer* renderer)
{
    if (!renderer) {
        return;
    }
    renderer->batch_count = 0;
    renderer->texture_slot_count = 0;
}

void sprite_renderer_submit(SpriteRenderer* renderer,
                            const Texture* texture,
                            vec2 position,
                            vec2 size,
                            float rotation,
                            vec4 color)
{
    if (!renderer || !texture || texture->id == 0) {
        return;
    }
    if (renderer->batch_count >= renderer->batch_capacity) {
        sprite_renderer_flush(renderer);
    }

    int texture_slot_index = sprite_renderer_find_texture_slot(renderer, texture->id);
    if (texture_slot_index < 0) {
        if (renderer->texture_slot_count >= SPRITE_TEXTURE_SLOT_CAPACITY) {
            sprite_renderer_flush(renderer);
        }
        texture_slot_index = sprite_renderer_find_texture_slot(renderer, texture->id);
        if (texture_slot_index < 0) {
            renderer->texture_slots[renderer->texture_slot_count] = texture->id;
            texture_slot_index = (int)renderer->texture_slot_count;
            renderer->texture_slot_count += 1;
        }
    }

    const float hx = size[0] * 0.5f;
    const float hy = size[1] * 0.5f;
    const float cx = position[0] + hx;
    const float cy = position[1] + hy;
    const float rad = glm_rad(rotation);
    const float c = cosf(rad);
    const float s = sinf(rad);

    const float local[4][2] = {
        {-hx, -hy},
        { hx, -hy},
        { hx,  hy},
        {-hx,  hy}
    };
    const float uvs[4][2] = {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f}
    };

    const size_t sprite_index = renderer->batch_count;
    float* vtx = renderer->batch_vertices +
                 sprite_index * SPRITE_VERTICES_PER_QUAD * SPRITE_FLOATS_PER_VERTEX;
    unsigned int* idx = renderer->batch_indices + sprite_index * SPRITE_INDICES_PER_QUAD;
    const unsigned int base = (unsigned int)(sprite_index * SPRITE_VERTICES_PER_QUAD);

    for (size_t i = 0; i < 4; ++i) {
        const float lx = local[i][0];
        const float ly = local[i][1];
        const float rx = lx * c - ly * s;
        const float ry = lx * s + ly * c;
        const size_t off = i * SPRITE_FLOATS_PER_VERTEX;

        vtx[off + 0] = cx + rx;
        vtx[off + 1] = cy + ry;
        vtx[off + 2] = uvs[i][0];
        vtx[off + 3] = uvs[i][1];
        vtx[off + 4] = color[0];
        vtx[off + 5] = color[1];
        vtx[off + 6] = color[2];
        vtx[off + 7] = color[3];
        vtx[off + 8] = (float)texture_slot_index;
    }

    idx[0] = base + 0;
    idx[1] = base + 1;
    idx[2] = base + 2;
    idx[3] = base + 2;
    idx[4] = base + 3;
    idx[5] = base + 0;

    renderer->batch_count += 1;
    renderer->stats_submitted_sprites += 1;
}

void sprite_renderer_flush(SpriteRenderer* renderer)
{
    if (!renderer || renderer->batch_count == 0 || renderer->texture_slot_count == 0) {
        return;
    }

    glUseProgram(renderer->shaderProgram);
    sprite_renderer_upload_camera_matrices(renderer);

    int samplers[SPRITE_TEXTURE_SLOT_CAPACITY] = {0, 1, 2, 3, 4, 5, 6, 7};
    for (size_t i = 0; i < renderer->texture_slot_count; ++i) {
        glActiveTexture(GL_TEXTURE0 + (GLenum)i);
        glBindTexture(GL_TEXTURE_2D, renderer->texture_slots[i]);
    }
    glUniform1iv(renderer->loc_texture, SPRITE_TEXTURE_SLOT_CAPACITY, samplers);
    glUniform4f(renderer->loc_color, 1.0f, 1.0f, 1.0f, 1.0f);

    glBindVertexArray(renderer->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->VBO);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    renderer->batch_count * SPRITE_VERTICES_PER_QUAD * SPRITE_FLOATS_PER_VERTEX * sizeof(float),
                    renderer->batch_vertices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->EBO);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                    0,
                    renderer->batch_count * SPRITE_INDICES_PER_QUAD * sizeof(unsigned int),
                    renderer->batch_indices);

    glDrawElements(GL_TRIANGLES,
                   (GLsizei)(renderer->batch_count * SPRITE_INDICES_PER_QUAD),
                   GL_UNSIGNED_INT,
                   0);
    renderer->stats_draw_calls += 1;
    renderer->stats_flushes += 1;

    renderer->batch_count = 0;
    renderer->texture_slot_count = 0;
}

void sprite_renderer_reset_stats(SpriteRenderer* renderer)
{
    if (!renderer) {
        return;
    }
    renderer->stats_draw_calls = 0;
    renderer->stats_flushes = 0;
    renderer->stats_submitted_sprites = 0;
}

unsigned int sprite_renderer_get_draw_calls(const SpriteRenderer* renderer)
{
    if (!renderer) {
        return 0;
    }
    return renderer->stats_draw_calls;
}

unsigned int sprite_renderer_get_flushes(const SpriteRenderer* renderer)
{
    if (!renderer) {
        return 0;
    }
    return renderer->stats_flushes;
}

unsigned int sprite_renderer_get_submitted_sprites(const SpriteRenderer* renderer)
{
    if (!renderer) {
        return 0;
    }
    return renderer->stats_submitted_sprites;
}

void sprite_renderer_destroy(SpriteRenderer* renderer)
{
    if (renderer) {
        free(renderer->batch_vertices);
        free(renderer->batch_indices);
        glDeleteVertexArrays(1, &renderer->VAO);
        glDeleteBuffers(1, &renderer->VBO);
        glDeleteBuffers(1, &renderer->EBO);
        free(renderer);
    }
}

void sprite_renderer_draw(SpriteRenderer* renderer,
                          Texture* texture,
                          vec2 position,
                          vec2 size,
                          float rotation,
                          vec4 color)
{
    if (!renderer || !texture || renderer->VAO == 0)
        return;
    sprite_renderer_begin_batch(renderer);
    sprite_renderer_submit(renderer, texture, position, size, rotation, color);
    sprite_renderer_flush(renderer);
}
