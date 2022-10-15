#include "movement_controller.h"

#include "assets/layers.h"

#include "framework64/n64/controller_button.h"
#include "framework64/math.h"

#define DEFAULT_MOVEMENT_SPEED 8.0f
#define DEFAULT_X_TURN_SPEED 90.0f  // look up-down
#define DEFAULT_Y_TURN_SPEED 180.0f // look left-right
#define STICK_THRESHOLD 0.15

static void movement_controller_get_ground_height(MovementController* controller);

void movement_controller_init(MovementController* controller, InputMapping* input_map, WeaponBob* weapon_bob, fw64Level* level, fw64Collider* collider){
    controller->input_map = input_map;
    controller->weapon_bob = weapon_bob;
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
    int move_test;
    float analog_mod = 1.0f;
    Vec3 temp = {0.0f, 0.0f, 0.0f};
    Vec3 move = {0.0f, 0.0f, 0.0f};


    move_test = mapped_input_controller_read(controller->input_map, controller->player_index, INPUT_MAP_MOVE_RIGHT, stick);
    if(move_test == INPUT_MAP_BUTTON_DOWN || move_test == INPUT_MAP_ANALOG) {           
        analog_mod = mapped_input_get_axis(controller->input_map, INPUT_MAP_MOVE_RIGHT, stick);
        fps_cam_right(controller, &temp);
        vec3_add_and_scale(&move, &move, &temp, controller->movement_speed * analog_mod * time_delta);
        did_move = 1;
    }

    move_test = mapped_input_controller_read(controller->input_map, controller->player_index, INPUT_MAP_MOVE_LEFT, stick);
    if(move_test == INPUT_MAP_BUTTON_DOWN || move_test == INPUT_MAP_ANALOG) {           
        analog_mod = mapped_input_get_axis(controller->input_map, INPUT_MAP_MOVE_LEFT, stick);
        fps_cam_left(controller, &temp);
        vec3_add_and_scale(&move, &move, &temp, controller->movement_speed * analog_mod * time_delta);
        did_move = 1;
    }

    move_test = mapped_input_controller_read(controller->input_map, controller->player_index, INPUT_MAP_MOVE_FORWARD, stick);
    if(move_test == INPUT_MAP_BUTTON_DOWN || move_test == INPUT_MAP_ANALOG) {           
        analog_mod = mapped_input_get_axis(controller->input_map, INPUT_MAP_MOVE_FORWARD, stick);
        fps_cam_forward(controller, &temp);
        vec3_add_and_scale(&move, &move, &temp, controller->movement_speed * analog_mod * time_delta);
        did_move = 1;
    }

    move_test = mapped_input_controller_read(controller->input_map, controller->player_index, INPUT_MAP_MOVE_BACKWARD, stick);
    if(move_test == INPUT_MAP_BUTTON_DOWN || move_test == INPUT_MAP_ANALOG) {           
        analog_mod = mapped_input_get_axis(controller->input_map, INPUT_MAP_MOVE_BACKWARD, stick);
        fps_cam_back(controller, &temp);
        vec3_add_and_scale(&move, &move, &temp, controller->movement_speed * analog_mod * time_delta);
        did_move = 1;
    }


    // note this is just prelimary...will probably need to get more fancy in the future.
    if (!did_move)
        return;

    controller->weapon_bob->is_active = 1;
    // prevent running faster by moving diagonally
    Vec3 ref_zero = {0.0f, 0.0f, 0.0f};
    if(vec3_distance(&move, &ref_zero) > (controller->movement_speed * time_delta))
    {
        vec3_normalize(&move);
        vec3_scale(&move, &move, controller->movement_speed * time_delta);
    }
    controller->weapon_bob->step_speed = vec3_distance(&ref_zero, &move) / (time_delta); 
    fw64IntersectMovingSphereQuery query;
    int remaining_checks = 10;
    while (remaining_checks > 0 && fw64_level_moving_sphere_intersection(controller->level, &controller->camera.transform.position, 1.0f, &move, controller->collision_mask, &query)) {
        remaining_checks--; //prevent weird infinite loop of collisions condition
        Vec3 collision_normal = {0.0f, 0.0f, 0.0f};
        fw64_collision_get_normal_box_point(&query.results[0].point,
                                    &query.results[0].node->collider->bounding,
                                    &collision_normal);
        float strength = fw64_fabsf(vec3_dot(&move, &collision_normal));
        vec3_add_and_scale(&move, &move, &collision_normal, strength); 
        //vec3_scale(&move, &move, query.results[0].tfirst - 0.1);
    }

    vec3_add(&controller->camera.transform.position, &controller->camera.transform.position, &move);
}

static void tilt_camera(MovementController* fps, float time_delta, Vec2* stick) {
    int look_test = 0;
    float analog_mod = 0.0f;

    look_test = mapped_input_controller_read(fps->input_map, fps->player_index, INPUT_MAP_LOOK_RIGHT, stick);
    if (look_test == INPUT_MAP_BUTTON_DOWN || look_test == INPUT_MAP_ANALOG) {
        analog_mod = mapped_input_get_axis(fps->input_map, INPUT_MAP_LOOK_RIGHT, stick);
        float delta = fps->turn_speed.y * analog_mod * time_delta;
        fps->rotation.y -= delta;
    }
    look_test = mapped_input_controller_read(fps->input_map, fps->player_index, INPUT_MAP_LOOK_LEFT, stick);
    if (look_test == INPUT_MAP_BUTTON_DOWN || look_test == INPUT_MAP_ANALOG) {
        analog_mod = mapped_input_get_axis(fps->input_map, INPUT_MAP_LOOK_LEFT, stick);
        float delta = fps->turn_speed.y * analog_mod * time_delta;
        fps->rotation.y += delta;
    }

    look_test = mapped_input_controller_read(fps->input_map, fps->player_index, INPUT_MAP_LOOK_UP, stick);
    if (look_test == INPUT_MAP_BUTTON_DOWN || look_test == INPUT_MAP_ANALOG) {
        analog_mod = mapped_input_get_axis(fps->input_map, INPUT_MAP_LOOK_UP, stick);
        fps->rotation.x += fps->turn_speed.x * analog_mod * time_delta;

        if (fps->rotation.x > 90.0f)
            fps->rotation.x = 90.0f;
    }
    look_test = mapped_input_controller_read(fps->input_map, fps->player_index, INPUT_MAP_LOOK_DOWN, stick);
    if (look_test == INPUT_MAP_BUTTON_DOWN || look_test == INPUT_MAP_ANALOG) {
        analog_mod = mapped_input_get_axis(fps->input_map, INPUT_MAP_LOOK_DOWN, stick);
        fps->rotation.x -= fps->turn_speed.x * analog_mod * time_delta;

        if (fps->rotation.x < -90.0f)
            fps->rotation.x = -90.0f;
    }
}

// returns the translation vector length
void movement_controller_update(MovementController* fps, float time_delta) {
    Vec2 stick = {0.0f, 0.0f};   
    move_camera(fps, time_delta, &stick);
    movement_controller_get_ground_height(fps);
    
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