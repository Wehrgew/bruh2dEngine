#include "demo_scene.h"

#include "shader.h"
#include <math.h>
#include <stdio.h>

static const char* WINDOW_TITLE = "Test window";
static const char* VERTEX_SHADER_PATH = "shaders/vertex.glsl";
static const char* FRAGMENT_SHADER_PATH = "shaders/fragment.glsl";
static const char* TEST_TEXTURE_PATH = "assets/test.png";
static const char* MONKE_TEXTURE_PATH = "assets/monke.png";
static const char* YOLTER_TEXTURE_PATH = "assets/yolter.png";
static const float SPRITE_WINDOW_SCALE = 0.35f;
static const float CAMERA_SMOOTHNESS = 3.0f;

static void compute_sprite_size(const Texture* texture, float base_size, vec2 out_size)
{
    float aspect = 1.0f;
    if (texture->height > 0) {
        aspect = (float)texture->width / (float)texture->height;
    }

    out_size[0] = base_size;
    out_size[1] = base_size;
    if (aspect >= 1.0f) {
        out_size[1] = base_size / aspect;
    } else {
        out_size[0] = base_size * aspect;
    }
}

bool demo_scene_init(DemoScene* scene)
{
    if (!scene) {
        return false;
    }

    scene->shader_program = create_shader_program(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
    if (scene->shader_program == 0) {
        printf("Failed to create shader program\n");
        return false;
    }

    scene->test_texture = load_texture(TEST_TEXTURE_PATH);
    scene->monke_texture = load_texture(MONKE_TEXTURE_PATH);
    scene->yolter_texture = load_texture(YOLTER_TEXTURE_PATH);
    if (scene->test_texture.id == 0 || scene->monke_texture.id == 0 || scene->yolter_texture.id == 0) {
        printf("Warning: one or more textures failed to load\n");
    }

    scene->renderer = sprite_renderer_create(scene->shader_program);
    if (!scene->renderer) {
        printf("Failed to create SpriteRenderer\n");
        glDeleteProgram(scene->shader_program);
        scene->shader_program = 0;
        return false;
    }

    camera2d_init(&scene->camera, 0.0f, 0.0f, 1.0f);
    return true;
}

void demo_scene_update(DemoScene* scene, Engine* engine, float delta_time)
{
    (void)delta_time;
    if (!scene || !engine || !scene->renderer) {
        return;
    }

    int framebuffer_width = 0;
    int framebuffer_height = 0;
    engine_get_framebuffer_size(engine, &framebuffer_width, &framebuffer_height);
    sprite_renderer_set_viewport_size(scene->renderer, framebuffer_width, framebuffer_height);

    float current_time = engine_get_time(engine);
    vec2 target_camera_pos = {
        20.0f * sinf(current_time * 0.8f),
        14.0f * cosf(current_time * 0.7f)
    };
    float target_zoom = 1.0f + 0.12f * sinf(current_time * 0.55f);
    camera2d_smooth_follow(&scene->camera,
                           target_camera_pos,
                           target_zoom,
                           CAMERA_SMOOTHNESS,
                           delta_time);
    sprite_renderer_set_camera(scene->renderer, &scene->camera);
}

void demo_scene_render(DemoScene* scene, Engine* engine)
{
    if (!scene || !engine || !scene->renderer) {
        return;
    }

    glClearColor(0.12f, 0.15f, 0.28f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (scene->test_texture.id == 0 || scene->monke_texture.id == 0 || scene->yolter_texture.id == 0) {
        return;
    }

    int framebuffer_width = 0;
    int framebuffer_height = 0;
    engine_get_framebuffer_size(engine, &framebuffer_width, &framebuffer_height);

    sprite_renderer_reset_stats(scene->renderer);

    float base_size = (framebuffer_width < framebuffer_height)
        ? (float)framebuffer_width
        : (float)framebuffer_height;
    base_size *= SPRITE_WINDOW_SCALE;

    vec2 size_center = {0.0f, 0.0f};
    vec2 size_left = {0.0f, 0.0f};
    vec2 size_right = {0.0f, 0.0f};
    compute_sprite_size(&scene->test_texture, base_size, size_center);
    compute_sprite_size(&scene->monke_texture, base_size * 0.8f, size_left);
    compute_sprite_size(&scene->yolter_texture, base_size * 0.8f, size_right);

    vec2 center_pos = {
        ((float)framebuffer_width - size_center[0]) * 0.5f,
        ((float)framebuffer_height - size_center[1]) * 0.5f
    };
    vec2 left_pos = {
        center_pos[0] - size_center[0] * 0.75f,
        center_pos[1] + size_center[1] * 0.05f
    };
    vec2 right_pos = {
        center_pos[0] + size_center[0] * 0.75f,
        center_pos[1] - size_center[1] * 0.05f
    };

    float current_time = engine_get_time(engine);
    float rotation_center = 8.0f * sinf(current_time * 0.6f);
    float rotation_left = -10.0f * sinf(current_time * 0.9f);
    float rotation_right = 10.0f * cosf(current_time * 0.9f);
    vec4 color_center = {1.0f, 1.0f, 1.0f, 1.0f};
    vec4 color_left = {0.95f, 1.0f, 0.95f, 1.0f};
    vec4 color_right = {0.95f, 0.95f, 1.0f, 1.0f};

    sprite_renderer_begin_batch(scene->renderer);
    sprite_renderer_submit(scene->renderer, &scene->monke_texture, left_pos, size_left, rotation_left, color_left);
    sprite_renderer_submit(scene->renderer, &scene->test_texture, center_pos, size_center, rotation_center, color_center);
    sprite_renderer_submit(scene->renderer, &scene->yolter_texture, right_pos, size_right, rotation_right, color_right);
    sprite_renderer_flush(scene->renderer);

    char title[256];
    snprintf(title,
             sizeof(title),
             "%s | sprites=%u draw_calls=%u flushes=%u",
             WINDOW_TITLE,
             sprite_renderer_get_submitted_sprites(scene->renderer),
             sprite_renderer_get_draw_calls(scene->renderer),
             sprite_renderer_get_flushes(scene->renderer));
    engine_set_window_title(engine, title);
}

void demo_scene_shutdown(DemoScene* scene)
{
    if (!scene) {
        return;
    }

    sprite_renderer_destroy(scene->renderer);
    scene->renderer = NULL;

    if (scene->shader_program != 0) {
        glDeleteProgram(scene->shader_program);
        scene->shader_program = 0;
    }
    glDeleteTextures(1, &scene->test_texture.id);
    glDeleteTextures(1, &scene->monke_texture.id);
    glDeleteTextures(1, &scene->yolter_texture.id);
}
