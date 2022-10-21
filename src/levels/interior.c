#include "interior.h"
#include "assets/assets.h"
#include "framework64/random.h"

#define ROOM_SCENE_COUNT 16
int room_scenes[ROOM_SCENE_COUNT] = {
	FW64_ASSET_scene_00_l0_r0_t0_b0,
	FW64_ASSET_scene_01_l0_r0_t0_b1,
	FW64_ASSET_scene_02_l0_r0_t1_b0,
	FW64_ASSET_scene_03_l0_r0_t1_b1,
	FW64_ASSET_scene_04_l0_r1_t0_b0,
	FW64_ASSET_scene_05_l0_r1_t0_b1,
	FW64_ASSET_scene_06_l0_r1_t1_b0,
	FW64_ASSET_scene_07_l0_r1_t1_b1,
	FW64_ASSET_scene_08_l1_r0_t0_b0,
	FW64_ASSET_scene_09_l1_r0_t0_b1,
	FW64_ASSET_scene_10_l1_r0_t1_b0,
	FW64_ASSET_scene_11_l1_r0_t1_b1,
	FW64_ASSET_scene_12_l1_r1_t0_b0,
	FW64_ASSET_scene_13_l1_r1_t0_b1,
	FW64_ASSET_scene_14_l1_r1_t1_b0,
	FW64_ASSET_scene_15_l1_r1_t1_b1	
};

#define BUMP_ALLOCATOR_SIZE (16 * 1024)
#define ROOM_SIZE 40.0f

// setup convenience constants
Vec3 vec_north_south = {0.0f, 0.0f, ROOM_SIZE};
Vec3 vec_east_west   = {ROOM_SIZE, 0.0f, 0.0f};
Vec3 start_room_pos  = {-(ROOM_SIZE * 0.5f), 0.0f, (ROOM_SIZE * 0.5f)};

typedef enum {
	ROOM_DIR_W = 8,
	ROOM_DIR_E = 4,
	ROOM_DIR_N = 2,
	ROOM_DIR_S = 1
} RoomDir;

typedef struct Room {
	Vec2 cell;
	Vec3 pos;
	int parent_dir;
	int  doors; // 4 bit field, NSEW
} Room;

static int room_taken(Room* rooms, int total, int cell_x, int cell_y);
static void create_room(Room* room, int cell_x, int cell_y, int parent_dir);
static void interior_load_room(InteriorLevel* level, int index, int room_scene, Vec3* pos);

void interior_level_init(InteriorLevel* level, fw64Engine* engine) {
    // reset all room scenes
	level_base_init(&level->base, engine, fw64_default_allocator(), FW64_INVALID_ASSET_ID, FW64_ASSET_soundbank_sounds);
    for (int i = 0; i < ROOM_COUNT; i++) {
        level->room_handles[i] = FW64_LEVEL_INVALID_CHUNK_HANDLE;
        fw64_bump_allocator_init(&level->allocators[i], BUMP_ALLOCATOR_SIZE);
    }
	
	Room all_rooms[ROOM_COUNT];
	Room* current_room = &all_rooms[0];
	vec2_set(&current_room->cell, 0.0f, 0.0f);
	vec3_copy(&current_room->pos, &start_room_pos);
	current_room->doors = 0;

	//current_room->parent_dir = 1 << fw64_random_int_in_range(0, 3);
	current_room->parent_dir = ROOM_DIR_S;
	do {
		current_room->doors = fw64_random_int_in_range(1, 15);
	} while(current_room->doors == current_room->parent_dir);
	current_room->doors &= ~current_room->parent_dir; // reserve 1 door of starter room for building exit
	
	int current_room_index = 0, running_room_count = 1, remaining_doors = current_room->doors;

	// generate rooms
	while(running_room_count < ROOM_COUNT) {
		if((remaining_doors & ROOM_DIR_N)) {
			if(!room_taken(&all_rooms[0], running_room_count, current_room->cell.x, current_room->cell.y - 1))
				create_room(&all_rooms[running_room_count++], current_room->cell.x, current_room->cell.y - 1, ROOM_DIR_S);
			remaining_doors &= ~(ROOM_DIR_N);
			continue;
		} else if((remaining_doors & ROOM_DIR_S)) {
			if(!room_taken(&all_rooms[0], running_room_count, current_room->cell.x, current_room->cell.y + 1))
				create_room(&all_rooms[running_room_count++], current_room->cell.x, current_room->cell.y + 1, ROOM_DIR_N);
			remaining_doors &= ~(ROOM_DIR_S);
			continue;
		} else if((remaining_doors & ROOM_DIR_E)) {
			if(!room_taken(&all_rooms[0], running_room_count, current_room->cell.x + 1, current_room->cell.y))
				create_room(&all_rooms[running_room_count++], current_room->cell.x + 1, current_room->cell.y, ROOM_DIR_W);
			remaining_doors &= ~(ROOM_DIR_E);
			continue;
		} else if((remaining_doors & ROOM_DIR_W)) {
			if(!room_taken(&all_rooms[0], running_room_count, current_room->cell.x - 1, current_room->cell.y))
				create_room(&all_rooms[running_room_count++], current_room->cell.x - 1, current_room->cell.y, ROOM_DIR_E);
			remaining_doors &= ~(ROOM_DIR_W);
			continue;
		} else {
			current_room_index++;
			if(current_room_index >= ROOM_COUNT)
				break;
			current_room = &all_rooms[current_room_index];
			remaining_doors = current_room->doors;
		}
	}

	// iterate all rooms once, to prune bad doors
	for(int i = 0; i < ROOM_COUNT; i++) {
		if(all_rooms[i].doors & ROOM_DIR_N) {
		    int taken = room_taken(&all_rooms[0], ROOM_COUNT, all_rooms[i].cell.x, all_rooms[i].cell.y - 1); 
			if(taken > 1) { all_rooms[taken-1].doors |= ROOM_DIR_S; } 
			else if (!taken) { all_rooms[i].doors &= ~ROOM_DIR_N; }			
		}
		if(all_rooms[i].doors & ROOM_DIR_S) {
		    int taken = room_taken(&all_rooms[0], ROOM_COUNT, all_rooms[i].cell.x, all_rooms[i].cell.y + 1); 
			if(taken > 1) { all_rooms[taken-1].doors |= ROOM_DIR_N; } 
			else if (!taken)  { all_rooms[i].doors &= ~ROOM_DIR_S; }			
		}
		if(all_rooms[i].doors & ROOM_DIR_W) {
		    int taken = room_taken(&all_rooms[0], ROOM_COUNT, all_rooms[i].cell.x - 1, all_rooms[i].cell.y); 
			if(taken > 1) { all_rooms[taken-1].doors |= ROOM_DIR_E; } 
			else if (!taken)  { all_rooms[i].doors &= ~ROOM_DIR_W; }			
		}
		if(all_rooms[i].doors & ROOM_DIR_E) {
		    int taken = room_taken(&all_rooms[0], ROOM_COUNT, all_rooms[i].cell.x + 1, all_rooms[i].cell.y); 
			if(taken > 1) { all_rooms[taken-1].doors |= ROOM_DIR_W; } 
			else if (!taken)  { all_rooms[i].doors &= ~ROOM_DIR_E; }			
		}
	}

	// iterate all rooms again, loading scenes
	for(int i = 0; i < ROOM_COUNT; i++) {
		interior_load_room(level, i, all_rooms[i].doors, &all_rooms[i].pos);
	}
    player_add_ammo(&level->base.player, WEAPON_TYPE_UZI, 320);
    player_set_weapon(&level->base.player, WEAPON_TYPE_UZI);

    Vec3 starting_pos = {0.0f, 5.0f, 15.0f};
    player_set_position(&level->base.player, &starting_pos);

    fw64_renderer_set_clear_color(engine->renderer, 32, 32, 32);
	fw64_renderer_set_fog_color(engine->renderer, 32, 32, 32);
    fw64_renderer_set_fog_positions(engine->renderer, 0.95f, 1.0f);
}

void interior_load_room(InteriorLevel* level, int index, int room_scene, Vec3* pos) {
    fw64LevelChunkInfo info;
    fw64_level_chunk_info_init(&info);
    info.scene_id = room_scenes[room_scene];
    info.allocator = &level->allocators[index].interface;
    level->room_handles[index] = fw64_level_load_chunk_at_pos(&level->base.level, &info, pos);
}

void create_room(Room* room, int cell_x, int cell_y, int parent_dir) {
	vec2_set(&room->cell, (float)cell_x, (float)cell_y);
	room->parent_dir = parent_dir;
	//vec3_set(&room->pos, &start_room_pos.x + (ROOM_SIZE * room->cell.x), start_room_pos.y, &start_room_pos.z + (ROOM_SIZE * room->cell.y));
	vec3_zero(&room->pos);
	room->pos.x = ROOM_SIZE * room->cell.x;
	room->pos.z = ROOM_SIZE * room->cell.y;
	vec3_add(&room->pos, &room->pos, &start_room_pos);

	do {
		room->doors = fw64_random_int_in_range(1, 15);
	} while(room->doors == room->parent_dir);
	room->doors |= room->parent_dir;

}

int room_taken(Room* rooms, int total, int cell_x, int cell_y) {
	for(int i = 0; i < total; i++) {
		if(rooms->cell.x == cell_x && rooms->cell.y == cell_y)
			return i+1;
		rooms++;
	}
	return 0;
}

void interior_level_uninit(InteriorLevel* level) {
    player_uninit(&level->base.player);
    fw64_level_uninit(&level->base.level);

    for (int i = 0; i < ROOM_COUNT; i++) {
        fw64_bump_allocator_uninit(&level->allocators[i]);
    }
}

void interior_level_update(InteriorLevel* level) {
    level_base_update(&level->base);
}

void interior_level_draw(InteriorLevel* level) {
    fw64Renderer* renderer = level->base.engine->renderer;
    fw64_renderer_set_anti_aliasing_enabled(renderer, 1);
    fw64_renderer_set_fog_enabled(renderer, 1);
	fw64_renderer_begin(renderer, FW64_RENDERER_MODE_TRIANGLES,  FW64_RENDERER_FLAG_CLEAR);
    player_draw(&level->base.player);
    fw64_renderer_set_fog_enabled(renderer, 0);
	player_draw_weapon(&level->base.player);
    fw64_renderer_set_anti_aliasing_enabled(renderer, 0);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}