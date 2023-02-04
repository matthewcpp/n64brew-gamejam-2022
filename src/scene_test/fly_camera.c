#include "fly_camera.h"

#include "framework64/n64/controller_button.h"

#define DEFAULT_MOVEMENT_SPEED 8.0f
#define DEFAULT_TURN_SPEED 90.0f
#define STICK_THRESHOLD 0.15

static void fw64_fly_camera_tilt(fw64FlyCamera* fly_cam, float time_delta);
static void fw64_fly_camera_move(fw64FlyCamera* fly_cam, float time_delta);
static void fw64_fly_camera_dolly(fw64FlyCamera* fly_cam, float time_delta);

void fw64_fly_camera_init(fw64FlyCamera* fly_cam, fw64Input* input) {
    fly_cam->input = input;

    fw64_camera_init(&fly_cam->camera);
    vec3_zero(&fly_cam->rotation);

    fly_cam->movement_speed = DEFAULT_MOVEMENT_SPEED;
    fly_cam->turn_speed = DEFAULT_TURN_SPEED;
    fly_cam->player_index = 0;
}

void fw64_fly_camera_update(fw64FlyCamera* fly_cam, float time_delta) {
    fw64_fly_camera_tilt(fly_cam, time_delta);
    fw64_fly_camera_move(fly_cam, time_delta);
    fw64_fly_camera_dolly(fly_cam, time_delta);

    quat_from_euler(&fly_cam->camera.transform.rotation, fly_cam->rotation.x, fly_cam->rotation.y, 0.0f);
    fw64_camera_update_view_matrix(&fly_cam->camera);
}

void fw64_fly_camera_tilt(fw64FlyCamera* fly_cam, float time_delta) {
    Vec2 stick;
    fw64_input_controller_stick(fly_cam->input, fly_cam->player_index, &stick);

    if (fw64_input_controller_button_down(fly_cam->input, fly_cam->player_index, FW64_N64_CONTROLLER_BUTTON_C_UP)) {
        fly_cam->rotation.x += fly_cam->turn_speed * time_delta;
    }

    if (fw64_input_controller_button_down(fly_cam->input, fly_cam->player_index, FW64_N64_CONTROLLER_BUTTON_C_DOWN)) {
        fly_cam->rotation.x -= fly_cam->turn_speed * time_delta;
    }
}

void fw64_fly_camera_move(fw64FlyCamera* fly_cam, float time_delta) {
    Vec2 stick;
    fw64_input_controller_stick(fly_cam->input, fly_cam->player_index, &stick);
    float speed = 0.0f;

    if (stick.y > STICK_THRESHOLD) {
        speed = stick.y * fly_cam->movement_speed * time_delta;
    }

    if (stick.y < -STICK_THRESHOLD) {
        speed = stick.y * fly_cam->movement_speed * time_delta;
    }

    if (stick.x > STICK_THRESHOLD) {
        fly_cam->rotation.y -= fly_cam->turn_speed * time_delta;
    }

    if (stick.x < -STICK_THRESHOLD) {
        fly_cam->rotation.y += fly_cam->turn_speed * time_delta;
    }

    Vec3 forward;
    fw64_transform_forward(&fly_cam->camera.transform, &forward);
    vec3_add_and_scale(&fly_cam->camera.transform.position, &fly_cam->camera.transform.position, &forward, speed);
}

void fw64_fly_camera_dolly(fw64FlyCamera* fly_cam, float time_delta) {
    if (fw64_input_controller_button_down(fly_cam->input, fly_cam->player_index, FW64_N64_CONTROLLER_BUTTON_C_RIGHT)) {
        
    }

    if (fw64_input_controller_button_down(fly_cam->input, fly_cam->player_index, FW64_N64_CONTROLLER_BUTTON_C_LEFT)) {
        
    }
}
