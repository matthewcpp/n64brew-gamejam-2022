#include "game.h"
#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"
#include "framework64/util/renderpass_util.h"

#include "assets/zombie_animation.h"

// TODO: move this into a common file
#define ZOMBIE_SCALE 0.025f

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    zombie_config_init(&game->config, engine, allocator);
}

void game_update(Game* game){
    zombie_config_update(&game->config);
}

void game_fixed_update(Game* game) {
    (void)game;
}

void game_draw(Game* game) {
    zombie_config_draw(&game->config);
}
