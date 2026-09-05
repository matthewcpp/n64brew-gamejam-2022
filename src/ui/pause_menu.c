#include "pause_menu.h"

#include <framework64/controller_mapping/n64.h>
#include <framework64/util/text_util.h>

#define PAUSE_MENU_WIDTH 100
#define PAUSE_MENU_HEIGHT 60

#define PAUSE_MENU_TITLE_POSITION_INDEX PAUSE_MENU_ITEM_COUNT

void pause_menu_init(PauseMenu* pause_menu, fw64Engine* engine, fw64Font* font, fw64Texture* background_tex, GameData* game_data) {
    pause_menu->engine = engine;
    pause_menu->font = font;
    pause_menu->background_tex = background_tex;
    pause_menu->game_data = game_data;
    pause_menu->prev_active = -1;
    pause_menu->active = 0;
    pause_menu->selected_item = 0;
    fw64_ui_navigation_init(&pause_menu->ui_nav, engine->input, 0);

    // center strings
    pause_menu->item_positions[PAUSE_MENU_TITLE_POSITION_INDEX] = fw64_text_util_horizontal_center_string(pause_menu->font, "Paused", PAUSE_MENU_WIDTH);
    pause_menu->item_positions[PAUSE_MENU_RESUME] = fw64_text_util_horizontal_center_string(pause_menu->font, "Resume", PAUSE_MENU_WIDTH);
    pause_menu->item_positions[PAUSE_MENU_EXIT] = fw64_text_util_horizontal_center_string(pause_menu->font, "Exit", PAUSE_MENU_WIDTH);
}

static void pause_menu_update_inactive(PauseMenu* pause_menu) {
    if (fw64_input_controller_button_released(pause_menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
        pause_menu->active = 1;
        pause_menu->selected_item = 0;
    }
}

static void pause_menu_select_item(PauseMenu* pause_menu) {
    if (pause_menu->selected_item == PAUSE_MENU_RESUME) {
        pause_menu->active = 0;
    } else if (pause_menu->selected_item == PAUSE_MENU_EXIT) {
        pause_menu->game_data->transition_to_state = GAME_STATE_MENU;
    }
}

static void pause_menu_update_active(PauseMenu* pause_menu) {
    if (fw64_ui_navigation_moved_up(&pause_menu->ui_nav)) {
        pause_menu->selected_item -= 1;

        if (pause_menu->selected_item < 0) {
            pause_menu->selected_item = PAUSE_MENU_ITEM_COUNT - 1;
        }
    } else if (fw64_ui_navigation_moved_down(&pause_menu->ui_nav)) {
        pause_menu->selected_item += 1;

        if (pause_menu->selected_item >= PAUSE_MENU_ITEM_COUNT) {
            pause_menu->selected_item = 0;
        }
    } else if (fw64_ui_navigation_accepted(&pause_menu->ui_nav)) {
        pause_menu_select_item(pause_menu);
    }
}

void pause_menu_update(PauseMenu* pause_menu) {
    int can_pause = pause_menu->prev_active >= 0;
    pause_menu->prev_active = pause_menu->active;
    fw64_ui_navigation_update(&pause_menu->ui_nav, pause_menu->engine->time->time_delta);

    if (!can_pause) {
        return;
    }

    if (pause_menu->active) {
        pause_menu_update_active(pause_menu);
    } else {
        pause_menu_update_inactive(pause_menu);
    }
}

static void pause_menu_set_text_color(PauseMenu* pause_menu, fw64SpriteBatch* spritebatch, int item) {
    if (pause_menu->selected_item == item) {
        fw64_spritebatch_set_color(spritebatch, 192, 192, 0, 255);
    } else {
        fw64_spritebatch_set_color(spritebatch, 255, 255, 255, 255);
    }
}

void pause_menu_draw(PauseMenu* pause_menu, fw64SpriteBatch* spritebatch) {
    if (!pause_menu->active) {
        return;
    }

    int pos_x = (320 - PAUSE_MENU_WIDTH) / 2;
    int pos_y = (240 - PAUSE_MENU_HEIGHT) / 2;

    fw64_spritebatch_set_color(spritebatch, 75, 75, 75, 150);
    fw64_spritebatch_draw_sprite_slice_rect(spritebatch, pause_menu->background_tex, 0, pos_x, pos_y, PAUSE_MENU_WIDTH, PAUSE_MENU_HEIGHT);
    
    fw64_spritebatch_set_color(spritebatch, 255, 255, 255, 255);
    fw64_spritebatch_draw_string(spritebatch, pause_menu->font, "Paused", pos_x + pause_menu->item_positions[PAUSE_MENU_TITLE_POSITION_INDEX], pos_y);

    pos_y += fw64_font_line_height(pause_menu->font);
    pause_menu_set_text_color(pause_menu, spritebatch, PAUSE_MENU_RESUME);
    fw64_spritebatch_draw_string(spritebatch, pause_menu->font, "Resume", pos_x + pause_menu->item_positions[PAUSE_MENU_RESUME], pos_y);

    pos_y += fw64_font_line_height(pause_menu->font);
    pause_menu_set_text_color(pause_menu, spritebatch, PAUSE_MENU_EXIT);
    fw64_spritebatch_draw_string(spritebatch, pause_menu->font, "Exit", pos_x + pause_menu->item_positions[PAUSE_MENU_EXIT], pos_y);
}
