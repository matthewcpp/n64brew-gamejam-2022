#pragma once

#include "framework64/allocator.h"
#include "framework64/asset_database.h"
#include "framework64/mesh.h"
#include "framework64/quat.h"
#include "framework64/transform.h"

typedef enum {
    WEAPON_TYPE_NONE,
    WEAPON_TYPE_AR15,
    WEAPON_TYPE_SHOTGUN
} WeaponType;

typedef struct {
    WeaponType type;
    fw64Mesh* mesh;
    fw64Mesh* casing;
    fw64Mesh* muzzle_flash;
    fw64Texture* crosshair;
    Vec3 default_position;
    Vec3 default_scale;
    int gunshot_sound;
    Vec3 casing_pos;
    float fire_rate;
    int mag_size;
    float recoil_angle;
    Vec3 recoil_pos;
} Weapon;

void weapon_init(Weapon* weapon);
void weapon_init_ar15(Weapon* weapon, fw64AssetDatabase* assets, fw64Allocator* allocator);
void weapon_init_shotgun(Weapon* weapon, fw64AssetDatabase* assets, fw64Allocator* allocator);
