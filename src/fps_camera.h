#pragma once

/** \file fps_camera.h */

#include "framework64/camera.h"
#include "framework64/input.h"
#include "framework64/vec2.h"
#include "mapped_input.h"

typedef struct {
    fw64Camera camera;
    float movement_speed;
    Vec2 turn_speed;
    int player_index;
    InputMapping* input_map;
    Vec2 rotation;
} fw64FpsCamera;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_fps_camera_init(fw64FpsCamera *fps, InputMapping* input_map);
void fw64_fps_camera_update(fw64FpsCamera *fps, float time_delta);

#ifdef __cplusplus
}
#endif
