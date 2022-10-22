#include "zombie_logic.h"
#include "framework64/random.h"
#include "assets/layers.h"
#include "framework64/level.h"
#include "zombie.h"

void zombie_ai_init(ZombieAI* zombie_ai, fw64Level* level, fw64Collider* collider, fw64Transform* transform, fw64Transform* target) {	
	zombie_ai->level = level;
	zombie_ai->state = ZLS_INACTIVE;
	zombie_ai->active_bahaviors = SB_NONE;
	zombie_ai->collider = collider;
	zombie_ai->transform = transform;
	zombie_ai->target = target;
	fw64_transform_init(&zombie_ai->targetPrev);
	vec3_zero(&zombie_ai->velocity.linear);
	zombie_ai->velocity.angular = 0.0f;
	zombie_ai->maxVelocity.linear = 0.0f;
	zombie_ai->maxVelocity.angular = 0.0f;	
	steering_behavior_data_init(zombie_ai->level,
								zombie_ai->collider,
								zombie_ai->transform->position,
								zombie_ai->target->position,
								&zombie_ai->velocity.linear,
								0.0f,
								&zombie_ai->sb_data);
	zombie_ai->timer = 0.0f;
	zombie_ai->attack_cooldown = 0.0f;
	return;
}

static void zombie_ai_init_wander(ZombieAI* zombie_ai);
static void zombie_ai_init_meet(ZombieAI* zombie_ai);
static void zombie_ai_init_follow(ZombieAI* zombie_ai);
static void zombie_ai_init_lead(ZombieAI* zombie_ai);
static void zombie_ai_share_target(ZombieAI* zombie_ai);
static void zombie_ai_timer_reset(ZombieAI* zombie_ai, ZombieLogicState newState);
static int  zombie_ai_target_in_view(ZombieAI* zombie_ai, fw64Transform* target);

static void zombie_ai_behavior_set(ZombieAI* zombie_ai, SteeringBehavior behavior);
static void zombie_ai_behavior_clear(ZombieAI* zombie_ai, SteeringBehavior behavior);
static void zombie_ai_behavior_apply(ZombieAI* zombie_ai, SteeringBehavior behavior, float deltaTime);
static void zombie_ai_behavior_apply_all_active(ZombieAI* zombie_ai, float deltaTime);

static void zombie_ai_update_idle(ZombieAI* zombie_ai, float deltaTime);
static void zombie_ai_update_wander(ZombieAI* zombie_ai, float deltaTime);
static void zombie_ai_update_meet(ZombieAI* zombie_ai, float deltaTime);
static void zombie_ai_update_explore(ZombieAI* zombie_ai, float deltaTime);
static void zombie_ai_update_herd(ZombieAI* zombie_ai, float deltaTime);
static void zombie_ai_update_follow(ZombieAI* zombie_ai, float deltaTime);
static void zombie_ai_update_lead(ZombieAI* zombie_ai, float deltaTime);
static void zombie_ai_update_aggro(ZombieAI* zombie_ai, float deltaTime);
static void zombie_ai_update_attack(ZombieAI* zombie_ai, float deltaTime);

void zombie_ai_set_logic_state(ZombieAI* zombie_ai, ZombieLogicState newState) {
	zombie_ai->state = newState;
	zombie_ai_behavior_clear(zombie_ai, SB_ALL);
	zombie_ai_timer_reset(zombie_ai, newState);

	switch(zombie_ai->state) {
		case ZLS_INACTIVE:
			break;
		case ZLS_IDLE:			
			zombie_ai->maxVelocity.linear = 0.0f;
			zombie_ai->maxVelocity.angular = 0.0f;
			break;
		case ZLS_WANDER:
			zombie_ai->maxVelocity.linear = ZOMBIE_WALK_SPEED;
			zombie_ai->maxVelocity.angular = 1.0f;
			zombie_ai_init_wander(zombie_ai);
			break;
		case ZLS_MEET:
			zombie_ai->maxVelocity.linear = ZOMBIE_WALK_SPEED;
			zombie_ai->maxVelocity.angular = 1.0f;
			zombie_ai_init_meet(zombie_ai);
			break;
		case ZLS_EXPLORE:
			zombie_ai->maxVelocity.linear = ZOMBIE_WALK_SPEED;
			zombie_ai->maxVelocity.angular = 1.0f;
			zombie_ai_behavior_set(zombie_ai, SB_WANDER);
			zombie_ai_behavior_set(zombie_ai, SB_AVOID_OBSTACLE);
			break;
		case ZLS_HERD:
			zombie_ai->maxVelocity.linear = ZOMBIE_WALK_SPEED;
			zombie_ai->maxVelocity.angular = 1.0f;
			zombie_ai_behavior_set(zombie_ai, SB_FLOCK);
			zombie_ai_behavior_set(zombie_ai, SB_AVOID_OBSTACLE);
			break;
		case ZLS_FOLLOW:
			zombie_ai->maxVelocity.linear = ZOMBIE_WALK_SPEED;
			zombie_ai->maxVelocity.angular = 1.0f;
			zombie_ai_init_follow(zombie_ai);
			zombie_ai_behavior_set(zombie_ai, SB_FLOCK);
			zombie_ai_behavior_set(zombie_ai, SB_AVOID_OBSTACLE);
			break;
		case ZLS_LEAD:
			zombie_ai->maxVelocity.linear = ZOMBIE_WALK_SPEED;
			zombie_ai->maxVelocity.angular = 1.0f;
			zombie_ai_init_lead(zombie_ai);
			break;
		case ZLS_AGGRO:
			zombie_ai->maxVelocity.linear = ZOMBIE_RUN_SPEED;
			zombie_ai->maxVelocity.angular = 3.0f;
			float propagateTargetChance = 20.0f;
			float attemptPropagate = fw64_random_float_in_range(0.0f, 100.0f);
			if(attemptPropagate <= propagateTargetChance) {
				zombie_ai_share_target(zombie_ai);
			}
			zombie_ai_behavior_set(zombie_ai, SB_SEEK);
			zombie_ai_behavior_set(zombie_ai, SB_AVOID_OBSTACLE);
			break;
		case ZLS_ATTACK:
			zombie_ai->maxVelocity.linear = 0.0f;
			zombie_ai->maxVelocity.angular = 0.0f;
			break;
		default:
			break;
	}
	return;
}
int zombie_get_logic_state(ZombieAI* zombie_ai) {
	return zombie_ai->state;
}

void zombie_ai_set_target(ZombieAI* zombie_ai, fw64Transform* target) {
	zombie_ai->target = target;
	
	vec3_copy(&zombie_ai->targetPrev.position, &zombie_ai->target->position);
	vec3_copy(&zombie_ai->targetPrev.scale, &zombie_ai->target->scale);

	zombie_ai->targetPrev.rotation.x = zombie_ai->target->rotation.x;
	zombie_ai->targetPrev.rotation.y = zombie_ai->target->rotation.y;
	zombie_ai->targetPrev.rotation.z = zombie_ai->target->rotation.z;
	zombie_ai->targetPrev.rotation.w = zombie_ai->target->rotation.w;
	
	return;
}
fw64Transform* zombie_ai_get_target(ZombieAI* zombie_ai) {
	return zombie_ai->target;
}

// returns 1 if target is in view, 0 if not
static int zombie_ai_target_in_view(ZombieAI* zombie_ai, fw64Transform* target) {
	if(target == NULL) {
		return 0;
	}
	Vec3 zed_pos, target_pos;
	vec3_copy(&zed_pos, &zombie_ai->transform->position);
	zed_pos.y = 1;
	vec3_copy(&target_pos, &target->position);
	target_pos.y = 1;
	float dist_sq = vec3_distance_squared(&zed_pos, &target_pos);
	
	if(dist_sq < (ZOMBIE_VISION_DISTANCE*ZOMBIE_VISION_DISTANCE)) {
		return 1;
	}

	return 0;
}




void zombie_ai_update(ZombieAI* zombie_ai, float deltaTime) {
	switch(zombie_ai->state) {
		case ZLS_INACTIVE:
			return;
		case ZLS_IDLE:
			zombie_ai_update_idle(zombie_ai, deltaTime);
			break;
		case ZLS_WANDER:
			zombie_ai_update_wander(zombie_ai, deltaTime);
			break;
		case ZLS_MEET:
			zombie_ai_update_meet(zombie_ai, deltaTime);
			break;
		case ZLS_EXPLORE:
			zombie_ai_update_explore(zombie_ai, deltaTime);
			break;
		case ZLS_HERD:
			zombie_ai_update_herd(zombie_ai, deltaTime);
			break;
		case ZLS_FOLLOW:
			zombie_ai_update_follow(zombie_ai, deltaTime);
			break;
		case ZLS_LEAD:
			zombie_ai_update_lead(zombie_ai, deltaTime);
			break;
		case ZLS_AGGRO:
			zombie_ai_update_aggro(zombie_ai, deltaTime);
			break;
		case ZLS_ATTACK:
			zombie_ai_update_attack(zombie_ai, deltaTime);
			break;
		default:
			break;
	}
	
	//trigger aggro if too close
	if((zombie_ai->state != ZLS_AGGRO) && (zombie_ai->state != ZLS_ATTACK)) {
		int sighted = zombie_ai_target_in_view(zombie_ai, zombie_ai->target);
		if(sighted) {
			zombie_ai_set_logic_state(zombie_ai, ZLS_AGGRO);
		}
	}
}

static void zombie_ai_init_wander(ZombieAI* zombie_ai) {
	zombie_ai->sb_data.position = zombie_ai->transform->position;
	zombie_ai->sb_data.targetPosition = zombie_ai->transform->position;
	Vec3 target;
	vec3_set(&target, fw64_random_float_in_range(-1.0, 1.0), 0.0f, fw64_random_float_in_range(-1.0, 1.0));
	vec3_normalize(&target);
	vec3_add_and_scale( &zombie_ai->sb_data.targetPosition,
						&zombie_ai->sb_data.targetPosition,
						&target,
						fw64_random_float_in_range(5.0f, 45.0f));
	zombie_ai_behavior_set(zombie_ai, SB_WANDER);
	zombie_ai_behavior_set(zombie_ai, SB_AVOID_OBSTACLE);
}
static void zombie_ai_init_meet(ZombieAI* zombie_ai) {}
static void zombie_ai_init_follow(ZombieAI* zombie_ai) {}
static void zombie_ai_init_lead(ZombieAI* zombie_ai) {}
static void zombie_ai_share_target(ZombieAI* zombie_ai) {}

static void zombie_ai_timer_reset(ZombieAI* zombie_ai, ZombieLogicState newState) {
	switch(newState) {
		case ZLS_IDLE:
		zombie_ai->timer = AI_TIMER_LENGTH_IDLE;
			break;
		case ZLS_WANDER:
		zombie_ai->timer = AI_TIMER_LENGTH_WANDER;
			break;
		case ZLS_MEET:
		zombie_ai->timer = AI_TIMER_LENGTH_MEET;
			break;
		case ZLS_EXPLORE:
		zombie_ai->timer = AI_TIMER_LENGTH_EXPLORE;
			break;
		case ZLS_HERD:
		zombie_ai->timer = AI_TIMER_LENGTH_HERD;
			break;
		case ZLS_FOLLOW:
		zombie_ai->timer = AI_TIMER_LENGTH_FOLLOW;
			break;
		case ZLS_LEAD:
		zombie_ai->timer = AI_TIMER_LENGTH_LEAD;
			break;
		case ZLS_AGGRO:
		zombie_ai->timer = AI_TIMER_LENGTH_AGGRO;
			break;
		case ZLS_INACTIVE: /* fall through */
		default:
			break;
	}
	return;
}

static void zombie_ai_behavior_set(ZombieAI* zombie_ai, SteeringBehavior behavior) {
    zombie_ai->active_bahaviors |= behavior;
}

static void zombie_ai_behavior_clear(ZombieAI* zombie_ai, SteeringBehavior behavior) {
    zombie_ai->active_bahaviors &= ~(behavior);
}

static void zombie_ai_behavior_apply(ZombieAI* zombie_ai, SteeringBehavior behavior, float deltaTime) {
    
	//TODO: this is a quick fix so wander doesn't just chase the player.
	Vec3 targetPos;
	if(zombie_ai->state == ZLS_WANDER) {
		vec3_copy(&targetPos, &zombie_ai->sb_data.targetPosition);
	} else {
		vec3_copy(&targetPos, &zombie_ai->target->position);
	}

	steering_behavior_data_init(zombie_ai->level,
								zombie_ai->collider,
								zombie_ai->transform->position,
								targetPos,
								&zombie_ai->velocity.linear,
								0.0f,
								&zombie_ai->sb_data);    
    //zombie_ai->sb_data.position.y = 0.0f;
    //zombie_ai->sb_data.targetPosition.y = 0.0f;

    switch(behavior) {
        case SB_SEEK: {          
            steering_seek(1.0f, &zombie_ai->sb_data);            
            break;
		}
        case SB_FLEE: {
            steering_flee(1.0f, &zombie_ai->sb_data);
            break;
		}
        case SB_ARRIVE: {
            steering_arrive(20.0f, 10.0f, 1.0f, &zombie_ai->sb_data);
            break;
		}
        case SB_PURSUE: {
			Vec3 targetVelocity;
            vec3_subtract(&targetVelocity, &zombie_ai->target->position, &zombie_ai->targetPrev.position);
            vec3_copy(&zombie_ai->targetPrev.position, &zombie_ai->target->position);
            steering_pursue(&targetVelocity, 1.0f, &zombie_ai->sb_data);
            break;
		}
        case SB_EVADE: {
			Vec3 targetVelocity;
            vec3_subtract(&targetVelocity, &zombie_ai->target->position, &zombie_ai->targetPrev.position);
            vec3_copy(&zombie_ai->targetPrev.position, &zombie_ai->target->position);
            steering_evade(&targetVelocity, 1.0f, &zombie_ai->sb_data);
            break;
		}
		case SB_WANDER: {
			steering_wander(1.0f, &zombie_ai->sb_data);            
            break;
		}
		case SB_AVOID_OBSTACLE: /* fall through */
		case SB_AVOID_COLLISION: {
			steering_avoid_collision(1.0f, &zombie_ai->sb_data);
			break;
		}
        default: break;
    }
    vec3_add(&zombie_ai->velocity.linear, &zombie_ai->velocity.linear, &zombie_ai->sb_data.linearAccel);
	zombie_ai->velocity.linear.y = 0.0f;
	zombie_ai->velocity.angular += zombie_ai->sb_data.angularAccel;
}

static void zombie_ai_behavior_apply_all_active(ZombieAI* zombie_ai, float deltaTime) {
    for(int i = 0; i < SB_TOTAL_BEHAVIORS; i++) {
        unsigned long long int check_bahavior = (1 << i);
        if(check_bahavior & zombie_ai->active_bahaviors) {
			zombie_ai_behavior_apply(zombie_ai, (SteeringBehavior)check_bahavior, deltaTime);
        }
    }
	Vec3 ref_zero;
    vec3_zero(&ref_zero);
    if(vec3_distance_squared(&ref_zero, &zombie_ai->velocity.linear) > (zombie_ai->maxVelocity.linear * zombie_ai->maxVelocity.linear)) {
        vec3_normalize(&zombie_ai->velocity.linear);
        vec3_scale(&zombie_ai->velocity.linear, &zombie_ai->velocity.linear, zombie_ai->maxVelocity.linear);
    }
}

static void zombie_ai_update_idle(ZombieAI* zombie_ai, float deltaTime)	{
	if(deltaTime >= zombie_ai->timer) {
		if(fw64_random_int() % 2) {
			zombie_ai_timer_reset(zombie_ai, ZLS_IDLE);
		} else {
			zombie_ai_set_logic_state(zombie_ai, ZLS_WANDER);
		}		
	} else {
		zombie_ai->timer -= deltaTime;
	}
}

static void zombie_ai_update_wander(ZombieAI* zombie_ai, float deltaTime) {
	
	//if it takes too long, give up
	if(deltaTime >= zombie_ai->timer) {
		if(fw64_random_int() % 2) {
			zombie_ai_timer_reset(zombie_ai, ZLS_WANDER);
		} else {
			zombie_ai_set_logic_state(zombie_ai, ZLS_IDLE);
		}		
	} else {
		zombie_ai->timer -= deltaTime;
	}

	// try to move to our random destination
	zombie_ai_behavior_apply_all_active(zombie_ai, deltaTime);

	// if we arrived, stop wandering
	Vec3 ref_zero = {0.0f, 0.0f, 0.0f};
	float dist_sq = vec3_distance_squared(&ref_zero, &zombie_ai->velocity.linear);
	if(dist_sq < 0.1f) {
		zombie_ai_set_logic_state(zombie_ai, ZLS_IDLE);
	}
}

static void zombie_ai_update_meet(ZombieAI* zombie_ai, float deltaTime) {}
static void zombie_ai_update_explore(ZombieAI* zombie_ai, float deltaTime) {}
static void zombie_ai_update_herd(ZombieAI* zombie_ai, float deltaTime) {}
static void zombie_ai_update_follow(ZombieAI* zombie_ai, float deltaTime) {}
static void zombie_ai_update_lead(ZombieAI* zombie_ai, float deltaTime) {}

static void zombie_ai_update_aggro(ZombieAI* zombie_ai, float deltaTime) {
	
	if(vec3_distance_squared(&zombie_ai->transform->position, &zombie_ai->target->position) < ZOMBIE_ATTACK_RANGE * ZOMBIE_ATTACK_RANGE) {
		if(zombie_ai->attack_cooldown <= 0.0f) {
			// attack!
			zombie_ai_set_logic_state(zombie_ai, ZLS_ATTACK);
			zombie_ai->attack_cooldown = ZOMBIE_ATTACK_COOLDOWN;
			return;
		} else {
			zombie_ai->attack_cooldown -= deltaTime;
		}
	} else {
		//periodically double check that the target is close enough to see
		if(deltaTime >= zombie_ai->timer) {
			if(zombie_ai_target_in_view(zombie_ai, zombie_ai->target)) {
				zombie_ai_timer_reset(zombie_ai, ZLS_AGGRO);
			} else {
				zombie_ai_set_logic_state(zombie_ai, ZLS_IDLE);
				return;
			}
		} else {
			zombie_ai->timer -= deltaTime;
		}
		
	zombie_ai_behavior_apply_all_active(zombie_ai, deltaTime);
	}
}


static void zombie_ai_update_attack(ZombieAI* zombie_ai, float deltaTime) {
		if(zombie_ai->attack_cooldown <= 0.0f) {
			zombie_ai->attack_cooldown = 0.0f;
			//zombie_ai_set_logic_state(zombie_ai, ZLS_AGGRO);			
			return;
		} else {
			//give animation time to ply out and give player time to back away
			zombie_ai->attack_cooldown -= deltaTime;
		}
}