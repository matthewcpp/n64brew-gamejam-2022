#include "levels/level_base.h"
#include "assets/layers.h"

static void level_base_init_audio(LevelBase* level, int music_bank, int sound_bank);

void level_base_init(LevelBase* level, fw64Engine* engine, GameData* game_data, fw64Allocator* allocator, int music_bank, int sound_bank) {
    level->engine = engine;
    level->game_data = game_data;
    level->allocator = allocator;

    fw64_level_init(&level->level, engine);
    level_base_init_audio(level, music_bank, sound_bank);

    projectile_controller_init(&level->projectile_controller, &level->level);
    player_init(&level->player, engine, &level->level, &level->projectile_controller, &level->audio_controller, level->allocator);

    pickups_init(&level->pickups, engine, &level->player, allocator);

    ui_init(&level->ui, engine, level->allocator, &level->player);
    interaction_init(&level->interaction, &level->level, &level->player.node->transform, FW64_layer_interactable);
}

void level_base_init_audio(LevelBase* level, int music_bank, int sound_bank) {
    if (music_bank != FW64_INVALID_ASSET_ID) {
        level->music = fw64_music_bank_load(level->engine->assets, music_bank, level->allocator);
        fw64_audio_set_music_bank(level->engine->audio, level->music);
    }
    else {
        level->music = NULL;
    }

    if (sound_bank != FW64_INVALID_ASSET_ID) {
        level->sound = fw64_sound_bank_load(level->engine->assets, sound_bank, level->allocator);
        fw64_audio_set_sound_bank(level->engine->audio, level->sound);
    }
    else {
        level->sound = NULL;
    }

    audio_controller_init(&level->audio_controller, level->engine->audio);
}

void level_base_uninit(LevelBase* level) {
    player_uninit(&level->player);
    fw64_level_uninit(&level->level);

    if (level->sound)
        fw64_sound_bank_delete(level->engine->assets, level->sound, level->allocator);
    
    if (level->music)
        fw64_music_bank_delete(level->engine->assets, level->music, level->allocator);
    
    ui_uninit(&level->ui);
}

void level_base_update(LevelBase* level) {
    //if (fw64_audio_get_music_status(level->engine->audio) == FW64_AUDIO_STOPPED)
    //    fw64_audio_play_music(level->engine->audio, 0);

    audio_controller_update(&level->audio_controller);
    player_update(&level->player);
    pickups_update(&level->pickups);
    interaction_update(&level->interaction);
}
