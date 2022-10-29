#pragma once

#include "framework64/engine.h"

typedef struct {
	fw64Engine* engine;
	fw64Allocator* allocator;
	fw64Transform* player_pos;
	int turn_compass;
	fw64Texture* compass_bg;
	fw64Texture* home_icon;
	fw64Texture* north_icon;
} Compass;

void compass_init(Compass* compass, fw64Engine* engine, fw64Allocator* allocator, fw64Transform* player_pos);
void compass_uninit(Compass* compass);
void compass_draw(Compass* compass);