#include "levels/level_base.h"
#include "assets/layers.h"
#include "assets/image_atlas_n64_buttons.h"

static void level_base_init_audio(LevelBase* level, int music_bank, int sound_bank);

void level_base_init(LevelBase* level, fw64Engine* engine, GameData* game_data, fw64Allocator* allocator) {
    level->engine = engine;
    level->game_data = game_data;
    level->allocator = allocator;

    fw64_level_init(&level->level, engine);

    projectile_controller_init(&level->projectile_controller, &level->level);
    player_init(&level->player, engine, &level->level, &level->projectile_controller, &level->audio_controller, level->allocator);

    pickups_init(&level->pickups, engine, &level->player, allocator);

    ui_init(&level->ui, engine, level->allocator, level);
    interaction_init(&level->interaction, &level->level, &level->player.node->transform, FW64_layer_interactable);
    audio_controller_init(&level->audio_controller, level->engine->audio);
}

void level_base_uninit(LevelBase* level) {
    player_uninit(&level->player);
    fw64_level_uninit(&level->level);
    ui_uninit(&level->ui);
}

static void update_ui_interaction_text(LevelBase* level) {
    fw64Node* interaction_node = level->interaction.interesting_node;
    if (interaction_node) {
        if (interaction_node->layer_mask & FW64_layer_entrance) {
            ui_set_interaction_text(&level->ui, "Enter", image_atlas_n64_buttons_frame_button_a);
            return;
        }
    }

    WeaponAmmo* weapon_ammo = weapon_controller_get_current_weapon_ammo( &level->player.weapon_controller);
    if (weapon_ammo->current_mag_count == 0 && weapon_ammo->additional_rounds_count > 0) {
        ui_set_interaction_text(&level->ui, "Reload", image_atlas_n64_buttons_frame_button_r);
        return;
    }
    
    ui_clear_interaction_text(&level->ui);
}

void level_base_update(LevelBase* level) {
    audio_controller_update(&level->audio_controller);
    player_update(&level->player);
    pickups_update(&level->pickups);
    interaction_update(&level->interaction);
    update_ui_interaction_text(level);
}
