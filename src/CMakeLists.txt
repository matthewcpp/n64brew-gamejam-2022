set(core_files
    audio_controller.h audio_controller.c
	behaviors.h behaviors.c
    interact.h interact.c
	mapped_input.h mapped_input.c
    level.h level.c
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

set(fw64_component_files
    components/fw64_billboard_node.h components/fw64_billboard_node.c
)

set(main_game_files
    dead_ritual/game.h dead_ritual/game.c
    game_data.h game_data.c
)

create_game(
    TARGET dead_ritual
    SOURCES ${main_game_files} ${core_files} ${level_files} ${game_state_files} ${ui_files} ${fw64_component_files} ${main_game_files}
)

target_include_directories(dead_ritual PRIVATE dead_ritual)

add_subdirectory(zombie_config)