#pragma once

#include "framework64/engine.h"
#include "framework64/node.h"
#include "framework64/animation_data.h"
#include "framework64/animation_controller.h"

#include "level.h"
#include "weapon.h"

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
    fw64AnimationController animation_controller;
    fw64Transform* target;
    ZombieState previous_state;
    ZombieState state;
    int health;
} Zombie;

void zombie_init(Zombie* zombie, fw64Engine* engine, fw64Level* level, fw64Mesh* mesh, fw64AnimationData* animation_data);
void zombie_update(Zombie* zombie);
void zombie_draw(Zombie* zombie);
void zombie_set_target(Zombie* zombie, fw64Transform* target);
void zombie_hit(Zombie* zombie, WeaponType weapon_type);
void zombie_set_new_state(Zombie* zombie, ZombieState new_state);

// temp
#define ZOMBIE_LAYER 2
