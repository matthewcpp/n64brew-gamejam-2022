#include "pause_menu.h"

#include <framework64/controller_mapping/n64.h>

void pause_menu_init(PauseMenu* pause_menu, fw64Engine* engine, fw64Font* font, GameData* game_data) {
    pause_menu->engine = engine;
    pause_menu->font = font;
    pause_menu->game_data = game_data;
    pause_menu->prev_active = 0;
    pause_menu->active = 0;
}

void pause_menu_update(PauseMenu* pause_menu) {
    pause_menu->prev_active = pause_menu->active;

    if (pause_menu->active) {
        
    } else {
        if (fw64_input_controller_button_pressed(pause_menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
            pause_menu->active = 1;
        }
    }
}

void pause_menu_draw(PauseMenu* pause_menu, fw64SpriteBatch* spritebatch) {
    if (!pause_menu->active) {
        return;
    }


}
