set(core_files
    audio_controller.h audio_controller.c
	behaviors.h behaviors.c
	mapped_input.h mapped_input.c
    player.h player.c
    movement_controller.h movement_controller.c
    weapon.h weapon.c
    projectile_controller.h projectile_controller.c
    weapon_bob.h weapon_bob.c
    weapon_controller.h weapon_controller.c
    weapon_pickup.h weapon_pickup.c
    trigger.h trigger.c
    zombie.h zombie.c
    zombie_logic.h zombie_logic.c
    zombie_spawner.h zombie_spawner.c
    
)

set(ui_files
    ui.h ui.c
    healthbar.h healthbar.c
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

    states/level_select.h states/level_select.c
    states/playing.h states/playing.c
)

set(main_game_files
    game.h game.c
    game_data.h game_data.c
)

create_game(
    TARGET spooky
    SOURCES ${main_game_files} ${core_files} ${level_files} ${game_state_files} ${ui_files} ${main_game_files})
