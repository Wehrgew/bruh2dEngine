#include "engine.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <stdio.h>
#include <string.h>

struct Engine {
    EngineConfig config;
    GLFWwindow* window;
    float current_time;
    float delta_time;
    int framebuffer_width;
    int framebuffer_height;
};

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    Engine* engine = (Engine*)glfwGetWindowUserPointer(window);
    if (engine) {
        engine->framebuffer_width = (width > 0) ? width : 1;
        engine->framebuffer_height = (height > 0) ? height : 1;
    }
    glViewport(0, 0, width, height);
}

static EngineConfig default_config(void)
{
    EngineConfig cfg;
    cfg.window_width = 800;
    cfg.window_height = 600;
    cfg.window_title = "Engine";
    cfg.gl_major = 3;
    cfg.gl_minor = 3;
    cfg.vsync = 1;
    return cfg;
}

int engine_run(const EngineConfig* config, const EngineCallbacks* callbacks, void* user_data)
{
    Engine engine;
    memset(&engine, 0, sizeof(engine));
    engine.config = default_config();
    if (config) {
        engine.config = *config;
    }

    if (!glfwInit()) {
        printf("Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, engine.config.gl_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, engine.config.gl_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    engine.window = glfwCreateWindow(engine.config.window_width,
                                     engine.config.window_height,
                                     engine.config.window_title,
                                     NULL,
                                     NULL);
    if (!engine.window) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(engine.window);
    glfwSetWindowUserPointer(engine.window, &engine);
    glfwSetFramebufferSizeCallback(engine.window, framebuffer_size_callback);
    glfwSwapInterval(engine.config.vsync ? 1 : 0);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        glfwDestroyWindow(engine.window);
        glfwTerminate();
        return -1;
    }

    glfwGetFramebufferSize(engine.window, &engine.framebuffer_width, &engine.framebuffer_height);
    if (engine.framebuffer_width <= 0) engine.framebuffer_width = 1;
    if (engine.framebuffer_height <= 0) engine.framebuffer_height = 1;
    glViewport(0, 0, engine.framebuffer_width, engine.framebuffer_height);

    if (callbacks && callbacks->on_init) {
        if (!callbacks->on_init(&engine, user_data)) {
            if (callbacks->on_shutdown) {
                callbacks->on_shutdown(&engine, user_data);
            }
            glfwDestroyWindow(engine.window);
            glfwTerminate();
            return -1;
        }
    }

    engine.current_time = (float)glfwGetTime();
    while (!glfwWindowShouldClose(engine.window)) {
        float new_time = (float)glfwGetTime();
        engine.delta_time = new_time - engine.current_time;
        engine.current_time = new_time;

        glfwPollEvents();

        if (callbacks && callbacks->on_update) {
            callbacks->on_update(&engine, engine.delta_time, user_data);
        }
        if (callbacks && callbacks->on_render) {
            callbacks->on_render(&engine, user_data);
        }

        glfwSwapBuffers(engine.window);
    }

    if (callbacks && callbacks->on_shutdown) {
        callbacks->on_shutdown(&engine, user_data);
    }

    glfwDestroyWindow(engine.window);
    glfwTerminate();
    return 0;
}

GLFWwindow* engine_get_window(const Engine* engine)
{
    return engine ? engine->window : NULL;
}

float engine_get_time(const Engine* engine)
{
    return engine ? engine->current_time : 0.0f;
}

float engine_get_delta_time(const Engine* engine)
{
    return engine ? engine->delta_time : 0.0f;
}

void engine_get_framebuffer_size(const Engine* engine, int* width, int* height)
{
    if (!engine) {
        if (width) *width = 0;
        if (height) *height = 0;
        return;
    }
    if (width) *width = engine->framebuffer_width;
    if (height) *height = engine->framebuffer_height;
}

void engine_set_window_title(Engine* engine, const char* title)
{
    if (!engine || !engine->window || !title) {
        return;
    }
    glfwSetWindowTitle(engine->window, title);
}

void engine_request_close(Engine* engine)
{
    if (!engine || !engine->window) {
        return;
    }
    glfwSetWindowShouldClose(engine->window, GLFW_TRUE);
}
