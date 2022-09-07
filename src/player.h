#pragma once

#include "framework64/engine.h"
#include "framework64/mesh.h"
#include "framework64/quat.h"
#include "framework64/scene.h"
#include "framework64/vec3.h"
#include "fps_camera.h"

#include "level.h"
#include "weapon_controller.h"
#include "ray.h"
#include "mapped_input.h"

typedef struct {
    InputMapping input_map;
    fw64FpsCamera camera;
    fw64Camera weapon_camera;
    Weapon weapon;
    WeaponController weapon_controller;
    Ray aim;    
    fw64Engine* engine;
    fw64Level* level;

} Player;

void player_init(Player* player, fw64Engine* engine, fw64Level* level);
void player_update(Player* player);
void player_draw(Player* player);
void player_draw_weapon(Player* player);

void player_set_weapon(Player* player, WeaponType weapon_type);