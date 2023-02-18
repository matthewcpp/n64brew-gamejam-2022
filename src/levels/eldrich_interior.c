#include "levels/eldrich_interior.h"

#include "assets/assets.h"
#include "assets/scene_1_door.h"

static fw64Scene* load_scene(EldrichLevel* level);

void eldrich_interior_level_init(EldrichLevel* level, fw64Engine* engine, GameData* game_data, fw64Allocator* state_allocator) {
    level_base_init(&level->base, engine, game_data, state_allocator);
    fw64Scene* scene = load_scene(level);

    player_add_ammo(&level->base.player, WEAPON_TYPE_1911, 50);
    player_add_ammo(&level->base.player, WEAPON_TYPE_MP5, 50);
    player_add_ammo(&level->base.player, WEAPON_TYPE_SHOTGUN, 50);
    player_add_ammo(&level->base.player, WEAPON_TYPE_UZI, 50);

        fw64Node* player_spawn_node = fw64_scene_get_node(scene, FW64_scene_1_door_node_Player_Start);
    player_set_position(&level->base.player, &player_spawn_node->transform.position);
}

static fw64Scene* load_scene(EldrichLevel* level) {
    fw64LevelChunkInfo info;
    fw64_level_chunk_info_init(&info);

    info.scene_id = FW64_ASSET_scene_1_door;
    info.allocator = level->base.allocator;
    return fw64_level_load_chunk(&level->base.level, &info)->scene;
}

void eldrich_interior_level_uninit(EldrichLevel* level) {
    level_base_uninit(&level->base);
}

void eldrich_interior_level_update(EldrichLevel* level) {
    level_base_update(&level->base);
}

void eldrich_interior_level_draw(EldrichLevel* level) {
    fw64Renderer* renderer = level->base.engine->renderer;
    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES,  FW64_RENDERER_FLAG_CLEAR);
    player_draw(&level->base.player);
    player_draw_weapon(&level->base.player);
    player_draw_damage(&level->base.player);
    ui_draw(&level->base.ui);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}