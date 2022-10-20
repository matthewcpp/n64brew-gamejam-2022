#pragma once

#include "game_data.h"
#include "player.h"
#include "ui.h"
#include "weapon_pickup.h"
#include "interact.h"

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
    WeaponPickups weapon_pickups;
    fw64Level level;
    Interaction interaction;
    fw64MusicBank* music;
    fw64SoundBank* sound;
    fw64Allocator* allocator;
} LevelBase;

void level_base_init(LevelBase* level, fw64Engine* engine, GameData* game_data, fw64Allocator* allocator, int music_bank, int sound_bank);
void level_base_uninit(LevelBase* level);
void level_base_update(LevelBase* level);
