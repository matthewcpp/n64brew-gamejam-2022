#pragma once

#include "framework64/vec2.h"
#include "framework64/input.h"
#include "framework64/n64/controller_button.h"

/** \file mapped_input.h */

#define INPUT_MAP_BUTTON_UP 0
#define INPUT_MAP_BUTTON_DOWN 1
#define INPUT_MAP_ANALOG 2
#define INPUT_MAP_ERROR -1

#define DEFAULT_STICK_THRESHOLD 0.15f

#define INPUT_MAP_TOTAL_ACTIONS 21
#define INPUT_MAP_START 0x8001 //deconflict with both N64 and SDL mappings
typedef enum {
	//actions
	INPUT_MAP_MOVE_FORWARD = INPUT_MAP_START,
	INPUT_MAP_MOVE_BACKWARD,
	INPUT_MAP_MOVE_LEFT,
	INPUT_MAP_MOVE_RIGHT,
	INPUT_MAP_MOVE_FORWARD_2,
	INPUT_MAP_MOVE_BACKWARD_2,
	INPUT_MAP_MOVE_LEFT_2,
	INPUT_MAP_MOVE_RIGHT_2,
	INPUT_MAP_LOOK_UP,
	INPUT_MAP_LOOK_DOWN,
	INPUT_MAP_LOOK_LEFT,
	INPUT_MAP_LOOK_RIGHT,
	INPUT_MAP_LOOK_UP_2,
	INPUT_MAP_LOOK_DOWN_2,
	INPUT_MAP_LOOK_LEFT_2,
	INPUT_MAP_LOOK_RIGHT_2,
	INPUT_MAP_WEAPON_FIRE,
	INPUT_MAP_WEAPON_SWAP,
	INPUT_MAP_WEAPON_RELOAD,
	INPUT_MAP_WEAPON_RELOAD_2,
	INPUT_MAP_INTERACT,
	// digital axis
	INPUT_MAP_VSTICK_UP,
	INPUT_MAP_VSTICK_DOWN,
	INPUT_MAP_VSTICK_LEFT,
	INPUT_MAP_VSTICK_RIGHT,
	// analog axis
	INPUT_MAP_VSTICK_ANALOG_X_POS,
	INPUT_MAP_VSTICK_ANALOG_X_NEG,
	INPUT_MAP_VSTICK_ANALOG_Y_POS,
	INPUT_MAP_VSTICK_ANALOG_Y_NEG,

	INPUT_MAP_INVALID
} MappedButton;

typedef enum {
	INPUT_MAP_LAYOUT_PERFECTEYE,
	INPUT_MAP_LAYOUT_MODERN_TWINSTICK,
	INPUT_MAP_LAYOUT_MODERN_TWINSTICK_SWAPPED
} InputMapLayout;

typedef struct {
	fw64Input* fw64_input;
	Vec2 threshold;
	int buttons[INPUT_MAP_TOTAL_ACTIONS];
	int enabled;
} InputMapping;

void mapped_input_init(InputMapping* mapping, fw64Input* fw64_input);
void mapped_input_set_map_layout(InputMapping* mapping, InputMapLayout layout);
void mapped_input_set_map_button(InputMapping* mapping, MappedButton mapped_input_name, int new_button_name);
void mapped_input_set_stick_threshold(InputMapping* mapping, Vec2 new_threshold);
int mapped_input_controller_read(InputMapping* mapping, int controller, MappedButton button, Vec2* stick);

// this needs to be refactored. it returns either 1.0f for digital buttons or the absolute value of the analog axis requested
float mapped_input_get_axis(InputMapping* mapping, MappedButton mapped_input_name, Vec2* stick);