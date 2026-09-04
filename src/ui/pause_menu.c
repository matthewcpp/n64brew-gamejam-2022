#include "pause_menu.h"

#include <framework64/controller_mapping/n64.h>

void pause_menu_init(PauseMenu* pause_menu, fw64Engine* engine, fw64Font* font, GameData* game_data) {
    pause_menu->engine = engine;
    pause_menu->font = font;
    pause_menu->game_data = game_data;
    pause_menu->prev_active = 0;
    pause_menu->active = 0;
    pause_menu->selected_item = 0;
    fw64_ui_navigation_init(&pause_menu->ui_nav, engine->input, 0);
}

static void pause_menu_update_inactive(PauseMenu* pause_menu) {
    if (fw64_input_controller_button_pressed(pause_menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
        pause_menu->active = 1;
        pause_menu->selected_item = 0;
    }
}

static void pause_menu_select_item(PauseMenu* pause_menu) {
    if (pause_menu->selected_item == PAUSE_MENU_CLOSE) {
        pause_menu->active = 0;
    } else if (pause_menu->selected_item == PAUSE_MENU_EXIT) {
        pause_menu->game_data->transition_to_state = GAME_STATE_MENU;
    }
}

static void pause_menu_update_active(PauseMenu* pause_menu) {
    fw64_ui_navigation_update(&pause_menu->ui_nav, pause_menu->engine->time->time_delta);

    if (fw64_ui_navigation_moved_up(&pause_menu->ui_nav)) {
        pause_menu->selected_item -= 1;

        if (pause_menu->selected_item < 0) {
            pause_menu->selected_item = PAUSE_MENU_ITEM_COUNT - 1;
        }
    } else if (fw64_ui_navigation_moved_up(&pause_menu->ui_nav)) {
        pause_menu->selected_item += 1;

        if (pause_menu->selected_item >= PAUSE_MENU_ITEM_COUNT) {
            pause_menu->selected_item = 0;
        }
    } else if (fw64_ui_navigation_accepted(&pause_menu->ui_nav)) {
        pause_menu_select_item(pause_menu);
    }
}

void pause_menu_update(PauseMenu* pause_menu) {
    pause_menu->prev_active = pause_menu->active;

    if (pause_menu->active) {
        pause_menu_update_active(pause_menu);
    } else {
        pause_menu_update_inactive(pause_menu);
    }
}

void pause_menu_draw(PauseMenu* pause_menu, fw64SpriteBatch* spritebatch) {
    if (!pause_menu->active) {
        return;
    }
}
