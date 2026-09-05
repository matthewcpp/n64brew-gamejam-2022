#include "game_data.h"

#include "fw64_ui_navigation.h"

#include <framework64/engine.h>

typedef enum {
    PAUSE_MENU_RESUME,
    PAUSE_MENU_EXIT,
    PAUSE_MENU_ITEM_COUNT
} PauseMenuIndex;


#define PAUSE_MENU_ITEM_POSITIONS_SIZE (PAUSE_MENU_ITEM_COUNT + 1)

typedef struct {
    fw64Engine* engine;
    fw64Font* font;
    fw64Texture* background_tex;
    GameData* game_data;
    int prev_active;
    int active;
    int selected_item;
    fw64UiNavigation ui_nav;
    int item_positions[PAUSE_MENU_ITEM_POSITIONS_SIZE];
} PauseMenu;

void pause_menu_init(PauseMenu* pause_menu, fw64Engine* engine, fw64Font* font, fw64Texture* background_tex, GameData* game_data);
void pause_menu_update(PauseMenu* pause_menu);
void pause_menu_draw(PauseMenu* pause_menu, fw64SpriteBatch* spritebatch);

#define pause_menu_did_change_state(pause_menu) ((pause_menu)->active != (pause_menu)->prev_active)