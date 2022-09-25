#include "weapon.h"

#include "assets/assets.h"
#include "assets/sound_bank_sounds.h"

#include <string.h>

WeaponInfo weapon_infos[WEAPON_COUNT - 1];

static void init_ar15_info(WeaponInfo* weapon) {
    weapon->type = WEAPON_TYPE_AR15;
    weapon->fire_rate = 0.333;
    weapon->gunshot_sound = sound_bank_sounds_rifle_shot_1;
    weapon->reload_sound = sound_bank_sounds_rifle_reload;
    
    vec3_set(&weapon->default_position, 2.58f, -3.01f, -6.74f);
    vec3_set(&weapon->lowered_position, 2.58f, -9.25f, -6.74f);
    vec3_set_all(&weapon->default_scale, 0.1f);
    quat_ident(&weapon->default_rotation);

    vec3_set(&weapon->ejection_port_pos, 2.8f, -2.13f, -5.74f);

    vec3_set(&weapon->recoil_pos, 2.58f, -3.01f, -5.74f);
    quat_ident(&weapon->recoil_rotation);
    weapon->recoil_time = 0.10f;

    weapon->mag_size = 30;
    weapon->max_additional_rounds = 180;
}

static void init_shotgun_info(WeaponInfo* weapon){
    weapon->type = WEAPON_TYPE_SHOTGUN;
    weapon->fire_rate = 1.6f;
    weapon->gunshot_sound = sound_bank_sounds_shotgun_shot_and_reload;
    weapon->reload_sound = sound_bank_sounds_shotgun_reload;
    
    vec3_set(&weapon->default_position, 3.49f, -4.16f, -8.40f);
    vec3_set(&weapon->lowered_position, 3.49f, -14.0f, -8.40f);
    vec3_set_all(&weapon->default_scale, 0.02f);
    quat_ident(&weapon->default_rotation);

    vec3_set(&weapon->ejection_port_pos, 4.54f, -2.64f, -6.41f);

    vec3_set(&weapon->recoil_pos, 3.49f, -4.8f, -5.0f);
    quat_from_euler(&weapon->recoil_rotation, 2.5f, 0.0f, 5.0f);
    weapon->recoil_time = 0.25f;

    weapon->mag_size = 8;
    weapon->max_additional_rounds = 40;
}

static void init_uzi_info(WeaponInfo* weapon) {
    weapon->type = WEAPON_TYPE_UZI;
    weapon->fire_rate = 0.15;

    weapon->gunshot_sound = sound_bank_sounds_rifle_shot_2;
    weapon->reload_sound = sound_bank_sounds_rifle_reload;


    vec3_set(&weapon->default_position, 1.966, -2.44f, -4.87f);
    vec3_set(&weapon->lowered_position, 1.96f, -4.82f, -4.87f);
    vec3_set_all(&weapon->default_scale, 0.01f);
    quat_ident(&weapon->default_rotation);

    vec3_set(&weapon->ejection_port_pos, 2.6f, -1.983f, -5.241f);

    vec3_set(&weapon->recoil_pos, 1.966, -2.44f, -4.545);
    quat_ident(&weapon->recoil_rotation);
    weapon->recoil_time = 0.12f;

    weapon->mag_size = 32;
    weapon->max_additional_rounds = 320;

}

WeaponInfo* weapon_get_info(WeaponType weapon_type) {
    return &weapon_infos[weapon_type];
}

void init_weapon_info() {
    memset(&weapon_infos[WEAPON_TYPE_NONE], 0 , sizeof(WeaponInfo));
    init_ar15_info(weapon_get_info(WEAPON_TYPE_AR15));
    init_ar15_info(weapon_get_info(WEAPON_TYPE_SHOTGUN));
    init_ar15_info(weapon_get_info(WEAPON_TYPE_UZI));
}



void weapon_init(Weapon* weapon) {
    memset(weapon, 0, sizeof(Weapon));
}

static void weapon_reomve_existing(Weapon* weapon, fw64AssetDatabase* assets, fw64Allocator* allocator) {
    if (weapon->mesh)
        fw64_mesh_delete(assets, weapon->mesh, allocator);
    if (weapon->casing)
        fw64_mesh_delete(assets, weapon->casing, allocator);
    if (weapon->muzzle_flash)
        fw64_mesh_delete(assets, weapon->muzzle_flash, allocator);
    if (weapon->crosshair) {
        fw64_image_delete(assets, fw64_texture_get_image(weapon->crosshair), allocator);
        fw64_texture_delete(weapon->crosshair, allocator);
    }
}

void weapon_uninit(Weapon* weapon, fw64AssetDatabase* assets, fw64Allocator* allocator) {
    weapon_reomve_existing(weapon, assets, allocator);
}

void weapon_init_none(Weapon* weapon, fw64AssetDatabase* assets, fw64Allocator* allocator) {
    weapon_reomve_existing(weapon, assets, allocator);
    memset(weapon, 0, sizeof(Weapon));
}

/** TODO: this should probably be controlled in export? */
static void clamp_textures(fw64Mesh* mesh) {
    int primitive_count = fw64_mesh_get_primitive_count(mesh);
    for (int i = 0; i < primitive_count; i++) {
        fw64Material* material = fw64_mesh_get_material_for_primitive(mesh, i);
        fw64Texture* texture = fw64_material_get_texture(material);
        fw64_texture_set_wrap_mode(texture, FW64_TEXTURE_WRAP_CLAMP, FW64_TEXTURE_WRAP_CLAMP);
    }
}

void weapon_init_ar15(Weapon* weapon, fw64AssetDatabase* assets, fw64Allocator* allocator) {
    weapon_reomve_existing(weapon, assets, allocator);

    weapon->mesh = fw64_mesh_load(assets, FW64_ASSET_mesh_us_ar33, allocator);
    weapon->casing = fw64_mesh_load(assets, FW64_ASSET_mesh_9mm_round, allocator);
    weapon->muzzle_flash = fw64_mesh_load(assets, FW64_ASSET_mesh_ar15_muzzle_flash, allocator);
    weapon->crosshair = fw64_texture_create_from_image(fw64_image_load(assets, FW64_ASSET_image_crosshair, allocator), allocator);
    clamp_textures(weapon->muzzle_flash);

    weapon->info = &weapon_infos[WEAPON_TYPE_AR15];
}

void weapon_init_shotgun(Weapon* weapon, fw64AssetDatabase* assets, fw64Allocator* allocator) {
    weapon_reomve_existing(weapon, assets, allocator);

    weapon->mesh = fw64_mesh_load(assets, FW64_ASSET_mesh_shotgun, allocator);
    weapon->casing = fw64_mesh_load(assets, FW64_ASSET_mesh_shotgun_shell, allocator);
    weapon->muzzle_flash = fw64_mesh_load(assets, FW64_ASSET_mesh_shotgun_muzzle_flash, allocator);
    weapon->crosshair = fw64_texture_create_from_image(fw64_image_load(assets, FW64_ASSET_image_crosshair, allocator), allocator);
    clamp_textures(weapon->muzzle_flash);

    weapon->info = &weapon_infos[WEAPON_TYPE_SHOTGUN];
}

void weapon_init_uzi(Weapon* weapon, fw64AssetDatabase* assets, fw64Allocator* allocator) {
    weapon_reomve_existing(weapon, assets, allocator);

    weapon->mesh = fw64_mesh_load(assets, FW64_ASSET_mesh_uzi, allocator);
    weapon->casing = fw64_mesh_load(assets, FW64_ASSET_mesh_9mm_round, allocator);
    weapon->muzzle_flash = fw64_mesh_load(assets, FW64_ASSET_mesh_uzi_muzzle_flash, allocator);
    weapon->crosshair = fw64_texture_create_from_image(fw64_image_load(assets, FW64_ASSET_image_crosshair, allocator), allocator);
    clamp_textures(weapon->muzzle_flash);

    weapon->info = &weapon_infos[WEAPON_TYPE_UZI];
}