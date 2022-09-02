set(src_files
boo.h boo.c
    game.h game.c
    fps_camera.h fps_camera.c
    player.h player.c
    ui.h ui.c
    weapon.h weapon.c
    weapon_controller.h weapon_controller.c
    zombie.h zombie.c)

create_game(
    TARGET spooky
    SOURCES ${src_files})