#include "zombie.h"
#include "assets/zombie_animation.h"
#include "framework64/random.h"

#define ZOMBIE_SCALE 0.025f
#define ZOMBIE_WALK_SPEED 5.0f
#define ZOMBIE_RUN_SPEED 15.0f
#define ZOMBIE_VISION_DISTANCE 45.0f
;
static void zombie_move(Zombie* zombie);

void zombie_init(Zombie* zombie, fw64Engine* engine, fw64Level* level, fw64Mesh* mesh, fw64AnimationData* animation_data) {
    zombie->engine = engine;
    zombie->level = level;
    zombie->mesh = mesh;
    zombie->target = NULL;
    vec3_zero(&zombie->targetVelocity);
    zombie->previous_state = ZOMBIE_STATE_INACTIVE;
    zombie->state = ZOMBIE_STATE_INACTIVE;
    zombie->health = ZOMBIE_MAX_HEALTH;
    zombie->rotation = fw64_random_float_in_range(0.0f, 359.9f) * (M_PI / 180.0f);
    fw64_node_init(&zombie->node);
    zombie->node.layer_mask = ZOMBIE_LAYER;
    zombie->node.data = zombie;
    vec3_set_all(&zombie->node.transform.scale, ZOMBIE_SCALE);
    fw64_node_set_mesh(&zombie->node, zombie->mesh);
    fw64_node_set_box_collider(&zombie->node, &zombie->collider);
    fw64_node_update(&zombie->node);
    fw64_animation_controller_init(&zombie->animation_controller, animation_data, zombie_animation_Idle, NULL);
    fw64_level_add_dyanmic_node(level, &zombie->node);
}

static void zombie_update_idle(Zombie* zombie) {
    if (!zombie->target)
        return;

    Vec3 target_xz = zombie->target->position;
    target_xz.y = 0.0f;
    Vec3 zombie_xz = zombie->node.transform.position;
    zombie_xz.y = 0.0f;

    if (vec3_distance_squared(&zombie_xz, &target_xz) <= (ZOMBIE_VISION_DISTANCE * ZOMBIE_VISION_DISTANCE)) {
        zombie_set_new_state(zombie, ZOMBIE_STATE_WALKING);
    }
}

static void zombie_move(Zombie* zombie) {

    zombie_ai_update(&zombie->ai, zombie->engine->time->time_delta);

    //this chunk just adjusts the rotation of the zombie's model when drawn.
    //I'm sure it can be done better without any atan2 nonsense
    //atan2 return from -pi to + pi.
    //so there's a spot where the function is not continuous, which is annoying
    Vec3 ref_zero;
    vec3_zero(&ref_zero);
    if((vec3_distance_squared(&ref_zero, &zombie->ai.velocity.linear )) > 0.01f)
    {
        float zombie_new_direction = atan2(-zombie->ai.velocity.linear.z, zombie->ai.velocity.linear.x) + (M_PI / 2.0f);
        float zombie_dir_delta = (zombie_new_direction - zombie->rotation);
        zombie->rotation += zombie_dir_delta;
        if(zombie->rotation > M_PI) {
            zombie->rotation -= 2.0f * M_PI;
        } else if (zombie->rotation < (-M_PI)) {
            zombie->rotation += 2.0f * M_PI;
        }
        quat_set_axis_angle(&zombie->node.transform.rotation, 0, 1, 0, zombie->rotation);
    }

    Vec3 delta_vel;
    vec3_scale(&delta_vel, &zombie->ai.velocity.linear, zombie->engine->time->time_delta);
    vec3_add(&zombie->node.transform.position, &zombie->node.transform.position, &delta_vel);

    fw64_node_update(&zombie->node);
}

static void zombie_update_hit_reaction(Zombie* zombie) {
    if (zombie->animation_controller.state == FW64_ANIMATION_STATE_STOPPED) {
        zombie_set_new_state(zombie, zombie->previous_state);
    }
}

static void zombie_update_moving(Zombie* zombie) {
    zombie_move(zombie);
}

static void zombie_update_dying(Zombie* zombie) {
    if (zombie->animation_controller.state == FW64_ANIMATION_STATE_STOPPED) {
        zombie_set_new_state(zombie, ZOMBIE_STATE_DEAD);
    }
}

void zombie_hit(Zombie* zombie, WeaponType weapon_type) {
    if (weapon_type == WEAPON_TYPE_SHOTGUN) {
        zombie_set_new_state(zombie, ZOMBIE_FLYING_BACK);
        zombie->health -= 3;
        return;
    }

    zombie->health -= 1;

    if (zombie->health <= 0) {
        zombie_set_new_state(zombie, ZOMBIE_STATE_FALLING_DOWN);
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
        case ZOMBIE_STATE_WALKING:
            zombie_update_moving(zombie);
        break;

        case ZOMBIE_STATE_HIT_REACTION:
            zombie_update_hit_reaction(zombie);
        break;

        case ZOMBIE_STATE_FALLING_DOWN:
        case ZOMBIE_FLYING_BACK:
            zombie_update_dying(zombie);
        break;

        case ZOMBIE_STATE_INACTIVE:
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
            speed = 2.35f;
        break;

        case ZOMBIE_STATE_FALLING_DOWN:
            animation = zombie_animation_Death;
            loop = 0;
        break;

        case ZOMBIE_FLYING_BACK:
            animation = zombie_animation_FlyBack;
            loop = 0;
        break;

        case ZOMBIE_STATE_INACTIVE:
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
    fw64_renderer_draw_animated_mesh(zombie->engine->renderer, zombie->mesh, &zombie->animation_controller, &zombie->node.transform);
}

void zombie_set_target(Zombie* zombie, fw64Transform* target) {
    zombie->target = target;
    zombie->targetPrevious = target;
    fw64_animation_controller_play(&zombie->animation_controller);
}