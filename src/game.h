#pragma once

#include "framework64/camera.h"
#include "framework64/engine.h"
#include "framework64/node.h"

#include "player.h"
#include "ui.h"
#include "boo.h"
#include "zombie.h"
#include "framework64/scene.h"

typedef struct {
    fw64Engine* engine;
    fw64Scene* scene;
    Player player;
    UI ui;
    Boo boo;
    Weapon us_ar33;
    Zombie zombie;
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
