#pragma once

/** \file fps_camera.h */

#include "framework64/camera.h"
#include "framework64/input.h"
#include "framework64/vec2.h"

#include "mapped_input.h"
#include "framework64/level.h"

typedef struct {
    fw64Level* level;
    fw64Collider* collider;
    InputMapping* input_map;
    fw64Camera camera;
    float movement_speed;
    Vec2 turn_speed;
    int player_index;
    fw64Input* _input;
    Vec2 rotation;
    float height;
    uint32_t collision_mask;
} MovementController;

#ifdef __cplusplus
extern "C" {
#endif

void movement_controller_init(MovementController* controller, InputMapping* input_map, fw64Level* level, fw64Collider* collider);
void movement_controller_update(MovementController* controller, float time_delta);

#ifdef __cplusplus
}
#endif
