#pragma once

#include "framework64/engine.h"

#include "player.h"
#include "healthbar.h"

typedef struct LevelBase LevelBase;

typedef struct {
    fw64Engine* engine;
    fw64Allocator* allocator;
    LevelBase* level;
    fw64Font* hud_font;
    Healthbar healthbar;

    fw64Font* interaction_font;
    fw64Texture* interaction_button;
    uint32_t interaction_image_frame;
    uint32_t interaction_loaded_frame;
    char interaction_text[16];
} UI;

void ui_init(UI* ui, fw64Engine* engine, fw64Allocator* allocator, LevelBase* level);
void ui_uninit(UI* ui);
void ui_draw(UI* ui);

void ui_set_interaction_text(UI* ui, const char* text, uint32_t icon_index);
void ui_clear_interaction_text(UI* ui);