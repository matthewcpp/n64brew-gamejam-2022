#include "weapon.h"

#include "assets.h"
#include "sound_bank_sounds.h"

#include <string.h>

void weapon_init(Weapon* weapon) {
    memset(weapon, 0, sizeof(Weapon));
}

void weapon_init_ar15(Weapon* weapon, fw64AssetDatabase* assets, fw64Allocator* allocator) {
    weapon->type = WEAPON_TYPE_AR15;
    weapon->mesh = fw64_mesh_load(assets, FW64_ASSET_mesh_us_ar33, allocator);
    weapon->casing = NULL;
    weapon->muzzle_flash = NULL;
    weapon->fire_rate = 0.333;
    weapon->gunshot_sound = sound_bank_sounds_rifle_shot_1;
    weapon->crosshair = fw64_texture_create_from_image(fw64_image_load(assets, FW64_ASSET_image_crosshair, allocator), allocator);
    vec3_set(&weapon->default_position, 2.58f, -3.01f, -6.74f);
    vec3_set_all(&weapon->default_scale, 0.1f);
}

void weapon_init_shotgun(Weapon* weapon, fw64AssetDatabase* assets, fw64Allocator* allocator) {
    weapon->type = WEAPON_TYPE_SHOTGUN;
    weapon->mesh = fw64_mesh_load(assets, FW64_ASSET_mesh_shotgun, allocator);
    weapon->casing = NULL;
    weapon->muzzle_flash = NULL;
    weapon->fire_rate = 1.0f;
    weapon->gunshot_sound = sound_bank_sounds_rifle_shot_2;
    weapon->crosshair = fw64_texture_create_from_image(fw64_image_load(assets, FW64_ASSET_image_crosshair, allocator), allocator);
    vec3_set(&weapon->default_position, 3.49f, -4.16f, -8.40f);
    vec3_set_all(&weapon->default_scale, 0.02f);
}