#include "game_data.h"

#include <framework64/engine.h>

typedef struct {
    fw64Engine* engine;
    fw64Font* font;
    GameData* game_data;
    int prev_active;
    int active;
} PauseMenu;

void pause_menu_init(PauseMenu* pause_menu, fw64Engine* engine, fw64Font* font, GameData* game_data);
void pause_menu_update(PauseMenu* pause_menu);
void pause_menu_draw(PauseMenu* pause_menu, fw64SpriteBatch* spritebatch);

#define pause_menu_did_change_state(pause_menu) ((pause_menu)->active != (pause_menu)->prev_active)