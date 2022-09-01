set(src_files
boo.h boo.c
    game.h game.c
    fps_camera.h fps_camera.c
    player.h player.c
    ui.h ui.c
    zombie.h zombie.c)

create_game(
    TARGET spooky
    SOURCES ${src_files})