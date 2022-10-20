#pragma once

#include "framework64/vec2.h"
#include "framework64/transform.h"

#include "mapped_input.h"
#include "weapon.h"
#include "weapon_controller.h"
#include "player.h"
#include "levels/levels.h"
#include "states/game_states.h"

typedef struct {
    int zombies_killed;
    double distance_run;
    int shots_fired;
    int missions_completed;
    int buildings_entered;
    int time_survived;
} PlayerCoolStats;
typedef struct {
    fw64Transform transform;
    int health;
    WeaponType equipped_weapon;
    WeaponAmmo ammo[WEAPON_COUNT];
    InputMapping input_map;
    PlayerCoolStats stats;
} PlayerData;

typedef struct {
    Vec2 city_cell;
    int node_id;
} DoorData;

typedef struct {
    Level transition_to_level;
    GameState transition_to_state;
    PlayerData player_data;
    DoorData door_data;
} GameData;

void game_data_init(GameData* game_data);
void game_data_save_player_data(GameData* game_data, Player* player);
void game_data_load_player_data(GameData* game_data, Player* player);