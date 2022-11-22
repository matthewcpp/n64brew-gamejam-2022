#pragma once

#include "framework64/animation_controller.h"
#include "framework64/engine.h"
#include "framework64/level.h"

#include "arcball_camera.h"

typedef struct {
    fw64Engine* engine;
    fw64ArcballCamera arcball;
    fw64Node* node;
    fw64AnimationData* animation_data;
    fw64AnimationController* animation_controller;
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
