#include "camera2d.h"

static float clampf(float value, float min_value, float max_value)
{
    if (value < min_value) return min_value;
    if (value > max_value) return max_value;
    return value;
}

void camera2d_init(Camera2D* camera, float x, float y, float zoom)
{
    if (!camera) {
        return;
    }

    camera->position[0] = x;
    camera->position[1] = y;
    camera->zoom = (zoom > 0.001f) ? zoom : 1.0f;
}

void camera2d_get_view_matrix(const Camera2D* camera, mat4 out_view)
{
    glm_mat4_identity(out_view);
    if (!camera) {
        return;
    }

    glm_scale(out_view, (vec3){camera->zoom, camera->zoom, 1.0f});
    glm_translate(out_view, (vec3){-camera->position[0], -camera->position[1], 0.0f});
}

void camera2d_smooth_follow(Camera2D* camera,
                            vec2 target_position,
                            float target_zoom,
                            float smoothness,
                            float delta_time)
{
    if (!camera) {
        return;
    }

    float t = clampf(smoothness * delta_time, 0.0f, 1.0f);

    camera->position[0] = glm_lerp(camera->position[0], target_position[0], t);
    camera->position[1] = glm_lerp(camera->position[1], target_position[1], t);

    target_zoom = (target_zoom > 0.001f) ? target_zoom : 0.001f;
    camera->zoom = glm_lerp(camera->zoom, target_zoom, t);
}
