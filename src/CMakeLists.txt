set(core_files
    audio_controller.h audio_controller.c
	behaviors.h behaviors.c
    interact.h interact.c
	mapped_input.h mapped_input.c
    mesh_collection.h mesh_collection.c
    movement_controller.h movement_controller.c
    pickups.h pickups.c
    player.h player.c
    projectile_controller.h projectile_controller.c
    trigger.h trigger.c
    weapon.h weapon.c
    weapon_bob.h weapon_bob.c
    weapon_controller.h weapon_controller.c
    zombie.h zombie.c
    zombie_logic.h zombie_logic.c
    zombie_spawner.h zombie_spawner.c
)

set(ui_files
    ui/ui.h ui/ui.c
    ui/healthbar.h ui/healthbar.c
    ui/dialogue_window.h ui/dialogue_window.c
    ui/compass.h ui/compass.c
)

set(level_files
    levels/level_base.h levels/level_base.c
    levels/church_hill.h levels/church_hill.c
    levels/test.h levels/test.c
    levels/tiles_test.h levels/tiles_test.c
    levels/interior.h levels/interior.c
)

set(game_state_files
    states/game_states.h

    states/splash.h states/splash.c
    states/menu.h states/menu.c
    states/game_over.h states/game_over.c
    states/playing.h states/playing.c
)

set(actor_files
    actors/catherine.h actors/catherine.c
)

set(main_game_files
    dead_ritual/game.h dead_ritual/game.c
    game_data.h game_data.c
)

create_game(
    TARGET spooky
    SOURCES ${main_game_files} ${core_files} ${level_files} ${game_state_files} ${ui_files} ${actor_files} ${main_game_files})

target_include_directories(spooky PRIVATE dead_ritual)

set(zombie_config_files
    zombie_config/game.h zombie_config/game.c
    zombie_config/arcball_camera.h zombie_config/arcball_camera.c
    zombie_config/zombie_config_ui.h zombie_config/zombie_config_ui.c
    zombie_config.h zombie_config.c
)

create_game(
    TARGET zombie_config
    SOURCES ${zombie_config_files}
)

target_include_directories(zombie_config PRIVATE zombie_config)