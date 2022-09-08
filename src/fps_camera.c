#include "fps_camera.h"

#include "framework64/n64/controller_button.h"

#define DEFAULT_MOVEMENT_SPEED 8.0f
#define DEFAULT_X_TURN_SPEED 180.0f
#define DEFAULT_Y_TURN_SPEED 90.0f
#define STICK_THRESHOLD 0.15

void fw64_fps_camera_init(fw64FpsCamera* fps, InputMapping* input_map){
    fps->input_map = input_map;

    fps->movement_speed = DEFAULT_MOVEMENT_SPEED;
    fps->turn_speed.x = DEFAULT_X_TURN_SPEED;
    fps->turn_speed.y = DEFAULT_Y_TURN_SPEED;
    fps->player_index = 0;

    fw64_camera_init(&fps->camera);
    vec2_set(&fps->rotation, 0.0f, 0.0f);
}

static void fps_cam_forward(fw64FpsCamera* fps, Vec3* out) {
    Quat q;
    quat_from_euler(&q, 0.0f, fps->rotation.y, 0.0f);

    Vec3 forward = { 0.0f, 0.0f, -1.0f };
    quat_transform_vec3(out, &q, &forward);
}

static void fps_cam_back(fw64FpsCamera* fps, Vec3* out) {
    fps_cam_forward(fps, out);
    vec3_negate(out);
}

static void fps_cam_right(fw64FpsCamera* fps, Vec3* out) {
    Quat q;
    quat_from_euler(&q, 0.0f, fps->rotation.y, 0.0f);

    Vec3 right = { 1.0f, 0.0f, 0.0f };
    quat_transform_vec3(out, &q, &right);
}

static void fps_cam_left(fw64FpsCamera* fps, Vec3* out) {
    fps_cam_right(fps, out);
    vec3_negate(out);
}

static void move_camera(fw64FpsCamera* fps, float time_delta, Vec2* stick) {
    int did_move = 0;
    Vec3 move = {0.0f, 0.0f, 0.0f};

    if(mapped_input_controller_read(fps->input_map, fps->player_index, INPUT_MAP_MOVE_RIGHT, stick) >= 1) {
        controller_cam_right(controller, &move);
        vec3_scale(&move, &move, controller->movement_speed * time_delta);
        did_move = 1;
    }

    if (mapped_input_controller_read(fps->input_map, fps->player_index, INPUT_MAP_MOVE_LEFT, stick) >= 1) {
        controller_cam_left(controller, &move);
        vec3_scale(&move, &move, controller->movement_speed * time_delta);
        did_move = 1;
    }

    if (mapped_input_controller_read(fps->input_map, fps->player_index, INPUT_MAP_MOVE_FORWARD, stick) >= 1) {
        controller_cam_forward(controller, &move);
        vec3_scale(&move, &move, controller->movement_speed * time_delta * stick->y);
        did_move = 1;
    }

    if (mapped_input_controller_read(fps->input_map, fps->player_index, INPUT_MAP_MOVE_BACKWARD, stick) >= 1) {
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

static void tilt_camera(fw64FpsCamera* fps, float time_delta, Vec2* stick) {
    if (mapped_input_controller_read(fps->input_map, fps->player_index, INPUT_MAP_LOOK_RIGHT, stick) >= 1) {
        float delta = fps->turn_speed.y * time_delta;
        if(stick->x > fps->input_map->threshold.x || stick->x < -fps->input_map->threshold.x)
            delta *= stick->x;
        fps->rotation.y -= delta;
    } else if (mapped_input_controller_read(fps->input_map, fps->player_index, INPUT_MAP_LOOK_LEFT, stick) >= 1) {
        float delta = fps->turn_speed.y * time_delta;
        if(stick->x > fps->input_map->threshold.x || stick->x < -fps->input_map->threshold.x)
            delta *= stick->x;
        fps->rotation.y += delta;
    }

    if (mapped_input_controller_read(fps->input_map, fps->player_index, INPUT_MAP_LOOK_UP, stick) >= 1) {
        fps->rotation.x += fps->turn_speed.x * time_delta;

        if (fps->rotation.x > 90.0f)
            fps->rotation.x = 90.0f;
    } else if (mapped_input_controller_read(fps->input_map, fps->player_index, INPUT_MAP_LOOK_DOWN, stick) >= 1) {
        fps->rotation.x -= fps->turn_speed.x * time_delta;

        if (fps->rotation.x < -90.0f)
            fps->rotation.x = -90.0f;
    }
}

void fw64_fps_camera_update(fw64FpsCamera* fps, float time_delta) {
    Vec2 stick;
    stick.x = 0;
    stick.y = 0;
    //fw64_input_controller_stick(fps->_input, fps->player_index, &stick);

    move_camera(fps, time_delta, &stick);
    
    stick.x = 0;
    stick.y = 0;
    tilt_camera(fps, time_delta, &stick);

    Quat q;
    quat_from_euler(&q, fps->rotation.x, fps->rotation.y, 0.0f);

    Vec3 forward = { 0.0f, 0.0f, -1.0f };
    Vec3 tar;
    quat_transform_vec3(&tar, &q, &forward);
    vec3_add(&tar, &fps->camera.transform.position, &tar);

    Vec3 up = {0.0f, 1.0f, 0.0f};

    fw64_transform_look_at(&fps->camera.transform, &tar, &up);
    fw64_camera_update_view_matrix(&fps->camera);
}