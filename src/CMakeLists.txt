set(src_files
	behaviors.h behaviors.c
	boo.h boo.c
    game.h game.c
    fps_camera.h fps_camera.c
    level.h level.c
	mapped_input.h mapped_input.c
    player.h player.c
    ray.h
	ui.h ui.c
    weapon.h weapon.c
    weapon_controller.h weapon_controller.c
    zombie.h zombie.c
    zombie_spawner.h zombie_spawner.c
    )

create_game(
    TARGET spooky
    SOURCES ${src_files})