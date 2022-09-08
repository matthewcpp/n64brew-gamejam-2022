#include "movement_controller.h"

#include "assets/layers.h"

#include "framework64/n64/controller_button.h"

#define DEFAULT_MOVEMENT_SPEED 8.0f
#define DEFAULT_X_TURN_SPEED 180.0f
#define DEFAULT_Y_TURN_SPEED 90.0f
#define STICK_THRESHOLD 0.15

static void movement_controller_get_ground_height(MovementController* controller);

void movement_controller_init(MovementController* controller, InputMapping* input_map, fw64Level* level, fw64Collider* collider){
    controller->input_map = input_map;
    controller->level = level;
    controller->collider = collider;

    controller->movement_speed = DEFAULT_MOVEMENT_SPEED;
    controller->turn_speed.x = DEFAULT_X_TURN_SPEED;
    controller->turn_speed.y = DEFAULT_Y_TURN_SPEED;
    controller->player_index = 0;

    fw64_camera_init(&controller->camera);
    vec2_set(&controller->rotation, 0.0f, 0.0f);
}

static void fps_cam_forward(MovementController* fps, Vec3* out) {
    Quat q;
    quat_from_euler(&q, 0.0f, fps->rotation.y, 0.0f);

    Vec3 forward = { 0.0f, 0.0f, -1.0f };
    quat_transform_vec3(out, &q, &forward);
}

static void fps_cam_back(MovementController* fps, Vec3* out) {
    fps_cam_forward(fps, out);
    vec3_negate(out);
}

static void fps_cam_right(MovementController* fps, Vec3* out) {
    Quat q;
    quat_from_euler(&q, 0.0f, fps->rotation.y, 0.0f);

    Vec3 right = { 1.0f, 0.0f, 0.0f };
    quat_transform_vec3(out, &q, &right);
}

static void fps_cam_left(MovementController* fps, Vec3* out) {
    fps_cam_right(fps, out);
    vec3_negate(out);
}

static void move_camera(MovementController* controller, float time_delta, Vec2* stick) {
    int did_move = 0;
    Vec3 move = {0.0f, 0.0f, 0.0f};

    if(mapped_input_controller_read(controller->input_map, controller->player_index, INPUT_MAP_MOVE_RIGHT, stick) >= 1) {
        fps_cam_right(controller, &move);
        vec3_scale(&move, &move, controller->movement_speed * time_delta);
        did_move = 1;
    }

    if (mapped_input_controller_read(controller->input_map, controller->player_index, INPUT_MAP_MOVE_LEFT, stick) >= 1) {
        fps_cam_left(controller, &move);
        vec3_scale(&move, &move, controller->movement_speed * time_delta);
        did_move = 1;
    }

    if (mapped_input_controller_read(controller->input_map, controller->player_index, INPUT_MAP_MOVE_FORWARD, stick) >= 1) {
        fps_cam_forward(controller, &move);
        vec3_scale(&move, &move, controller->movement_speed * time_delta * stick->y);
        did_move = 1;
    }

    if (mapped_input_controller_read(controller->input_map, controller->player_index, INPUT_MAP_MOVE_BACKWARD, stick) >= 1) {
        fps_cam_back(controller, &move);
        vec3_scale(&move, &move, controller->movement_speed * time_delta * -stick->y);
        did_move = 1;
    }

    if (!did_move)
        return;

    fw64IntersectMovingBoxQuery query;
    if (fw64_level_moving_box_intersection(controller->level, &controller->collider->bounding, &move, controller->collision_mask, &query)) {
        vec3_scale(&move, &move, query.results[0].tlast - 0.1);
    }

    vec3_add(&controller->camera.transform.position, &controller->camera.transform.position, &move);
}

static void tilt_camera(MovementController* fps, float time_delta, Vec2* stick) {
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

void movement_controller_update(MovementController* fps, float time_delta) {
    Vec2 stick;
    stick.x = 0;
    stick.y = 0;
    //fw64_input_controller_stick(fps->_input, fps->player_index, &stick);

    movement_controller_get_ground_height(fps);
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

void movement_controller_get_ground_height(MovementController* controller) {
    fw64RaycastHit raycast_hit;

    Vec3 ray_pos = controller->camera.transform.position;
    ray_pos.y = 1000.0f;
    Vec3 ray_dir = {0.0f, -1.0f, 0.0f};

    if (fw64_level_raycast(controller->level, &ray_pos, &ray_dir, FW64_layer_ground, &raycast_hit)) {
        controller->camera.transform.position.y = raycast_hit.point.y + controller->height;
    }
}