#ifndef DEMO_SCENE_H
#define DEMO_SCENE_H

#include "camera2d.h"
#include "engine.h"
#include "sprite_renderer.h"
#include "texture.h"
#include <glad/glad.h>
#include <stdbool.h>

typedef struct {
    GLuint shader_program;
    Texture test_texture;
    Texture monke_texture;
    Texture yolter_texture;
    SpriteRenderer* renderer;
    Camera2D camera;
} DemoScene;

bool demo_scene_init(DemoScene* scene);
void demo_scene_update(DemoScene* scene, Engine* engine, float delta_time);
void demo_scene_render(DemoScene* scene, Engine* engine);
void demo_scene_shutdown(DemoScene* scene);

#endif
