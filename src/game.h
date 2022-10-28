#pragma once

#include "framework64/engine.h"

#include "game_data.h"

#include "states/level_select.h"
#include "states/playing.h"
#include "states/splash.h"

typedef union
{
    Splash splash;
    LevelSelect level_select;
    Playing playing;
} GameStates;


typedef struct {
    fw64Engine* engine;
    GameData game_data;
    GameState current_state;
    GameStates states;
} Game;

#ifdef __cplusplus
extern "C" {
#endif

void game_init(Game* game, fw64Engine* engine);
void game_update(Game* game);
void game_draw(Game* game);

#ifdef __cplusplus
}
#endif
