#pragma once

#include "framework64/camera.h"
#include "framework64/engine.h"
#include "framework64/node.h"

#include "player.h"
#include "ui.h"
#include "zombie_spawner.h"

#include "level.h"

typedef struct {
    fw64Engine* engine;
    Player player;
    UI ui;
    ZombieSpawner zombie_spawner;
    fw64Level level;
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
