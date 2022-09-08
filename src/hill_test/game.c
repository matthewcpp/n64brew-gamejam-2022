#include "game.h"


void game_init(Game* game, fw64Engine* engine) {
    hill_level_init(&game->hill_level, engine);
}

void game_update(Game* game){
    hill_level_update(&game->hill_level);
}

void game_draw(Game* game) {
    hill_level_draw(&game->hill_level);
}
