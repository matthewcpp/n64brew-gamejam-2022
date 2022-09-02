#include "zombie.h"
#include "zombie_animation.h"

#define ZOMBIE_SCALE 0.025f
#define ZOMBIE_WALK_SPEED 5.0f
#define ZOMBIE_RUN_SPEED 15.0f
#define ZOMBIE_VISION_DISTANCE 45.0f

static void zombie_set_new_state(Zombie* zombie, ZombieState new_state);
static void zombie_move(Zombie* zombie, float speed);

void zombie_init(Zombie* zombie, fw64Engine* engine, fw64Node* node, fw64Mesh* mesh, fw64AnimationData* animation_data) {
    zombie->engine = engine;
    zombie->node = node;
    zombie->mesh = mesh;
    zombie->target = NULL;
    zombie->previous_state = ZOMBIE_STATE_IDLE;
    zombie->state = ZOMBIE_STATE_IDLE;
    zombie->health = 3;

    vec3_set_all(&zombie->node->transform.scale, ZOMBIE_SCALE);
    fw64_node_update(zombie->node);
    fw64_animation_controller_init(&zombie->animation_controller, animation_data, zombie_animation_Idle, NULL);
    zombie_set_new_state(zombie, ZOMBIE_STATE_IDLE);
}

static void zombie_update_idle(Zombie* zombie) {
    if (!zombie->target)
        return;

    Vec3 target_xz = zombie->target->position;
    target_xz.y = 0.0f;
    Vec3 zombie_xz = zombie->node->transform.position;
    zombie_xz.y = 0.0f;

    if (vec3_distance_squared(&zombie_xz, &target_xz) <= (ZOMBIE_VISION_DISTANCE * ZOMBIE_VISION_DISTANCE)) {
        zombie_set_new_state(zombie, ZOMBIE_STATE_WALKING);
    }
}

#include "framework64/n64/controller_button.h"

void zombie_move(Zombie* zombie, float speed) {
    Vec3 target_xz = zombie->target->position;
    target_xz.y = 0.0f;
    Vec3 zombie_xz = zombie->node->transform.position;
    zombie_xz.y = 0.0f;

    Vec3 direction_to_target;

    vec3_subtract(&direction_to_target, &target_xz, &zombie_xz);
    vec3_normalize(&direction_to_target);
    vec3_add_and_scale(&zombie->node->transform.position, &zombie->node->transform.position, &direction_to_target, speed * zombie->engine->time->time_delta);
    
    fw64_node_update(zombie->node);
}

static void zombie_update_running(Zombie* zombie) {
    zombie_move(zombie, ZOMBIE_RUN_SPEED);

    // temp
    if (fw64_input_controller_button_pressed(zombie->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_Z)) {
        zombie_hit(zombie);
    }
}

static void zombie_update_walking(Zombie* zombie) {
    zombie_move(zombie, ZOMBIE_WALK_SPEED);

    // temp
    if (fw64_input_controller_button_pressed(zombie->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_Z)) {
        zombie_hit(zombie);
    }
}

static void zombie_update_hit_reaction(Zombie* zombie) {
    if (zombie->animation_controller.state == FW64_ANIMATION_STATE_STOPPED) {
        zombie_set_new_state(zombie, zombie->previous_state);
    }
}

static void zombie_update_death(Zombie* zombie) {
    if (zombie->animation_controller.state == FW64_ANIMATION_STATE_STOPPED) {
        zombie_set_new_state(zombie, ZOMBIE_STATE_DEAD);
    }
}

void zombie_hit(Zombie* zombie) {
    zombie-> health -= 1;

    if (zombie->health == 0) {
        zombie_set_new_state(zombie, ZOMBIE_STATE_DEATH);
        return;
    }

    if (zombie->state != ZOMBIE_STATE_HIT_REACTION) {
        zombie_set_new_state(zombie, ZOMBIE_STATE_HIT_REACTION);
    }
}

void zombie_update(Zombie* zombie) {
    fw64_animation_controller_update(&zombie->animation_controller, zombie->engine->time->time_delta);

    switch(zombie->state) {
        case ZOMBIE_STATE_IDLE:
            zombie_update_idle(zombie);
        break;

        case ZOMBIE_STATE_RUNNING:
            zombie_update_running(zombie);
        break;

        case ZOMBIE_STATE_WALKING:
            zombie_update_walking(zombie);
        break;

        case ZOMBIE_STATE_HIT_REACTION:
            zombie_update_hit_reaction(zombie);
        break;

        case ZOMBIE_STATE_DEATH:
            zombie_update_death(zombie);
        break;

        case ZOMBIE_STATE_DEAD:
            return;
        break;
    }
}

void zombie_set_new_state(Zombie* zombie, ZombieState new_state) {
    zombie->previous_state = zombie->state;
    zombie->state = new_state;

    int animation;
    int loop;
    float speed = 1.0f;

    switch (zombie->state) {
        case ZOMBIE_STATE_IDLE:
            animation = zombie_animation_Idle;
            loop = 1;
        break;

        case ZOMBIE_STATE_RUNNING:
            animation = zombie_animation_Run;
            loop = 1;
        break;

        case ZOMBIE_STATE_WALKING:
            animation = zombie_animation_Walk;
            loop = 1;
        break;

        case ZOMBIE_STATE_HIT_REACTION:
            animation = zombie_animation_Hit;
            loop = 0;
            speed = 1.75f;
        break;

        case ZOMBIE_STATE_DEATH:
            animation = zombie_animation_Death;
            loop = 0;
        break;

        case ZOMBIE_STATE_DEAD:
            return;
        break;
    }

    fw64_animation_controller_set_animation(&zombie->animation_controller, animation);
    zombie->animation_controller.loop = loop;
    zombie->animation_controller.speed = speed;
    fw64_animation_controller_play(&zombie->animation_controller);
}

void zombie_draw(Zombie* zombie) {
    fw64_renderer_draw_animated_mesh(zombie->engine->renderer, zombie->mesh, &zombie->animation_controller, &zombie->node->transform);
}

void zombie_set_target(Zombie* zombie, fw64Transform* target) {
    zombie->target = target;
    fw64_animation_controller_play(&zombie->animation_controller);
}