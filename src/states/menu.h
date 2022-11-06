#pragma once

#include "framework64/engine.h"
#include "framework64/util/bump_allocator.h"
#include "audio_controller.h"
#include "game_data.h"

typedef enum {
	MENU_SCREEN_NONE,
    MENU_SCREEN_MAIN,
    MENU_SCREEN_CONTROLS
} MenuScreen;

typedef enum {
	MAIN_MENU_START,
	MAIN_MENU_CONTROLS,
	MAIN_MENU_PRACTICE,
	MAIN_MENU_CHOICES_COUNT
} MainMenuChoices;

typedef struct {
	fw64Engine* engine;
	fw64BumpAllocator bump_allocator;
	fw64BumpAllocator image_allocator;
	fw64Camera camera;
	GameData* game_data;
	fw64Texture* bg;
	fw64Texture* axis_arrows;
	fw64Font* font;
	fw64Font* font_small;
	MenuScreen current_menu;
	int menu_choice;
	int control_scheme;
} Menu;

void game_state_menu_init(Menu* menu, fw64Engine* engine, GameData* game_data);
void game_state_menu_update(Menu* menu);
void game_state_menu_draw(Menu* menu);
void game_state_menu_uninit(Menu* menu);