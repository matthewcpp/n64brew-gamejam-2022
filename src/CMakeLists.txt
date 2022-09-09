set(core_files
	behaviors.h behaviors.c
	mapped_input.h mapped_input.c
    player.h player.c
    movement_controller.h movement_controller.c
    weapon.h weapon.c
    level.h level.c
    weapon_controller.h weapon_controller.c
    trigger.h trigger.c
    zombie.h zombie.c
    zombie_spawner.h zombie_spawner.c
    ui.h ui.c
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
    SOURCES ${main_game_files} ${core_files} ${level_files} ${game_state_files} ${main_game_files})
