#pragma once

#include "player.h"
#include "ui.h"
#include "weapon_pickup.h"

/**
 * Contains common level elements.
 * Note this should be the FIRST member in any specific level class
 */
typedef struct LevelBase{
    fw64Engine* engine;
    Player player;
    ProjectileController projectile_controller;
    UI ui;
    AudioController audio_controller;
    WeaponPickups weapon_pickups;
    fw64Level level;
    fw64MusicBank* music;
    fw64SoundBank* sound;
    fw64Allocator* allocator;
} LevelBase;

void level_base_init(LevelBase* level, fw64Engine* engine, fw64Allocator* allocator, int music_bank, int sound_bank);
void level_base_uninit(LevelBase* level);
void level_base_update(LevelBase* level);
