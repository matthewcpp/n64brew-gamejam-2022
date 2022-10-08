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
static void tiles_test_load_next_row(TilesTestLevel* level, CompassDirections dir);
static void rotate_all_handles(TilesTestLevel* level, CompassDirections dir);
static void rotate_one_handle(TilesTestLevel* level, int* handle, CompassDirections dir);
static int  get_rand_tile(int32_t x, int32_t y);

void tiles_test_level_init(TilesTestLevel* level, fw64Engine* engine) {
    level_base_init(&level->base, engine, fw64_default_allocator(), FW64_INVALID_ASSET_ID, FW64_ASSET_soundbank_sounds);
    
    level->handle_nw = 0;
    level->handle_ne =  TILE_ROW_CELLS  - 1;
    level->handle_sw =  TILE_ROW_CELLS  * (TILE_COL_CELLS  - 1);
    level->handle_se = (TILE_ROW_CELLS  *  TILE_COL_CELLS) - 1;

    /*
     * Fill the buffer of level chunks like so:
     * 
     *                 N (-z)
     *          [ 0 ]  [ 1 ]  [ 2 ]
     *  W (-x)  [ 3 ]  [ 4 ]  [ 5 ]   E (+x)
     *          [ 6 ]  [ 7 ]  [ 8 ]
     *                 S (+z)
     * 
     *  with chunk 4 being centered on the world origin
     *  for larger (5x5 etc) grids the center chunk stays at the world origin
     */
    Vec3 pos = { -(TILE_SIZE * (TILE_ROW_CELLS / 2)), 0.0f, -(TILE_SIZE * (TILE_ROW_CELLS / 2))};
    
    level->next_row_pos[NORTH] = pos.z -  TILE_SIZE;
    level->next_row_pos[SOUTH] = pos.z + (TILE_SIZE * TILE_COL_CELLS);
    level->next_row_pos[EAST]  = pos.x + (TILE_SIZE * TILE_ROW_CELLS);
    level->next_row_pos[WEST]  = pos.x -  TILE_SIZE;

    level->next_row_trigger[NORTH] = level->next_row_pos[NORTH] + (TILE_COL_CELLS * TILE_SIZE / 2);
    level->next_row_trigger[SOUTH] = level->next_row_pos[SOUTH] - (TILE_COL_CELLS * TILE_SIZE / 2);
    level->next_row_trigger[EAST]  = level->next_row_pos[EAST]  - (TILE_ROW_CELLS * TILE_SIZE / 2);
    level->next_row_trigger[WEST]  = level->next_row_pos[WEST]  + (TILE_ROW_CELLS * TILE_SIZE / 2);
    
    
    for (int i = 0; i < ACTIVE_TILE_COUNT; i++) {
        level->chunk_handles[i] = FW64_LEVEL_INVALID_CHUNK_HANDLE;
        fw64_bump_allocator_init(&level->allocators[i], BUMP_ALLOCATOR_SIZE);
        tiles_test_load_tile(level, i, &pos);
        pos.x += TILE_SIZE;
        if( pos.x >= (TILE_SIZE * ((TILE_ROW_CELLS / 2) + 1))) {
            pos.x -= TILE_SIZE * TILE_ROW_CELLS;
            pos.z += TILE_SIZE;
        }
    }

    player_add_ammo(&level->base.player, WEAPON_TYPE_UZI, 320);
    player_set_weapon(&level->base.player, WEAPON_TYPE_UZI);

    Vec3 starting_pos = {0.0f, 0.0f, 0.0f};
    player_set_position(&level->base.player, &starting_pos);
    vec3_copy(&level->player_prev_position, &level->base.player.node->transform.position); 

    fw64_renderer_set_clear_color(engine->renderer, 147, 204, 234); // happy sky blue
    fw64_renderer_set_fog_color(engine->renderer, 147, 204, 234);
    // fw64_renderer_set_clear_color(engine->renderer, 20, 4, 40); // spoopy blurple
    // fw64_renderer_set_fog_color(engine->renderer, 20, 4, 40);
    fw64_renderer_set_fog_positions(engine->renderer, 0.95f, 1.0f);
    // fw64_renderer_set_ambient_light_color(engine->renderer, 20, 4, 40);
    //fw64_renderer_set_light_color(engine->renderer, 0, 40, 16, 80);
}

void tiles_test_load_next_row(TilesTestLevel* level, CompassDirections dir) {
    
    Vec3 new_tile_pos = { 0.0f, 0.0f, 0.0f };
    
    switch(dir) {
        case NORTH: {
            new_tile_pos.x = level->next_row_pos[WEST] + TILE_SIZE;
            new_tile_pos.z = level->next_row_pos[NORTH];
            level->next_row_pos[NORTH] -= TILE_SIZE;
            level->next_row_pos[SOUTH] -= TILE_SIZE;
            level->next_row_trigger[NORTH] -= TILE_SIZE;
            level->next_row_trigger[SOUTH] -= TILE_SIZE;

            int row_index = level->handle_sw / TILE_ROW_CELLS;
            int col_index = level->handle_sw % TILE_ROW_CELLS;
            for(int i = 0; i < TILE_ROW_CELLS; i++) {
                int tile_index = (row_index * TILE_ROW_CELLS) + ((col_index + i) % TILE_ROW_CELLS);
                tiles_test_load_tile(level, tile_index, &new_tile_pos);
                new_tile_pos.x += TILE_SIZE;
            }
            break;
        }
        case SOUTH: {
            new_tile_pos.x = level->next_row_pos[WEST] + TILE_SIZE;
            new_tile_pos.z = level->next_row_pos[SOUTH];
            level->next_row_pos[NORTH] += TILE_SIZE;
            level->next_row_pos[SOUTH] += TILE_SIZE;
            level->next_row_trigger[NORTH] += TILE_SIZE;
            level->next_row_trigger[SOUTH] += TILE_SIZE;
            int row_index = level->handle_nw / TILE_ROW_CELLS;
            int col_index = level->handle_nw % TILE_ROW_CELLS;
            for(int i = 0; i < TILE_ROW_CELLS; i++) {
                int tile_index = (row_index * TILE_ROW_CELLS) + ((col_index + i) % TILE_ROW_CELLS);
                tiles_test_load_tile(level, tile_index, &new_tile_pos);
                new_tile_pos.x += TILE_SIZE;
            }
            break;
        }
        case EAST:  {
            new_tile_pos.x = level->next_row_pos[EAST];
            new_tile_pos.z = level->next_row_pos[NORTH] + TILE_SIZE;
            level->next_row_pos[WEST] += TILE_SIZE;
            level->next_row_pos[EAST] += TILE_SIZE;
            level->next_row_trigger[WEST] += TILE_SIZE;
            level->next_row_trigger[EAST] += TILE_SIZE;
            int row_index = level->handle_nw / TILE_ROW_CELLS;
            int col_index = level->handle_nw % TILE_ROW_CELLS;
            for(int i = 0; i < TILE_COL_CELLS; i++) {
                int tile_index = (((row_index + i) * TILE_ROW_CELLS) % ACTIVE_TILE_COUNT) + col_index;
                tiles_test_load_tile(level, tile_index, &new_tile_pos);
                new_tile_pos.z += TILE_SIZE;
            }
            break;
        }
        case WEST:  {
            new_tile_pos.x = level->next_row_pos[WEST];
            new_tile_pos.z = level->next_row_pos[NORTH] + TILE_SIZE;
            level->next_row_pos[WEST] -= TILE_SIZE;
            level->next_row_pos[EAST] -= TILE_SIZE;
            level->next_row_trigger[WEST] -= TILE_SIZE;
            level->next_row_trigger[EAST] -= TILE_SIZE;
            int row_index = level->handle_ne / TILE_ROW_CELLS;
            int col_index = level->handle_ne % TILE_ROW_CELLS;
            for(int i = 0; i < TILE_COL_CELLS; i++) {
                int tile_index = (((row_index + i) * TILE_ROW_CELLS) % ACTIVE_TILE_COUNT) + col_index;
                tiles_test_load_tile(level, tile_index, &new_tile_pos);
                new_tile_pos.z += TILE_SIZE;
            }
            break;
        }
        default:
            break;
    }

    rotate_all_handles(level, dir);
}

void tiles_test_load_tile(TilesTestLevel* level, int index, Vec3* pos) {
    // eject previous chunk in this index
    fw64_level_unload_chunk(&level->base.level, level->chunk_handles[index]);
    fw64_bump_allocator_reset(&level->allocators[index]);

    fw64LevelChunkInfo info;
    fw64_level_chunk_info_init(&info);
    
    int32_t grid_x = pos->x / TILE_SIZE;
    int32_t grid_y = pos->z / TILE_SIZE;

    info.scene_id = tile_scenes[get_rand_tile(grid_x, grid_y)];
    info.allocator = &level->allocators[index].interface;

    level->chunk_handles[index] = fw64_level_load_chunk_at_pos(&level->base.level, &info, pos);
}

void rotate_all_handles(TilesTestLevel* level, CompassDirections dir) {
    rotate_one_handle(level, &level->handle_nw, dir);
    rotate_one_handle(level, &level->handle_ne, dir);
    rotate_one_handle(level, &level->handle_sw, dir);
    rotate_one_handle(level, &level->handle_se, dir);
}

static void rotate_one_handle(TilesTestLevel* level, int* handle, CompassDirections dir) {
    switch(dir) {
        case NORTH:
            if ((*handle) < TILE_ROW_CELLS) { (*handle) += (TILE_ROW_CELLS * TILE_COL_CELLS); }
            *handle -= TILE_ROW_CELLS;
            break;
        case SOUTH:
            if ((*handle) >= (TILE_ROW_CELLS * (TILE_COL_CELLS - 1))) { (*handle) -= (TILE_ROW_CELLS * TILE_COL_CELLS); }
            *handle += TILE_ROW_CELLS;
            break;
        case WEST:
            if ((*handle) % TILE_ROW_CELLS == 0) { (*handle) += (TILE_ROW_CELLS - 1); }
            else { (*handle) -= 1; }
            break;
        case EAST:
            if ((*handle) % TILE_ROW_CELLS == (TILE_ROW_CELLS - 1)) { (*handle) -= (TILE_ROW_CELLS - 1); }
            else { (*handle) += 1; }
            break;
        default:
            break;
    }
}

void tiles_test_level_uninit(TilesTestLevel* level) {
    player_uninit(&level->base.player);
    fw64_level_uninit(&level->base.level);

    for (int i = 0; i < ACTIVE_TILE_COUNT; i++) {
        fw64_bump_allocator_uninit(&level->allocators[i]);
    }
}

void tiles_test_level_update(TilesTestLevel* level) {
    level_base_update(&level->base);
    Player* player = &level->base.player;
    Vec3 player_position;
    vec3_copy(&player_position, &player->node->transform.position);
    
    if (level->player_prev_position.z > level->next_row_trigger[NORTH] &&  player_position.z <= level->next_row_trigger[NORTH]) {
        tiles_test_load_next_row(level, NORTH);
    } else if (level->player_prev_position.z < level->next_row_trigger[SOUTH] &&  player_position.z >= level->next_row_trigger[SOUTH]) {
        tiles_test_load_next_row(level, SOUTH);
    }

    if (level->player_prev_position.x > level->next_row_trigger[WEST] &&  player_position.x <= level->next_row_trigger[WEST]) {
        tiles_test_load_next_row(level, WEST);
    } else if (level->player_prev_position.x < level->next_row_trigger[EAST] && player_position.x >= level->next_row_trigger[EAST]) {
        tiles_test_load_next_row(level, EAST);
    }
    
    vec3_copy(&level->player_prev_position, &player_position);
}

void tiles_test_level_draw(TilesTestLevel* level) {
    fw64Renderer* renderer = level->base.engine->renderer;

    fw64_renderer_set_anti_aliasing_enabled(renderer, 1);
    fw64_renderer_set_fog_enabled(renderer, 1);
    fw64_renderer_begin(renderer, FW64_RENDERER_MODE_TRIANGLES,  FW64_RENDERER_FLAG_CLEAR);
    player_draw(&level->base.player);
    fw64_renderer_set_fog_enabled(renderer, 0);
    player_draw_weapon(&level->base.player);
    fw64_renderer_set_anti_aliasing_enabled(renderer, 0);
    ui_draw(&level->base.ui);    
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}

// local function separate from the global rand function since it is meant to be reseeded each use
// reseeding the global rand every time we load a new grid tile is highly undesirable
int get_rand_tile(int32_t x, int32_t y) {
    /* 
    * Adapted from:
    * https://www.math.uni-bielefeld.de/~sillke/ALGORITHMS/random/marsaglia-c
    * by George Marsaglia
    */
    static uint32_t z_base, w_base;
    static char initialized = 0;

    if(!initialized) {
        initialized = 1;

        z_base = 362436069 ^ fw64_random_int();
        w_base = 521288629 ^ fw64_random_int();
    }

    uint32_t z, w;
    z = z_base ^ x;
    w = w_base ^ y;
    z = ((36969*(z&65535)+(z>>16))<<16);
    w = ((18000*(w&65535)+(w>>16))&65535);
    return (z+w) % TILE_COUNT;
}