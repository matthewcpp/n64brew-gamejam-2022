#pragma once

#include "framework64/engine.h"

#include "player.h"

typedef struct {
    fw64Engine* engine;
    fw64Allocator* allocator;
    Player* player;
    fw64Font* ammo_font;
} UI;

void ui_init(UI* ui, fw64Engine* engine, fw64Allocator* allocator, Player* player);
void ui_uninit(UI* ui);
void ui_draw(UI* ui);