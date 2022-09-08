#include "zombie.h"
#include "assets/zombie_animation.h"

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
    zombie->previous_state = ZOMBIE_STATE_INACTIVE;
    zombie->state = ZOMBIE_STATE_INACTIVE;
    zombie->health = ZOMBIE_MAX_HEALTH;
    vec3_zero(&zombie->velocity);
    zombie->rotation = (rand() % 360) * (M_PI / 180.0f);
    zombie->rotation_speed = 90.0f * (M_PI / 180.0f); //90 deg/sec max turn speed. not used yet
    zombie->max_speed = 0.0f;
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

void zombie_move(Zombie* zombie) {

    zombie_apply_active_behaviors(zombie);

    //this chunk just adjusts the rotation of the zombie's model when drawn.
    //I'm sure it can be done better without any atan2 nonsense
    //atan2 return from -pi to + pi.
    //so there's a spot where the function is not continuous, which is annoying
    float zombie_new_direction = atan2(-zombie->velocity.z, zombie->velocity.x) + (M_PI / 2.0f);
    float zombie_dir_delta = (zombie_new_direction - zombie->rotation);
    zombie->rotation += zombie_dir_delta;
    if(zombie->rotation > M_PI) {
        zombie->rotation -= 2.0f * M_PI;
    } else if (zombie->rotation < (-M_PI)) {
        zombie->rotation += 2.0f * M_PI;
    }
    quat_set_axis_angle(&zombie->node.transform.rotation, 0, 1, 0, zombie->rotation);

    vec3_add(&zombie->node.transform.position, &zombie->node.transform.position, &zombie->velocity);

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

    zombie_set_behavior(zombie, SB_NONE);

    int animation;
    int loop;
    float speed = 1.0f;

    switch (zombie->state) {
        case ZOMBIE_STATE_IDLE:
            zombie->max_speed = 0.0f;
            animation = zombie_animation_Idle;
            loop = 1;
        break;

        case ZOMBIE_STATE_RUNNING:
            zombie_set_behavior(zombie, SB_SEEK);
            zombie->max_speed = ZOMBIE_RUN_SPEED;
            animation = zombie_animation_Run;
            loop = 1;
        break;

        case ZOMBIE_STATE_WALKING:
            zombie_set_behavior(zombie, SB_SEEK);
            zombie->max_speed = ZOMBIE_WALK_SPEED;
            animation = zombie_animation_Walk;
            loop = 1;
        break;

        case ZOMBIE_STATE_HIT_REACTION:
            zombie->max_speed = 0.0f;
            animation = zombie_animation_Hit;
            loop = 0;
            speed = 2.35f;
        break;

        case ZOMBIE_STATE_FALLING_DOWN:
            zombie->max_speed = 0.0f;
            animation = zombie_animation_Death;
            loop = 0;
        break;

        case ZOMBIE_FLYING_BACK:
            zombie->max_speed = 0.0f;
            animation = zombie_animation_FlyBack;
            loop = 0;
        break;

        case ZOMBIE_STATE_INACTIVE:
        case ZOMBIE_STATE_DEAD:
            zombie->max_speed = 0.0f;
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
    fw64_animation_controller_play(&zombie->animation_controller);
}

void zombie_set_behavior(Zombie* zombie, SteeringBehavior behavior) {
    zombie->active_bahaviors |= behavior;
}

void zombie_clear_behavior(Zombie* zombie, SteeringBehavior behavior) {
    zombie->active_bahaviors &= ~(behavior);
}

void zombie_apply_behavior(Zombie* zombie, SteeringBehavior behavior) {
    Vec3 behavior_velocity;
    float displacement = min(zombie->max_speed, zombie->max_speed * zombie->engine->time->time_delta); //cap displacement at 1 second's worth. hacky debug tool.
    Vec3 position = zombie->node.transform.position;
    Vec3 target = zombie->target->position;
    position.y = 0;
    target.y = 0;
    
    switch(behavior) {
        case SB_SEEK:          
            steering_seek(&position, &target, displacement, &behavior_velocity);  //here displacement is simple. just the max move speed of the zombie.                                                                      //theoretically though you can adjust this to adjust the strength of this behavior  
            vec3_add(&zombie->velocity, &zombie->velocity, &behavior_velocity);
            break;
        case SB_FLEE:
            steering_flee(&position, &target, displacement, &behavior_velocity);  //here displacement is simple. just the max move speed of the zombie.                                                                      //theoretically though you can adjust this to adjust the strength of this behavior  
            vec3_add(&zombie->velocity, &zombie->velocity, &behavior_velocity);
            break;
        default: break;
    }

    vec3_normalize(&zombie->velocity);
    vec3_scale(&zombie->velocity, &zombie->velocity, displacement);
}

void zombie_apply_active_behaviors(Zombie* zombie) {
    for(int i = 0; i < SB_TOTAL_BEHAVIORS; i++) {
        unsigned long long int check_bahavior = (1 << i);
        if(check_bahavior & zombie->active_bahaviors) {
            zombie_apply_behavior(zombie, (SteeringBehavior)check_bahavior);
        }
    }
}