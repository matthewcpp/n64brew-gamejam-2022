#include "zombie.h"
#include "assets/zombie_animation.h"
#include "assets/layers.h"
#include "framework64/random.h"
#include "framework64/collision.h"
#include "player.h"

#define ZOMBIE_SCALE 0.025f

static void zombie_move(Zombie* zombie);

void zombie_init(Zombie* zombie, fw64Engine* engine, fw64Level* level, fw64Mesh* mesh, fw64AnimationData* animation_data, fw64Allocator* allocator) {
    zombie->engine = engine;
    zombie->level = level;
    zombie->mesh = mesh;

    zombie_reset(zombie);

    fw64_animation_controller_init(&zombie->animation_controller, animation_data, zombie_animation_Idle, allocator);
}

void zombie_reset(Zombie* zombie) {
    zombie->target = NULL;
    fw64_node_init(&zombie->node);
    zombie->node.layer_mask = ZOMBIE_LAYER;
    zombie->node.data = zombie;
    fw64_node_set_mesh(&zombie->node, zombie->mesh);
    fw64_node_set_box_collider(&zombie->node, &zombie->collider);

    vec3_zero(&zombie->targetVelocity);
    zombie->previous_state = ZOMBIE_STATE_INACTIVE;
    zombie->state = ZOMBIE_STATE_INACTIVE;
    zombie->health = ZOMBIE_MAX_HEALTH;
    zombie->rotation = fw64_random_float_in_range(0.0f, 359.9f) * (M_PI / 180.0f);    
    vec3_set_all(&zombie->node.transform.scale, ZOMBIE_SCALE);
    fw64_node_update(&zombie->node);
}

void zombie_uninit(Zombie* zombie, fw64Allocator* allocator) {
    fw64_animation_controller_uninit(&zombie->animation_controller, allocator);
}

static void zombie_update_idle(Zombie* zombie) {
    return;
}

static void zombie_move(Zombie* zombie) {

    // rotate mesh to face direction of vel vector
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

    if(vec3_dot(&delta_vel, &delta_vel) < EPSILON)
        return;

    uint32_t mask = (uint32_t)(FW64_layer_obstacles | FW64_layer_tree | FW64_layer_wall);
    fw64IntersectMovingSphereQuery statics_query, dynamics_query;
    int hit_statics = fw64_level_moving_sphere_intersection( zombie->level,
                                                            &zombie->node.transform.position,
                                                            2.5f, &delta_vel, mask, &statics_query);
    mask = (uint32_t)ZOMBIE_LAYER;
    int hit_dynamics = fw64_level_moving_spheres_dynamic_intersection( zombie->level,
                                                                      &zombie->node.transform.position,
                                                                      0.5f, &delta_vel, mask, &dynamics_query);
    
    if(hit_dynamics && dynamics_query.count <= 1) { hit_dynamics = 0; }
    
    Vec3 collision_normal = {0.0f, 0.0f, 0.0f};
    
    if(hit_statics) {
        fw64_collision_get_normal_box_point(&statics_query.results[0].point,
                                    &statics_query.results[0].node->collider->bounding,
                                    &collision_normal);
    } else if (hit_dynamics) { // use node 1 for dynamics, collision 0 is always self
        vec3_subtract(&collision_normal,
                &dynamics_query.results[1].point,
                &dynamics_query.results[1].node->transform.position);
        vec3_normalize(&collision_normal);
    }

    float strength = fw64_fabsf(vec3_dot(&delta_vel, &collision_normal));
    vec3_add_and_scale(&delta_vel, &delta_vel, &collision_normal, strength);
    vec3_add(&zombie->node.transform.position, &zombie->node.transform.position, &delta_vel);   
    zombie_set_to_ground_height(zombie);
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

static void damage_player(Zombie* zombie) {
	// just for test
    return;
    
    uint32_t dyanmic_node_count = fw64_level_get_dynamic_node_count(zombie->level);
	Player* player = NULL;

	for (uint32_t i = 0; i < dyanmic_node_count; i++) {
		fw64Node* node = fw64_level_get_dynamic_node(zombie->level, i);
		if (node->layer_mask & FW64_layer_player) {
			player = (Player*)node->data;
			break;
		}
	}

	if (player)
	    player_take_damage(player, 15);
}

static void zombie_update_attack(Zombie* zombie) {
    static const float attack_radius = ZOMBIE_ATTACK_RANGE* 1.5f;
    if(zombie->animation_controller.current_time > (zombie->animation_controller.current_animation->total_time*0.50f)){
        if(!zombie->this_attack_hit) {
            Vec3 pos, target_pos;
            vec3_copy(&pos, &zombie->node.transform.position);
            pos.y = 0.0f;
            vec3_copy(&target_pos, &zombie->ai.target->position);
            target_pos.y = 0.0f;
            float dist_sq = vec3_distance_squared(&pos, &target_pos);
            if(dist_sq <= attack_radius*attack_radius) {
                Vec3 dir;
                vec3_subtract(&dir, &target_pos, &pos);
                vec3_normalize(&dir);
                Vec3 facing = {0.0f, 0.0f, 1.0f};
                quat_transform_vec3(&facing, &zombie->node.transform.rotation, &facing);
                if(vec3_dot(&dir, &facing) > 0) {
                    damage_player(zombie);
                    zombie->this_attack_hit = 1;
                }
            }
        }
    }
    if (zombie->animation_controller.state == FW64_ANIMATION_STATE_STOPPED) {
        // damage_player(zombie);
        if(zombie->health > 0) {
            zombie_ai_set_logic_state(&zombie->ai, ZLS_AGGRO);
            zombie_set_new_state(zombie, ZOMBIE_STATE_RUNNING);
        }
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

int zombie_update(Zombie* zombie) {
    fw64_animation_controller_update(&zombie->animation_controller, zombie->engine->time->time_delta);
    zombie_ai_update(&zombie->ai, zombie->engine->time->time_delta);
    // TODO: replace this series of polling ifs with an animation state change inside the ai state change
    if(zombie->ai.state == ZLS_IDLE) {
        if(zombie->state == ZOMBIE_STATE_RUNNING || zombie->state == ZOMBIE_STATE_WALKING)
            zombie_set_new_state(zombie, ZOMBIE_STATE_IDLE);
    } else if (zombie->ai.state == ZLS_ATTACK && zombie->health > 0) {
        if(zombie->state != ZOMBIE_STATE_ATTACK) {
            zombie_set_new_state(zombie, ZOMBIE_STATE_ATTACK);
            zombie->this_attack_hit = 0;
        }
    } else {
        Vec3 ref_zero = {0.0f, 0.0f, 0.0f};
        float speed_sq = vec3_distance_squared(&ref_zero, &zombie->ai.velocity.linear);

        if (zombie->state == ZOMBIE_STATE_IDLE) {
            zombie_set_new_state(zombie, ZOMBIE_STATE_WALKING);
        }
        if(speed_sq <= (ZOMBIE_WALK_SPEED*ZOMBIE_WALK_SPEED*1.1f)) {
            if(zombie->state == ZOMBIE_STATE_RUNNING)
                zombie_set_new_state(zombie, ZOMBIE_STATE_WALKING);
        } else if (zombie->state == ZOMBIE_STATE_WALKING){
            zombie_set_new_state(zombie, ZOMBIE_STATE_RUNNING);
        }
    }
    
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

        case ZOMBIE_STATE_ATTACK:
            zombie_update_attack(zombie);
        break;

        case ZOMBIE_STATE_INACTIVE:
        case ZOMBIE_STATE_DEAD:
            return 0;
        break;
    }
    return 1;
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

        case ZOMBIE_STATE_ATTACK:
            animation = zombie_animation_Attack;
            loop = 0;
            speed = 1.85f;
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

void zombie_set_to_ground_height(Zombie* zombie) {
    fw64RaycastHit raycast_hit;

    Vec3 ray_pos = zombie->node.transform.position;
    ray_pos.y = 1000.0f;
    Vec3 ray_dir = {0.0f, -1.0f, 0.0f};

    if (fw64_level_raycast(zombie->level, &ray_pos, &ray_dir, FW64_layer_ground, &raycast_hit)) {
        zombie->node.transform.position.y = raycast_hit.point.y;
    }
}