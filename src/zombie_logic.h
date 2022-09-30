#pragma once

/** \file zombie_logic.h */

#include "behaviors.h"
#include "framework64/level.h"
#include "framework64/transform.h"

#define AI_TIMER_LENGTH_IDLE 	fw64_random_float_in_range(1.0f, 30.0f)
#define AI_TIMER_LENGTH_WANDER 	30.0f
#define AI_TIMER_LENGTH_MEET 	30.0f
#define AI_TIMER_LENGTH_EXPLORE fw64_random_float_in_range(10.0f, 60.0f)
#define AI_TIMER_LENGTH_HERD 	10.0f
#define AI_TIMER_LENGTH_FOLLOW 	10.0f
#define AI_TIMER_LENGTH_LEAD 	15.0f
#define AI_TIMER_LENGTH_AGGRO 	15.0f

#define ZOMBIE_WALK_SPEED 2.5f
#define ZOMBIE_RUN_SPEED 15.0f
#define ZOMBIE_VISION_DISTANCE 45.0f
#define ZOMBIE_ATTACK_RANGE 8.0f
#define ZOMBIE_ATTACK_COOLDOWN 1.0f

typedef enum {
	ZLS_INACTIVE,	// special state, reserved for zombie that is not in use
	ZLS_IDLE,		// waiting
	ZLS_WANDER,		// move to random location in view
	ZLS_MEET,		// move towards random ally in view
	ZLS_EXPLORE,	// move aimlessly for some amount of time
	ZLS_HERD,		// stick with nearby allies
	ZLS_FOLLOW,		// stick with nearby allies, strongly influenced by a specific "leader"
	ZLS_LEAD,		// ignore nearby allies, restrict behaviors to wander, explore, and aggro
	ZLS_AGGRO,		// move towards player location
	ZLS_ATTACK		// swing or bite
} ZombieLogicState;

typedef struct {
	fw64Level* level; // for collision data with environment and dynamic nodes
	ZombieLogicState state;
    unsigned long long int active_bahaviors; // bitset of up to 64 unique behaviors. primarily used to deactivate behaviors.
    SteeringBehaviorData sb_data;
	fw64Collider* collider;
	fw64Transform* transform;
	fw64Transform* target;
	fw64Transform targetPrev;
	struct { 
		Vec3 linear;
		float angular;
	} velocity;
	struct { 
		float linear;
		float angular;
	} maxVelocity;
	float timer;
	float attack_cooldown;
} ZombieAI;

void zombie_ai_init(ZombieAI* zombie_ai, fw64Level* level, fw64Collider* collider, fw64Transform* transform, fw64Transform* target);
void zombie_ai_update(ZombieAI* zombie_ai, float deltaTime);
void zombie_ai_set_logic_state(ZombieAI* zombie_ai, ZombieLogicState newState);
int zombie_ai_get_logic_state(ZombieAI* zombie_ai);
void zombie_ai_set_target(ZombieAI* zombie_ai, fw64Transform* target);
fw64Transform* zombie_ai_get_target(ZombieAI* zombie_ai);
