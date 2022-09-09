#include "game_data.h"

#include <string.h>

void game_data_init(GameData* game_data) {
    memset(game_data, 0, sizeof(GameData));
}