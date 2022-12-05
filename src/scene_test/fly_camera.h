#pragma once

#include "framework64/camera.h"
#include "framework64/input.h"

typedef struct {
    fw64Input* input;
    fw64Camera camera;
    Vec3 rotation;
    float movement_speed;
    float turn_speed;
    int player_index;
} fw64FlyCamera;


void fw64_fly_camera_init(fw64FlyCamera* fly_cam, fw64Input* input);
void fw64_fly_camera_update(fw64FlyCamera* fly_cam, float time_delta);
