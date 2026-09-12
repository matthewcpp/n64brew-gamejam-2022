set(zombie_config_files
    game.h game.c
    arcball_camera.h arcball_camera.c
    zombie_config_ui.h zombie_config_ui.c
    zombie_config.h zombie_config.c
    zombie_appearance.h zombie_appearance.c
    ../components/fw64_headlight.h ../components/fw64_headlight.c
)

create_game(
    TARGET zombie_config
    SOURCES ${zombie_config_files}
)

target_include_directories(zombie_config PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)