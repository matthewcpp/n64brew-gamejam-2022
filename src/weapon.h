#pragma once

#include "framework64/allocator.h"
#include "framework64/asset_database.h"
#include "framework64/mesh.h"
#include "framework64/quat.h"
#include "framework64/transform.h"

typedef enum {
    WEAPON_TYPE_NONE,
    WEAPON_TYPE_AR15,
    WEAPON_TYPE_SHOTGUN,
    WEAPON_TYPE_UZI,
    WEAPON_COUNT
} WeaponType;

typedef struct {
    WeaponType type;
    fw64Mesh* mesh;
    fw64Mesh* casing;
    fw64Mesh* muzzle_flash;
    fw64Texture* crosshair;
    Vec3 default_position;
    Quat default_rotation;
    Vec3 default_scale;
    Vec3 lowered_position;
    Vec3 ejection_port_pos;
    int gunshot_sound;
    int reload_sound;
    Vec3 casing_pos;
    float fire_rate;
    uint32_t mag_size;
    uint32_t max_ammo_capacity;
    Quat recoil_rotation;
    Vec3 recoil_pos;
    float recoil_time;
} Weapon;

/** 
 * initializes the weapon to an empty state i.e WEAPON_TYPE_NONE
 * This should be called before any of the other weapon_init_xxx functions!
*/
void weapon_init(Weapon* weapon);
void weapon_init_none(Weapon* weapon, fw64AssetDatabase* assets, fw64Allocator* allocator);
void weapon_uninit(Weapon* weapon, fw64AssetDatabase* assets, fw64Allocator* allocator);
void weapon_init_ar15(Weapon* weapon, fw64AssetDatabase* assets, fw64Allocator* allocator);
void weapon_init_shotgun(Weapon* weapon, fw64AssetDatabase* assets, fw64Allocator* allocator);
void weapon_init_uzi(Weapon* weapon, fw64AssetDatabase* assets, fw64Allocator* allocator);
