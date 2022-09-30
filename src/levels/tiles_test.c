#include "tiles_test.h"

#include "assets/assets.h"

#include "framework64/random.h"

#define TILE_COUNT 6

int tile_scenes[TILE_COUNT] = {
    FW64_ASSET_scene_tile_arches,
    FW64_ASSET_scene_tile_cubes,
    FW64_ASSET_scene_tile_trees,
    FW64_ASSET_scene_tile_cylinders,
    FW64_ASSET_scene_tile_spheres,
    FW64_ASSET_scene_tile_torus
};

#define BUMP_ALLOCATOR_SIZE (16 * 1024)
#define TILE_SIZE 100.0f

static void tiles_test_load_tile(TilesTestLevel* level, int index, Vec3* pos);
static void tiles_test_load_next_row(TilesTestLevel* level);

void tiles_test_level_init(TilesTestLevel* level, fw64Engine* engine) {
    level->engine = engine;
    level->next_row_pos = 0.0f;
    level->handle_index = 0;

    fw64_level_init(&level->level, engine);

    for (int i = 0; i < ACTIVE_TILE_COUNT; i++) {
        level->chunk_handles[i] = FW64_LEVEL_INVALID_CHUNK_HANDLE;
        fw64_bump_allocator_init(&level->allocators[i], BUMP_ALLOCATOR_SIZE);
    }

    projectile_controller_init(&level->projectile_controller, &level->level);
    player_init(&level->player, engine, &level->level, &level->projectile_controller, fw64_default_allocator());
    player_set_weapon(&level->player, WEAPON_TYPE_UZI);
    Vec3 starting_pos = {0.0f, 0.0f, 50.0f};
    player_set_position(&level->player, &starting_pos);
    level->player_previous_z = starting_pos.z;

    tiles_test_load_next_row(level);
    tiles_test_load_next_row(level);
    level->next_row_trigger = -TILE_SIZE / 2.0f;

    fw64_renderer_set_clear_color(engine->renderer, 147, 204, 234);
}

void tiles_test_load_next_row(TilesTestLevel* level) {
    Vec3 pos = {-TILE_SIZE, 0.0f, level->next_row_pos};
    tiles_test_load_tile(level, level->handle_index++, &pos);

    pos.x = 0.0f;
    tiles_test_load_tile(level, level->handle_index++, &pos);

    pos.x = TILE_SIZE;
    tiles_test_load_tile(level, level->handle_index++, &pos);

    level->next_row_pos -= TILE_SIZE;
    level->next_row_trigger -= TILE_SIZE;

    if (level->handle_index >= ACTIVE_TILE_COUNT)
        level->handle_index = 0;
}

void tiles_test_load_tile(TilesTestLevel* level, int index, Vec3* pos) {
    // eject previous chunk in this index
    fw64_level_unload_chunk(&level->level, level->chunk_handles[index]);
    fw64_bump_allocator_reset(&level->allocators[index]);

    fw64LevelChunkInfo info;
    fw64_level_chunk_info_init(&info);
    info.scene_id = tile_scenes[fw64_random_int_in_range(0, TILE_COUNT - 1)];
    info.allocator = &level->allocators[index].interface;


    level->chunk_handles[index] = fw64_level_load_chunk_at_pos(&level->level, &info, pos);
}

void tiles_test_level_uninit(TilesTestLevel* level) {
    player_uninit(&level->player);
    fw64_level_uninit(&level->level);

    for (int i = 0; i < ACTIVE_TILE_COUNT; i++) {
        fw64_bump_allocator_uninit(&level->allocators[i]);
    }
}

void tiles_test_level_update(TilesTestLevel* level) {
    Player* player = &level->player;

    fw64_level_update(&level->level);
    player_update(player);

    float player_z = player->node->transform.position.z;
    
    if (level->player_previous_z > level->next_row_trigger &&  player_z <= level->next_row_trigger)
        tiles_test_load_next_row(level);

    level->player_previous_z = player_z;
}

void tiles_test_level_draw(TilesTestLevel* level) {
    fw64Renderer* renderer = level->engine->renderer;

    fw64_renderer_set_anti_aliasing_enabled(renderer, 1);
    fw64_renderer_begin(renderer, FW64_RENDERER_MODE_TRIANGLES,  FW64_RENDERER_FLAG_CLEAR);
    player_draw(&level->player);
    player_draw_weapon(&level->player);

    fw64_renderer_set_anti_aliasing_enabled(renderer, 0);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}