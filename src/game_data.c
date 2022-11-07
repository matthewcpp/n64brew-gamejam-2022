#include "game_data.h"

#include <string.h>

static void player_cool_stats_init(PlayerCoolStats* stats) {
    memset(stats, 0, sizeof(PlayerCoolStats));
}

void player_data_reset(PlayerData* data) {
    data->health = PLAYER_MAX_HEALTH;
    data->equipped_weapon = WEAPON_TYPE_NONE;
    for(int i = 0; i < WEAPON_COUNT; i++) {
        data->ammo[i].current_mag_count = 0;
        data->ammo[i].additional_rounds_count = 0;
    }
    player_cool_stats_init(&data->stats);
}

void player_data_init(PlayerData* data) {
    player_data_reset(data);
    fw64_transform_init(&data->transform);
    mapped_input_set_map_layout(&data->input_map, INPUT_MAP_LAYOUT_MODERN_TWINSTICK); // does not set mapping's fw64Input*
    Vec2 threshold = {DEFAULT_STICK_THRESHOLD, DEFAULT_STICK_THRESHOLD};
    mapped_input_set_stick_threshold(&data->input_map, threshold);
}



static void door_data_init(DoorData* data) {
    data->city_cell.x = 0;
    data->city_cell.y = 0;
    data->node_id = -1;
}

void game_data_init(GameData* game_data) {
    memset(game_data, 0, sizeof(GameData));
    player_data_init(&game_data->player_data);
    door_data_init(&game_data->door_data);
}

void game_data_save_player_data(GameData* game_data, Player* player) {
    game_data->player_data.health = player->current_health;
    game_data->player_data.equipped_weapon = player->weapon_controller.weapon.info->type;
    for(int i = 0; i < WEAPON_COUNT; i++) {
        game_data->player_data.ammo[i].current_mag_count = player->weapon_controller.weapon_ammo[i].current_mag_count;
        game_data->player_data.ammo[i].additional_rounds_count = player->weapon_controller.weapon_ammo[i].additional_rounds_count;
    }

    for(int i = 0; i < INPUT_MAP_TOTAL_ACTIONS; i++) {
        game_data->player_data.input_map.buttons[i] = player->input_map.buttons[i];
    }
    game_data->player_data.input_map.threshold.x = player->input_map.threshold.x;
    game_data->player_data.input_map.threshold.y = player->input_map.threshold.y;
}
void game_data_load_player_data(GameData* game_data, Player* player) {    
    player->current_health = game_data->player_data.health;
    for(int i = 0; i < WEAPON_COUNT; i++) {
        weapon_controller_set_weapon_ammo(  &player->weapon_controller, i,
                                            game_data->player_data.ammo[i].current_mag_count,
                                            game_data->player_data.ammo[i].additional_rounds_count);
    }
    weapon_controller_set_weapon(&player->weapon_controller, game_data->player_data.equipped_weapon);

    for(int i = 0; i < INPUT_MAP_TOTAL_ACTIONS; i++) {
        player->input_map.buttons[i] = game_data->player_data.input_map.buttons[i];
    }
    player->input_map.threshold.x = game_data->player_data.input_map.threshold.x;
    player->input_map.threshold.y = game_data->player_data.input_map.threshold.y;
}