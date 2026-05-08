#ifndef CAMERA2D_H
#define CAMERA2D_H

#include <cglm.h>

typedef struct {
    vec2 position;
    float zoom;
} Camera2D;

void camera2d_init(Camera2D* camera, float x, float y, float zoom);
void camera2d_get_view_matrix(const Camera2D* camera, mat4 out_view);
void camera2d_smooth_follow(Camera2D* camera,
                            vec2 target_position,
                            float target_zoom,
                            float smoothness,
                            float delta_time);

#endif
