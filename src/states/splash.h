#pragma once

#include "framework64/engine.h"
#include "framework64/util/bump_allocator.h"

#include "game_data.h"

typedef enum {
    SPLASH_STATE_NONE,
    SPLASH_STATE_JAMLOGO,
    SPLASH_STATE_TEAMLOGO,
    SPLASH_STATE_SCARYLOGO,
} SplashState;

typedef struct {
    fw64Engine* engine;
    GameData* game_data;
    fw64BumpAllocator bump_allocator;
    SplashState current_state;
    float current_state_time;
    fw64Texture* image_tex;
    fw64Camera camera;
} Splash;

void game_state_splash_init(Splash* splash, fw64Engine* engine, GameData* game_data);
void game_state_splash_uninit(Splash* splash);
void game_state_splash_update(Splash* splash);
void game_state_splash_draw(Splash* splash);