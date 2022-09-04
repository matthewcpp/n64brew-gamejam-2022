#pragma once

#include "framework64/camera.h"
#include "framework64/engine.h"
#include "framework64/node.h"

#include "player.h"
#include "ui.h"
#include "boo.h"
#include "zombie_spawner.h"

#include "level.h"

typedef struct {
    fw64Engine* engine;
    Player player;
    UI ui;
    Boo boo;
    ZombieSpawner zombie_spawner;
    fw64Level level;
} Game;

#ifdef __cplusplus
extern "C" {
#endif

void game_init(Game* example, fw64Engine* engine);
void game_update(Game* example);
void game_draw(Game* example);

#ifdef __cplusplus
}
#endif
