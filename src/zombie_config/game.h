#pragma once

#include "zombie_config.h"


typedef struct {
    ZombieConfig config;
} Game;

#ifdef __cplusplus
extern "C" {
#endif

void game_init(Game* game, fw64Engine* engine);
void game_update(Game* game);
void game_fixed_update(Game* game);
void game_draw(Game* game);

#ifdef __cplusplus
}
#endif
