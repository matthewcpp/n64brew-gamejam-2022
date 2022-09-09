#pragma once

#include "framework64/engine.h"

#include "level.h"
#include "player.h"
#include "ui.h"
#include "trigger.h"

#define HILL_LEVEL_TRIGGER_CROW 0
#define HILL_LEVEL_TRIGGER_HOWL 1
#define HILL_LEVEL_TRIGGER_COUNT 2

typedef struct {
    fw64Engine* engine;
    fw64Level level;
    Player player;
    UI ui;
    TriggerBox triggers[HILL_LEVEL_TRIGGER_COUNT];
} HillLevel;

void hill_level_init(HillLevel* hill_level, fw64Engine* engine);
void hill_level_update(HillLevel* hill_level);
void hill_level_draw(HillLevel* hill_level);