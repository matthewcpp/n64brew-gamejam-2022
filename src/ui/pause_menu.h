#include "game_data.h"

#include "fw64_ui_navigation.h"

#include <framework64/engine.h>

typedef enum {
    PAUSE_MENU_CLOSE,
    PAUSE_MENU_EXIT,
    PAUSE_MENU_ITEM_COUNT
} PauseMenuIndex;

typedef struct {
    fw64Engine* engine;
    fw64Font* font;
    GameData* game_data;
    int prev_active;
    int active;
    int selected_item;
    fw64UiNavigation ui_nav;
} PauseMenu;

void pause_menu_init(PauseMenu* pause_menu, fw64Engine* engine, fw64Font* font, GameData* game_data);
void pause_menu_update(PauseMenu* pause_menu);
void pause_menu_draw(PauseMenu* pause_menu, fw64SpriteBatch* spritebatch);

#define pause_menu_did_change_state(pause_menu) ((pause_menu)->active != (pause_menu)->prev_active)