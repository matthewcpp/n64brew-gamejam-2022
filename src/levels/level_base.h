#pragma once

#include "game_data.h"
#include "player.h"
#include "ui/ui.h"
#include "pickups.h"
#include "interact.h"

typedef enum {
    RENDER_PASS_LEVEL,
    RENDER_PASS_PLAYER_WEAPON,
    RENDER_PASS_UI,
    RENDER_PASS_COUNT
} LevelRenderPassIndex;

/**
 * Contains common level elements.
 * Note this should be the FIRST member in any specific level class
 */
typedef struct LevelBase{
    fw64Engine* engine;
    GameData* game_data;
    Player player;
    ProjectileController projectile_controller;
    UI ui;
    AudioController audio_controller;
    fw64Level level;
    Interaction interaction;
    Pickups pickups;
    fw64Allocator* allocator;
    fw64RenderPass* renderpasses[RENDER_PASS_COUNT];
} LevelBase;

void level_base_init(LevelBase* level, fw64Engine* engine, GameData* game_data, fw64Allocator* allocator);
void level_base_uninit(LevelBase* level);
void level_base_update(LevelBase* level);
