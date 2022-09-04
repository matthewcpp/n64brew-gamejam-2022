#include "weapon.h"

#include "assets.h"
#include "sound_bank_sounds.h"

#include <string.h>

void weapon_init(Weapon* weapon) {
    memset(weapon, 0, sizeof(Weapon));
}

void weapon_reomve_existing(Weapon* weapon, fw64AssetDatabase* assets, fw64Allocator* allocator) {
    if (weapon->mesh)
        fw64_mesh_delete(assets, weapon->mesh, allocator);
    if (weapon->casing)
        fw64_mesh_delete(assets, weapon->casing, allocator);
    if (weapon->muzzle_flash)
        fw64_mesh_delete(assets, weapon->muzzle_flash, allocator);
}

void weapon_init_ar15(Weapon* weapon, fw64AssetDatabase* assets, fw64Allocator* allocator) {
    weapon_reomve_existing(weapon, assets, allocator);

    weapon->type = WEAPON_TYPE_AR15;
    weapon->mesh = fw64_mesh_load(assets, FW64_ASSET_mesh_us_ar33, allocator);
    weapon->casing = fw64_mesh_load(assets, FW64_ASSET_mesh_9mm_round, allocator);;
    weapon->muzzle_flash = NULL;
    weapon->fire_rate = 0.333;
    weapon->gunshot_sound = sound_bank_sounds_rifle_shot_1;
    weapon->reload_sound = sound_bank_sounds_rifle_reload;
    weapon->crosshair = fw64_texture_create_from_image(fw64_image_load(assets, FW64_ASSET_image_crosshair, allocator), allocator);
    vec3_set(&weapon->default_position, 2.58f, -3.01f, -6.74f);
    vec3_set(&weapon->lowered_position, 2.58f, -9.25f, -6.74f);
    vec3_set_all(&weapon->default_scale, 0.1f);
    vec3_set(&weapon->ejection_port_pos, 2.8f, -2.13f, -5.74f);
}

void weapon_init_shotgun(Weapon* weapon, fw64AssetDatabase* assets, fw64Allocator* allocator) {
    weapon_reomve_existing(weapon, assets, allocator);

    weapon->type = WEAPON_TYPE_SHOTGUN;
    weapon->mesh = fw64_mesh_load(assets, FW64_ASSET_mesh_shotgun, allocator);
    weapon->casing = fw64_mesh_load(assets, FW64_ASSET_mesh_shotgun_shell, allocator);
    weapon->muzzle_flash = NULL;
    weapon->fire_rate = 1.6f;
    weapon->gunshot_sound = sound_bank_sounds_shotgun_shot_and_reload;
    weapon->reload_sound = sound_bank_sounds_shotgun_reload;
    weapon->crosshair = fw64_texture_create_from_image(fw64_image_load(assets, FW64_ASSET_image_crosshair, allocator), allocator);
    vec3_set(&weapon->default_position, 3.49f, -4.16f, -8.40f);
    vec3_set(&weapon->lowered_position, 3.49f, -14.0f, -8.40f);
    vec3_set_all(&weapon->default_scale, 0.02f);
    vec3_set(&weapon->ejection_port_pos, 4.54f, -2.64f, -6.41f);
}