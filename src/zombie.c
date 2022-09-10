#include "zombie.h"
#include "zombie_animation.h"
#include "framework64/random.h"
#include "behaviors.h"

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
    vec3_zero(&zombie->velocity_linear);
    zombie->velocity_angular = 0.0f;
    zombie->rotation = fw64_random_float_in_range(0.0f, 359.9f) * (M_PI / 180.0f);
    zombie->max_speed_linear = 0.0f;
    zombie->max_speed_angular = 0.0f;
    zombie->max_accel_linear = 0.0f;
    zombie->max_accel_angular = 0.0f;
    fw64_node_init(&zombie->node);
    zombie->node.layer_mask = ZOMBIE_LAYER;
    zombie->node.data = zombie;
    vec3_set_all(&zombie->node.transform.scale, ZOMBIE_SCALE);
    fw64_node_set_mesh(&zombie->node, zombie->mesh);
    fw64_node_set_box_collider(&zombie->node, &zombie->collider);
    fw64_node_update(&zombie->node);

    steering_behavior_data_init(zombie->node.transform.position,
                                zombie->node.transform.position,
                                &zombie->velocity_linear,
                                zombie->engine->time->time_delta,
                                &zombie->sb_data);
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

void zombie_move(Zombie* zombie) {

    zombie_apply_active_behaviors(zombie);
    // char debugMessage[80];
    // float dist = vec3_distance(&zombie->node.transform.position, &zombie->target->position);
    // sprintf(debugMessage, "Vel: (%.4f, %.4f, %.4f)   Dist: %.4f", zombie->velocity_linear.x,
    //                                                               zombie->velocity_linear.y,
    //                                                               zombie->velocity_linear.z,
    //                                                               dist);
    
    // fw64_debug_log(debugMessage);
    //this chunk just adjusts the rotation of the zombie's model when drawn.
    //I'm sure it can be done better without any atan2 nonsense
    //atan2 return from -pi to + pi.
    //so there's a spot where the function is not continuous, which is annoying
    Vec3 ref_zero;
    vec3_zero(&ref_zero);
    if((vec3_distance_squared(&ref_zero, &zombie->velocity_linear)) > 0.01f)
    {
        float zombie_new_direction = atan2(-zombie->velocity_linear.z, zombie->velocity_linear.x) + (M_PI / 2.0f);
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
    vec3_scale(&delta_vel, &zombie->velocity_linear, zombie->engine->time->time_delta);
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

    zombie_clear_behavior(zombie, SB_ALL); //clear all active behaviors on new state

    int animation;
    int loop;
    float speed = 1.0f;

    switch (zombie->state) {
        case ZOMBIE_STATE_IDLE:
            zombie->max_speed_linear = 0.0f;
            animation = zombie_animation_Idle;
            loop = 1;
        break;

        case ZOMBIE_STATE_RUNNING:
            zombie_set_behavior(zombie, SB_PURSUE);
            zombie->max_speed_linear = ZOMBIE_RUN_SPEED;
            animation = zombie_animation_Run;
            loop = 1;
        break;

        case ZOMBIE_STATE_WALKING:
            zombie_set_behavior(zombie, SB_EVADE);
            zombie->max_speed_linear = ZOMBIE_WALK_SPEED;
            animation = zombie_animation_Walk;
            loop = 1;
        break;

        case ZOMBIE_STATE_HIT_REACTION:
            zombie->max_speed_linear = 0.0f;
            animation = zombie_animation_Hit;
            loop = 0;
            speed = 2.35f;
        break;

        case ZOMBIE_STATE_FALLING_DOWN:
            zombie->max_speed_linear = 0.0f;
            animation = zombie_animation_Death;
            loop = 0;
        break;

        case ZOMBIE_FLYING_BACK:
            zombie->max_speed_linear = 0.0f;
            animation = zombie_animation_FlyBack;
            loop = 0;
        break;

        case ZOMBIE_STATE_INACTIVE:
        case ZOMBIE_STATE_DEAD:
            zombie->max_speed_linear = 0.0f;
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

void zombie_set_behavior(Zombie* zombie, SteeringBehavior behavior) {
    zombie->active_bahaviors |= behavior;
}

void zombie_clear_behavior(Zombie* zombie, SteeringBehavior behavior) {
    zombie->active_bahaviors &= ~(behavior);
}

void zombie_apply_behavior(Zombie* zombie, SteeringBehavior behavior) {
    steering_behavior_data_init(zombie->node.transform.position,
                                zombie->target->position,
                                &zombie->velocity_linear,
                                zombie->engine->time->time_delta,
                                &zombie->sb_data);
    
    zombie->sb_data.position.y = 0.0f;
    zombie->sb_data.targetPosition.y = 0.0f;

    //float displacement = min(zombie->max_speed_linear, zombie->max_speed_linear * zombie->engine->time->time_delta); //cap displacement at 1 second's worth. hacky debug tool.
    
    switch(behavior) {
        case SB_SEEK:          
            steering_seek(1.0f, &zombie->sb_data);            
            break;
        case SB_FLEE:
            steering_flee(1.0f, &zombie->sb_data);
            break;
        case SB_ARRIVE:
            steering_arrive(20.0f, 10.0f, 1.0f, &zombie->sb_data);
            break;
        case SB_PURSUE:
            vec3_subtract(&zombie->targetVelocity, &zombie->target->position, &zombie->targetPrevious->position);
            zombie->targetPrevious->position = zombie->target->position;
            steering_pursue(&zombie->targetVelocity, 1.0f, &zombie->sb_data);
            break;
        case SB_EVADE:
            vec3_subtract(&zombie->targetVelocity, &zombie->target->position, &zombie->targetPrevious->position);
            zombie->targetPrevious->position = zombie->target->position;
            steering_evade(&zombie->targetVelocity, 1.0f, &zombie->sb_data);
            break;
        default: break;
    }
    vec3_add(&zombie->velocity_linear, &zombie->velocity_linear, &zombie->sb_data.linearAccel);
    Vec3 ref_zero;
    vec3_zero(&ref_zero);
    if(vec3_distance_squared(&ref_zero, &zombie->velocity_linear) > (zombie->max_speed_linear * zombie->max_speed_linear)) {
        vec3_normalize(&zombie->velocity_linear);
        vec3_scale(&zombie->velocity_linear, &zombie->velocity_linear, zombie->max_speed_linear);
    }
}

void zombie_apply_active_behaviors(Zombie* zombie) {
    for(int i = 0; i < SB_TOTAL_BEHAVIORS; i++) {
        unsigned long long int check_bahavior = (1 << i);
        if(check_bahavior & zombie->active_bahaviors) {
            zombie_apply_behavior(zombie, (SteeringBehavior)check_bahavior);
        }
    }
}