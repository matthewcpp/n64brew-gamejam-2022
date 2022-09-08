#include "movement_controller.h"

#include "framework64/n64/controller_button.h"

#include "assets/layers.h"

#define DEFAULT_MOVEMENT_SPEED 8.0f
#define DEFAULT_TURN_SPEED 90.0f
#define STICK_THRESHOLD 0.15

static void movement_controller_get_ground_height(MovementController* controller);

void movement_controller_init(MovementController* controller, fw64Input* input, fw64Level* level, fw64Collider* collider){
    controller->_input = input;
    controller->level = level;
    controller->collider = collider;

    controller->movement_speed = DEFAULT_MOVEMENT_SPEED;
    controller->turn_speed = DEFAULT_TURN_SPEED;

    controller->player_index = 0;
    controller->height = 5;

    controller->collision_mask = FW64_layer_obstacles | FW64_layer_wall;

    fw64_camera_init(&controller->camera);
    vec2_set(&controller->rotation, 0.0f, 0.0f);
}

static void controller_cam_forward(MovementController* controller, Vec3* out) {
    Quat q;
    quat_from_euler(&q, 0.0f, controller->rotation.y, 0.0f);

    Vec3 forward = { 0.0f, 0.0f, -1.0f };
    quat_transform_vec3(out, &q, &forward);
}

static void controller_cam_back(MovementController* controller, Vec3* out) {
    controller_cam_forward(controller, out);
    vec3_negate(out);
}

static void controller_cam_right(MovementController* controller, Vec3* out) {
    Quat q;
    quat_from_euler(&q, 0.0f, controller->rotation.y, 0.0f);

    Vec3 right = { 1.0f, 0.0f, 0.0f };
    quat_transform_vec3(out, &q, &right);
}

static void controller_cam_left(MovementController* controller, Vec3* out) {
    controller_cam_right(controller, out);
    vec3_negate(out);
}

static void move_camera(MovementController* controller, float time_delta, Vec2* stick) {
    int did_move = 0;
    Vec3 move = {0.0f, 0.0f, 0.0f};

    if (fw64_input_controller_button_down(controller->_input, controller->player_index, FW64_N64_CONTROLLER_BUTTON_C_RIGHT)) {
        controller_cam_right(controller, &move);
        vec3_scale(&move, &move, controller->movement_speed * time_delta);
        did_move = 1;
    }

    if (fw64_input_controller_button_down(controller->_input, controller->player_index, FW64_N64_CONTROLLER_BUTTON_C_LEFT)) {
        controller_cam_left(controller, &move);
        vec3_scale(&move, &move, controller->movement_speed * time_delta);
        did_move = 1;
    }

    if (stick->y > STICK_THRESHOLD) {
        controller_cam_forward(controller, &move);
        vec3_scale(&move, &move, controller->movement_speed * time_delta * stick->y);
        did_move = 1;
    }

    if (stick->y < -STICK_THRESHOLD) {
        controller_cam_back(controller, &move);
        vec3_scale(&move, &move, controller->movement_speed * time_delta * -stick->y);
        did_move = 1;
    }

    if (!did_move)
        return;

    fw64IntersectMovingBoxQuery query;
    if (fw64_level_moving_box_intersection(controller->level, &controller->collider->bounding, &move, controller->collision_mask, &query)) {
        vec3_scale(&move, &move, query.results[0].tfirst - 0.01f);
    }

    vec3_add(&controller->camera.transform.position, &controller->camera.transform.position, &move);
}

static void tilt_camera(MovementController* controller, float time_delta, Vec2* stick) {
    if (stick->x > STICK_THRESHOLD) {
        controller->rotation.y -= controller->turn_speed * time_delta;
    }

    if (stick->x < -STICK_THRESHOLD) {
        controller->rotation.y += controller->turn_speed * time_delta;
    }

    if (fw64_input_controller_button_down(controller->_input, controller->player_index, FW64_N64_CONTROLLER_BUTTON_C_UP)) {
        controller->rotation.x += controller->turn_speed * time_delta;

        if (controller->rotation.x > 90.0f)
            controller->rotation.x = 90.0f;
    }

    if (fw64_input_controller_button_down(controller->_input, controller->player_index, FW64_N64_CONTROLLER_BUTTON_C_DOWN)) {
        controller->rotation.x -= controller->turn_speed * time_delta;

        if (controller->rotation.x < -90.0f)
            controller->rotation.x = -90.0f;
    }
}

void movement_controller_update(MovementController* controller, float time_delta) {
    Vec2 stick;
    fw64_input_controller_stick(controller->_input, controller->player_index, &stick);

    movement_controller_get_ground_height(controller);

    move_camera(controller, time_delta, &stick);
    tilt_camera(controller, time_delta, &stick);

    Quat q;
    quat_from_euler(&q, controller->rotation.x, controller->rotation.y, 0.0f);

    Vec3 forward = { 0.0f, 0.0f, -1.0f };
    Vec3 tar;
    quat_transform_vec3(&tar, &q, &forward);
    vec3_add(&tar, &controller->camera.transform.position, &tar);

    Vec3 up = {0.0f, 1.0f, 0.0f};

    fw64_transform_look_at(&controller->camera.transform, &tar, &up);
    fw64_camera_update_view_matrix(&controller->camera);
}

void movement_controller_get_ground_height(MovementController* controller) {
    fw64RaycastHit raycast_hit;

    Vec3 ray_pos = controller->camera.transform.position;
    ray_pos.y = 1000.0f;
    Vec3 ray_dir = {0.0f, -1.0f, 0.0f};

    if (fw64_level_raycast(controller->level, &ray_pos, &ray_dir, FW64_layer_ground, &raycast_hit)) {
        controller->camera.transform.position.y = raycast_hit.point.y + controller->height;
    }
}