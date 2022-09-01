#pragma once

#include "framework64/engine.h"
#include "framework64/mesh.h"
#include "framework64/quat.h"
#include "framework64/scene.h"
#include "framework64/vec3.h"
#include "fps_camera.h"

typedef struct {
    fw64Mesh* mesh;
    Vec3 position;
    Quat rotation;
    Vec3 scale;
    fw64Texture* crosshair_sprite;
} Weapon;

typedef struct {
    fw64FpsCamera camera;
    fw64Camera weapon_camera;
    fw64Transform weapon_transform;
    Weapon* weapon;

    fw64Engine* engine;
    fw64Scene* scene;

} Player;

void player_init(Player* player, fw64Engine* engine, fw64Scene* scene);
void player_update(Player* player);
void player_draw(Player* player);
void player_draw_weapon(Player* player);

void player_set_weapon(Player* player, Weapon* weapon);