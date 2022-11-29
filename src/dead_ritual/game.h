#pragma once

#include "framework64/engine.h"

#include "game_data.h"

#include "states/menu.h"
#include "states/playing.h"
#include "states/splash.h"
#include "states/game_over.h"

typedef union
{
    Splash splash;
    Menu menu;
    Playing playing;
    GameOver game_over;
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
