#include "mapped_input.h"

#define DEFAULT_STICK_THRESHOLD 0.15f

void mapped_input_init(InputMapping* mapping, fw64Input* fw64_input) {
	mapping->fw64_input = fw64_input;
	mapping->threshold.x = DEFAULT_STICK_THRESHOLD;
	mapping->threshold.y = DEFAULT_STICK_THRESHOLD;
	mapped_input_set_map_layout(mapping, INPUT_MAP_LAYOUT_MODERN_TWINSTICK);
    mapping->enabled = 1;
}

void mapped_input_set_map_layout(InputMapping* mapping, InputMapLayout layout) {

	mapping->buttons[INPUT_MAP_WEAPON_FIRE 		- INPUT_MAP_START] = FW64_N64_CONTROLLER_BUTTON_Z;
	mapping->buttons[INPUT_MAP_WEAPON_SWAP 		- INPUT_MAP_START] = FW64_N64_CONTROLLER_BUTTON_B;
	mapping->buttons[INPUT_MAP_WEAPON_RELOAD 	- INPUT_MAP_START] = FW64_N64_CONTROLLER_BUTTON_R;
	mapping->buttons[INPUT_MAP_INTERACT 		- INPUT_MAP_START] = FW64_N64_CONTROLLER_BUTTON_A;

	switch(layout) {
		case INPUT_MAP_LAYOUT_PERFECTEYE:
			// Inspired by 2 particular games
			// the analog stick moves fwd/back and turns left/right
			// the c-buttons look up/down and strafe
			mapping->buttons[INPUT_MAP_MOVE_FORWARD 	- INPUT_MAP_START] = INPUT_MAP_VSTICK_ANALOG_Y_POS;
			mapping->buttons[INPUT_MAP_MOVE_BACKWARD 	- INPUT_MAP_START] = INPUT_MAP_VSTICK_ANALOG_Y_NEG;
			mapping->buttons[INPUT_MAP_MOVE_LEFT 		- INPUT_MAP_START] = FW64_N64_CONTROLLER_BUTTON_C_LEFT;
			mapping->buttons[INPUT_MAP_MOVE_RIGHT 		- INPUT_MAP_START] = FW64_N64_CONTROLLER_BUTTON_C_RIGHT;
			mapping->buttons[INPUT_MAP_LOOK_UP 			- INPUT_MAP_START] = FW64_N64_CONTROLLER_BUTTON_C_UP;
			mapping->buttons[INPUT_MAP_LOOK_DOWN 		- INPUT_MAP_START] = FW64_N64_CONTROLLER_BUTTON_C_DOWN;
			mapping->buttons[INPUT_MAP_LOOK_LEFT 		- INPUT_MAP_START] = INPUT_MAP_VSTICK_ANALOG_X_NEG;
			mapping->buttons[INPUT_MAP_LOOK_RIGHT 		- INPUT_MAP_START] = INPUT_MAP_VSTICK_ANALOG_X_POS;
			break;
		case INPUT_MAP_LAYOUT_MODERN_TWINSTICK:
			// Similar to a typical FPS control scheme for more modern controllers
			// analog stick moves fwd/back and strafes
			// c-buttons look up/down and turn left/right
			mapping->buttons[INPUT_MAP_MOVE_FORWARD 	- INPUT_MAP_START] = INPUT_MAP_VSTICK_ANALOG_Y_POS;
			mapping->buttons[INPUT_MAP_MOVE_BACKWARD 	- INPUT_MAP_START] = INPUT_MAP_VSTICK_ANALOG_Y_NEG;
			mapping->buttons[INPUT_MAP_MOVE_LEFT 		- INPUT_MAP_START] = INPUT_MAP_VSTICK_ANALOG_X_NEG;
			mapping->buttons[INPUT_MAP_MOVE_RIGHT 		- INPUT_MAP_START] = INPUT_MAP_VSTICK_ANALOG_X_POS;
			mapping->buttons[INPUT_MAP_LOOK_UP 			- INPUT_MAP_START] = FW64_N64_CONTROLLER_BUTTON_C_UP;
			mapping->buttons[INPUT_MAP_LOOK_DOWN 		- INPUT_MAP_START] = FW64_N64_CONTROLLER_BUTTON_C_DOWN;
			mapping->buttons[INPUT_MAP_LOOK_LEFT 		- INPUT_MAP_START] = FW64_N64_CONTROLLER_BUTTON_C_LEFT;
			mapping->buttons[INPUT_MAP_LOOK_RIGHT 		- INPUT_MAP_START] = FW64_N64_CONTROLLER_BUTTON_C_RIGHT;
			break;
		case INPUT_MAP_LAYOUT_MODERN_TWINSTICK_SWAPPED:
			// For those that have 1 stick and prefer precise aim at all times
			// c-buttons move fwd/back and strafe
			// analog stick looks up/down and turn left/right
			mapping->buttons[INPUT_MAP_LOOK_UP		 	- INPUT_MAP_START] = INPUT_MAP_VSTICK_ANALOG_Y_POS;
			mapping->buttons[INPUT_MAP_LOOK_DOWN	 	- INPUT_MAP_START] = INPUT_MAP_VSTICK_ANALOG_Y_NEG;
			mapping->buttons[INPUT_MAP_LOOK_LEFT 		- INPUT_MAP_START] = INPUT_MAP_VSTICK_ANALOG_X_NEG;
			mapping->buttons[INPUT_MAP_LOOK_RIGHT 		- INPUT_MAP_START] = INPUT_MAP_VSTICK_ANALOG_X_POS;
			mapping->buttons[INPUT_MAP_MOVE_FORWARD 	- INPUT_MAP_START] = FW64_N64_CONTROLLER_BUTTON_C_UP;
			mapping->buttons[INPUT_MAP_MOVE_BACKWARD	- INPUT_MAP_START] = FW64_N64_CONTROLLER_BUTTON_C_DOWN;
			mapping->buttons[INPUT_MAP_MOVE_LEFT 		- INPUT_MAP_START] = FW64_N64_CONTROLLER_BUTTON_C_LEFT;
			mapping->buttons[INPUT_MAP_MOVE_RIGHT 		- INPUT_MAP_START] = FW64_N64_CONTROLLER_BUTTON_C_RIGHT;
			break;
		default: 
			mapped_input_set_map_layout(mapping, INPUT_MAP_LAYOUT_MODERN_TWINSTICK);
			break;
	}
}

void mapped_input_set_map_button(InputMapping* mapping, MappedButton mapped_input_name, int new_button_name) {
	mapping->buttons[mapped_input_name - INPUT_MAP_START] = new_button_name;
}
void mapped_input_set_stick_threshold(InputMapping* mapping, Vec2 new_threshold) {
	mapping->threshold = new_threshold;
}

static int safety_check_button_value(int button) {
	switch(button) {
		case FW64_N64_CONTROLLER_BUTTON_A:
		case FW64_N64_CONTROLLER_BUTTON_B:
		case FW64_N64_CONTROLLER_BUTTON_L:
		case FW64_N64_CONTROLLER_BUTTON_R:
		case FW64_N64_CONTROLLER_BUTTON_Z:
		case FW64_N64_CONTROLLER_BUTTON_START:
		case FW64_N64_CONTROLLER_BUTTON_C_UP:
		case FW64_N64_CONTROLLER_BUTTON_C_DOWN:
		case FW64_N64_CONTROLLER_BUTTON_C_LEFT:
		case FW64_N64_CONTROLLER_BUTTON_C_RIGHT:
		case FW64_N64_CONTROLLER_BUTTON_DPAD_UP:
		case FW64_N64_CONTROLLER_BUTTON_DPAD_DOWN:
		case FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT:
		case FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT:
			return 1;
		default:
			return 0;
	}
}

// returns:
// 0	button up
// 1	button down
// 2	analog stick values saved in stick parameter
// -1	error
int mapped_input_controller_read(InputMapping* mapping, int controller, MappedButton button, Vec2* stick) {
	if(button < INPUT_MAP_START || button > (INPUT_MAP_START + INPUT_MAP_TOTAL_ACTIONS - 1)) {
		return -1;
	}

	if (!mapping->enabled)
		return 0;

	Vec2 temp_stick;
	fw64_input_controller_stick(mapping->fw64_input, controller, &temp_stick);
	int val;
	switch(mapping->buttons[button - INPUT_MAP_START]) {
		
		case INPUT_MAP_VSTICK_UP:
			val = (temp_stick.y > mapping->threshold.y) ? 1 : 0;
			if(stick != 0) { //null pointer check
				stick->y = val;
			}
			return val;
		case INPUT_MAP_VSTICK_DOWN:
			val = (temp_stick.y < -(mapping->threshold.y)) ? 1 : 0;
			if(stick != 0) { //null pointer check
				stick->y = -1 * val;
			}
			return val;
		case INPUT_MAP_VSTICK_RIGHT:
			val = (temp_stick.x > mapping->threshold.x) ? 1 : 0;
			if(stick != 0) { //null pointer check
				stick->x = val;
			}
			return val;
		case INPUT_MAP_VSTICK_LEFT:
			val = (temp_stick.x < -(mapping->threshold.x)) ? 1 : 0;
			if(stick != 0) { //null pointer check
				stick->x = -1 * val;
			}
			return val;
		case INPUT_MAP_VSTICK_ANALOG_X_NEG:
			if(stick != 0 && (temp_stick.x < -mapping->threshold.x)) { //null pointer check
				stick->x = temp_stick.x;
				return 2;
			}
			return 0;
		case INPUT_MAP_VSTICK_ANALOG_X_POS:
			if(stick != 0 && (temp_stick.x > mapping->threshold.x)) { //null pointer check
				stick->x = temp_stick.x;
				return 2;
			}
			return 0;
		case INPUT_MAP_VSTICK_ANALOG_Y_NEG:
			if(stick != 0 && (temp_stick.y < -mapping->threshold.y)) { //null pointer check
				stick->y = temp_stick.y;
				return 2;
			}
			return 0;
		case INPUT_MAP_VSTICK_ANALOG_Y_POS:
			if(stick != 0 && (temp_stick.y > mapping->threshold.y)) { //null pointer check
				stick->y = temp_stick.y;
				return 2;
			}
			return 0;
		default:
			if(safety_check_button_value(mapping->buttons[button - INPUT_MAP_START])) {
				return !!(fw64_input_controller_button_down(mapping->fw64_input, controller, mapping->buttons[button - INPUT_MAP_START]));
			}
			break;
	}

	return -1;
	
}

float mapped_input_get_axis(InputMapping* mapping, MappedButton mapped_input_name, Vec2* stick){
	if(mapped_input_name < INPUT_MAP_START || mapped_input_name > (INPUT_MAP_START + INPUT_MAP_TOTAL_ACTIONS - 1)) {
		return 0.0f;
	}

	if (!mapping->enabled)
		return 0;

	float modifier = 1.0f;
	switch(mapping->buttons[mapped_input_name - INPUT_MAP_START]) {
		case INPUT_MAP_VSTICK_ANALOG_X_NEG: /* fall through */
			modifier *= -1.0f;
		case INPUT_MAP_VSTICK_ANALOG_X_POS:
			modifier *= stick->x;
			
			if(modifier < mapping->threshold.x)
				modifier = 0.0f;
			
			break;
		case INPUT_MAP_VSTICK_ANALOG_Y_NEG: /* fall through */
			modifier *= -1.0f;
		case INPUT_MAP_VSTICK_ANALOG_Y_POS:
			modifier *= stick->y;

			if(modifier < mapping->threshold.y)
				modifier = 0.0f;

			break;
		default:
			break;                
	}
	return modifier;
}