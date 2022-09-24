set(core_files
	behaviors.h behaviors.c
	mapped_input.h mapped_input.c
    player.h player.c
    movement_controller.h movement_controller.c
    weapon.h weapon.c
    level.h level.c
    projectile_controller.h projectile_controller.c
    weapon_controller.h weapon_controller.c
    trigger.h trigger.c
    zombie.h zombie.c
    zombie_logic.h zombie_logic.c
    zombie_spawner.h zombie_spawner.c
    ui.h ui.c
)

set(debug_logging_files
    usb_debugging/debug.h usb_debugging/debug.c
    usb_debugging/usb.h usb_debugging/usb.c
    debug.h
)

set(level_files
    levels/church_hill.h levels/church_hill.c
    levels/test.h levels/test.c
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
    SOURCES ${main_game_files} ${core_files} ${debug_logging_files} ${level_files} ${game_state_files} ${main_game_files})
