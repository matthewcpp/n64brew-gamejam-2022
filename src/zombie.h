#pragma once

#include "framework64/engine.h"
#include "framework64/node.h"
#include "framework64/animation_data.h"
#include "framework64/animation_controller.h"

#include "level.h"
#include "weapon.h"
#include "behaviors.h"

#define ZOMBIE_MAX_HEALTH 3

typedef enum {
    ZOMBIE_STATE_INACTIVE,
    ZOMBIE_STATE_DEAD,
    ZOMBIE_STATE_IDLE,
    ZOMBIE_STATE_RUNNING,
    ZOMBIE_STATE_WALKING,
    ZOMBIE_STATE_HIT_REACTION,
    ZOMBIE_STATE_FALLING_DOWN,
    ZOMBIE_FLYING_BACK
    
} ZombieState;

typedef struct {
    fw64Engine* engine;
    fw64Mesh* mesh;
    fw64Level* level;
    fw64Node node;
    fw64Collider collider;
    fw64AnimationController animation_controller;
    fw64Transform* target;
    fw64Transform* targetPrevious;
    Vec3 targetVelocity;
    ZombieState previous_state;
    ZombieState state;
    int health;
    Vec3 velocity_linear; // linear
    float velocity_angular;
    float rotation; // current angular offset. radians about the y axis. stored to easily update
    float rotation_speed; // max rotation speed in rads;
    float max_speed_linear;
    float max_speed_angular;
    float max_accel_linear;
    float max_accel_angular;
    unsigned long long int active_bahaviors; // bitset of up to 64 unique behaviors. primarily used to deactivate behaviors.
    SteeringBehaviorData sb_data;
} Zombie;

void zombie_init(Zombie* zombie, fw64Engine* engine, fw64Level* level, fw64Mesh* mesh, fw64AnimationData* animation_data);
void zombie_update(Zombie* zombie);
void zombie_draw(Zombie* zombie);
void zombie_set_target(Zombie* zombie, fw64Transform* target);
void zombie_hit(Zombie* zombie, WeaponType weapon_type);
void zombie_set_new_state(Zombie* zombie, ZombieState new_state);
void zombie_set_behavior(Zombie* zombie, SteeringBehavior behavior);
void zombie_clear_behavior(Zombie* zombie, SteeringBehavior behavior);
void zombie_apply_behavior(Zombie* zombie, SteeringBehavior behavior);
void zombie_apply_active_behaviors(Zombie* zombie);

// temp
#define ZOMBIE_LAYER 2
