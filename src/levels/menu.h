#pragma once

#include "framework64/engine.h"
#include "audio_controller.h"

typedef enum {
    MENU_SCREEN_INTRO_JAMLOGO,
    MENU_SCREEN_INTRO_TEAMLOGO,
    MENU_SCREEN_INTRO_SCARYLOGO,
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
	fw64Allocator* allocator;
	fw64Texture* bg;
	AudioController audio_controller;
	fw64SoundBank* sound;
	fw64Font* font;
	MenuScreen current_menu;
	float timer;
	int menu_choice;
} Menu;

void menu_init(Menu* menu, fw64Engine* engine, fw64Allocator* allocator);
void menu_update(Menu* menu);
void menu_draw(Menu* menu);
void menu_uninit(Menu* menu);