#pragma once

#include "framework64/engine.h"
#include "framework64/mesh.h"
#include "framework64/quat.h"
#include "framework64/scene.h"
#include "framework64/vec3.h"
#include "fps_camera.h"

#include "weapon_controller.h"

typedef struct {
    fw64FpsCamera camera;
    fw64Camera weapon_camera;
    Weapon weapon;
    WeaponController weapon_controller;

    fw64Engine* engine;
    fw64Scene* scene;

} Player;

void player_init(Player* player, fw64Engine* engine, fw64Scene* scene);
void player_update(Player* player);
void player_draw(Player* player);
void player_draw_weapon(Player* player);

void player_set_weapon(Player* player, WeaponType weapon_type);