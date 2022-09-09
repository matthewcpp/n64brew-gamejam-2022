set(common_files
	behaviors.h behaviors.c
	mapped_input.h mapped_input.c
    player.h player.c
    movement_controller.h movement_controller.c
    weapon.h weapon.c
    level.h level.c
    weapon_controller.h weapon_controller.c
    trigger.h trigger.c
    zombie.h zombie.c
    ui.h ui.c
)

set(main_game_files
    game.h game.c
    zombie_spawner.h zombie_spawner.c
)

set(hill_test_files
    hill_test/game.h hill_test/game.c
    hill_test/hill_level.h hill_test/hill_level.c
)

create_game(
    TARGET spooky
    SOURCES ${main_game_files} ${common_files})

create_game(
    TARGET hill_test
    SOURCES ${hill_test_files} ${common_files})