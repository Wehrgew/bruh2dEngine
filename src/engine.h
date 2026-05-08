#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>

typedef struct GLFWwindow GLFWwindow;

typedef struct Engine Engine;

typedef struct {
    int window_width;
    int window_height;
    const char* window_title;
    int gl_major;
    int gl_minor;
    int vsync;
} EngineConfig;

typedef struct {
    bool (*on_init)(Engine* engine, void* user_data);
    void (*on_update)(Engine* engine, float delta_time, void* user_data);
    void (*on_render)(Engine* engine, void* user_data);
    void (*on_shutdown)(Engine* engine, void* user_data);
} EngineCallbacks;

int engine_run(const EngineConfig* config, const EngineCallbacks* callbacks, void* user_data);

GLFWwindow* engine_get_window(const Engine* engine);
float engine_get_time(const Engine* engine);
float engine_get_delta_time(const Engine* engine);
void engine_get_framebuffer_size(const Engine* engine, int* width, int* height);
void engine_set_window_title(Engine* engine, const char* title);
void engine_request_close(Engine* engine);

#endif
