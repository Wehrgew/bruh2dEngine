#include "demo_scene.h"
#include "engine.h"

#include <string.h>

static const char* WINDOW_TITLE = "Test window";

static bool app_init(Engine* engine, void* user_data)
{
    (void)engine;
    return demo_scene_init((DemoScene*)user_data);
}

static void app_update(Engine* engine, float delta_time, void* user_data)
{
    demo_scene_update((DemoScene*)user_data, engine, delta_time);
}

static void app_render(Engine* engine, void* user_data)
{
    demo_scene_render((DemoScene*)user_data, engine);
}

static void app_shutdown(Engine* engine, void* user_data)
{
    (void)engine;
    demo_scene_shutdown((DemoScene*)user_data);
}

int main(void)
{
    DemoScene scene;
    memset(&scene, 0, sizeof(scene));

    EngineConfig config;
    config.window_width = 800;
    config.window_height = 600;
    config.window_title = WINDOW_TITLE;
    config.gl_major = 3;
    config.gl_minor = 3;
    config.vsync = 1;

    EngineCallbacks callbacks;
    callbacks.on_init = app_init;
    callbacks.on_update = app_update;
    callbacks.on_render = app_render;
    callbacks.on_shutdown = app_shutdown;

    return engine_run(&config, &callbacks, &scene);
}
