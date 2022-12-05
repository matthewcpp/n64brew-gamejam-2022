#pragma once

#include "framework64/engine.h"
#include "framework64/util/bump_allocator.h"
#include "scene_viewer.h"

typedef struct {
    fw64Engine* engine;
    fw64BumpAllocator allocator;
    fw64Font* font;
    fw64SceneViewer scene_viewer;
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
