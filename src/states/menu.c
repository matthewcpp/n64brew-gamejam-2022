#include "menu.h"
#include "audio_controller.h"
#include "framework64/n64/controller_button.h"
#include "framework64/util/renderer_util.h"

#include "mapped_input.h"
#include "levels/levels.h"

#include "assets/assets.h"
#include "assets/music_bank_music.h"

#include <stdio.h>

#define LEVEL_MEMORY_POOL_SIZE (400 * 1024)
#define IMAGE_ALLOCATOR_BUFFER_SIZE (175 * 1024)

static void main_menu_draw(Menu* menu);
static void controls_menu_draw(Menu* menu);
static void process_input(Menu* menu);
static void set_menu_screen(Menu* menu, MenuScreen screen);

void game_state_menu_init(Menu* menu, fw64Engine* engine, GameData* game_data) {
	menu->engine = engine;
	menu->game_data = game_data;
	menu->menu_choice = 0;

	fw64_camera_init(&menu->camera);
	fw64_bump_allocator_init(&menu->bump_allocator, LEVEL_MEMORY_POOL_SIZE);

	uint8_t* image_buffer = menu->bump_allocator.interface.memalign(&menu->bump_allocator.interface, 8, IMAGE_ALLOCATOR_BUFFER_SIZE);
	fw64_bump_allocator_init_from_buffer(&menu->image_allocator, image_buffer, IMAGE_ALLOCATOR_BUFFER_SIZE);

	menu->control_scheme = INPUT_MAP_LAYOUT_MODERN_TWINSTICK;
	mapped_input_set_map_layout(&menu->game_data->player_data.input_map, menu->control_scheme);

	menu->font = fw64_font_load(engine->assets, FW64_ASSET_font_menu, &menu->bump_allocator.interface);

	menu->bg = NULL;
	set_menu_screen(menu, MENU_SCREEN_MAIN);
}

void game_state_menu_update(Menu* menu) {
	process_input(menu);

}
void game_state_menu_draw(Menu* menu) {
	fw64_renderer_begin(menu->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
	fw64_renderer_set_camera(menu->engine->renderer, &menu->camera);

	fw64_renderer_draw_sprite(menu->engine->renderer, menu->bg, 0, 0);
	
	if(menu->current_menu == MENU_SCREEN_MAIN) {
		main_menu_draw(menu);
	} else if (menu->current_menu == MENU_SCREEN_CONTROLS) {
		controls_menu_draw(menu);
	}

	fw64_renderer_end(menu->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}
void game_state_menu_uninit(Menu* menu) {
	set_menu_screen(menu, MENU_SCREEN_NONE);
	fw64_font_delete(menu->engine->assets, menu->font, &menu->bump_allocator.interface);
	
	fw64_bump_allocator_uninit(&menu->image_allocator);
	fw64_bump_allocator_uninit(&menu->bump_allocator);
	fw64_audio_stop_music(menu->engine->audio);
}

static void set_menu_screen(Menu* menu, MenuScreen screen) {
	if (menu->bg) {
		fw64Image* image = fw64_texture_get_image(menu->bg);
		fw64_image_delete(menu->engine->assets, image, &menu->image_allocator.interface);
		fw64_texture_delete(menu->bg, &menu->image_allocator.interface);
		menu->bg = NULL;
	}

	fw64_bump_allocator_reset(&menu->image_allocator);
	menu->current_menu = screen;

	fw64AssetId asset_id = FW64_INVALID_ASSET_ID;

	switch (menu->current_menu)
	{
		case MENU_SCREEN_MAIN:
			asset_id = FW64_ASSET_image_menu_main;
			break;

		case MENU_SCREEN_CONTROLS:
			asset_id = FW64_ASSET_image_menu_controls;
			break;
	}

	if (asset_id != FW64_INVALID_ASSET_ID) {
		fw64Image* bg_image = fw64_image_load(menu->engine->assets, asset_id, &menu->image_allocator.interface);
		menu->bg = fw64_texture_create_from_image(bg_image, &menu->image_allocator.interface);
	}
}

static void start_playing(Menu* menu, Level level) {
	menu->game_data->transition_to_level = level;
	menu->game_data->transition_to_state = GAME_STATE_PLAYING;

	PlayerData* player_data = &menu->game_data->player_data;
	WeaponInfo* handgun_info = weapon_get_info(WEAPON_TYPE_HANDGUN);

	player_data_init(player_data);
	player_data->equipped_weapon = WEAPON_TYPE_HANDGUN;
	player_data->ammo[WEAPON_TYPE_HANDGUN].current_mag_count = handgun_info->mag_size;
	player_data->ammo[WEAPON_TYPE_HANDGUN].additional_rounds_count = handgun_info->max_additional_rounds;
}

void process_input(Menu* menu) {
	switch(menu->current_menu) {
		case MENU_SCREEN_MAIN: {
				int go  = fw64_input_controller_button_pressed(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_A);
					go |= fw64_input_controller_button_pressed(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START);

				if(go) {
					switch(menu->menu_choice) {
						case MAIN_MENU_START:
							start_playing(menu, LEVEL_TILES);
							break;
						case MAIN_MENU_PRACTICE:
							start_playing(menu, LEVEL_TEST);
							break;
						case MAIN_MENU_CONTROLS:
							set_menu_screen(menu, MENU_SCREEN_CONTROLS);
							break;
						default:
							break;
					}
					break;
				}
				static Vec2 prev_stick = {0.0f, 0.0f};
				Vec2 stick = {0.0f, 0.0f};
				fw64_input_controller_stick(menu->engine->input, 0, &stick);
				int move_cursor_up  = fw64_input_controller_button_pressed(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_UP);
					move_cursor_up |= fw64_input_controller_button_pressed(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_UP);
					if(prev_stick.y <= 0.5f)
						move_cursor_up |= !!(stick.y > 0.5f);
				int move_cursor_down  = fw64_input_controller_button_pressed(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_DOWN);
					move_cursor_down |= fw64_input_controller_button_pressed(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_DOWN);
					if(prev_stick.y >= -0.5f)
						move_cursor_down |= !!(stick.y < -0.5f);

				prev_stick.x = stick.x;
				prev_stick.y = stick.y;

				if(move_cursor_up) {
					menu->menu_choice -= 1;
					if(menu->menu_choice < 0)
						menu->menu_choice = MAIN_MENU_CHOICES_COUNT - 1;
				} else if (move_cursor_down) {
					menu->menu_choice += 1;
					if(menu->menu_choice >= MAIN_MENU_CHOICES_COUNT)
						menu->menu_choice = 0;
				}
			break;
		}
		case MENU_SCREEN_CONTROLS: {
			 	int go  = fw64_input_controller_button_pressed(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_B);
					go |= fw64_input_controller_button_pressed(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START);
				if(go) {
					mapped_input_set_map_layout(&menu->game_data->player_data.input_map, menu->control_scheme);
					set_menu_screen(menu, MENU_SCREEN_MAIN);
				}
				static Vec2 prev_stick = {0.0f, 0.0f};
				Vec2 stick = {0.0f, 0.0f};
				fw64_input_controller_stick(menu->engine->input, 0, &stick);
				int move_cursor_left  = fw64_input_controller_button_pressed(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT);
					move_cursor_left |= fw64_input_controller_button_pressed(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_LEFT);
					if(prev_stick.x >= -0.5f)
						move_cursor_left |= !!(stick.x < -0.5f);
				int move_cursor_right  = fw64_input_controller_button_pressed(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT);
					move_cursor_right |= fw64_input_controller_button_pressed(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_RIGHT);
					if(prev_stick.x <= 0.5f)
						move_cursor_right |= !!(stick.x > 0.5f);

				prev_stick.x = stick.x;
				prev_stick.y = stick.y;

				if(move_cursor_left) {
					menu->control_scheme -= 1;
					if(menu->control_scheme < 0)
						menu->control_scheme = INPUT_MAP_LAYOUT_MODERN_TWINSTICK_SWAPPED;
				} else if (move_cursor_right) {
					menu->control_scheme += 1;
					if(menu->control_scheme > INPUT_MAP_LAYOUT_MODERN_TWINSTICK_SWAPPED)
						menu->control_scheme = INPUT_MAP_LAYOUT_PERFECTEYE;
				}
			break;
		}
		default:
			break;
	}
}

void main_menu_draw(Menu* menu) {
	int x = 20;
	int y = 130;
	int y_advance = 24;
	char text[20] = {0};

	if(menu->menu_choice == MAIN_MENU_START) {
		fw64_renderer_set_fill_color(menu->engine->renderer, 192, 192, 0, 255);
	} else {
		fw64_renderer_set_fill_color(menu->engine->renderer, 224, 224, 224, 255);
	}
		
	sprintf(text, "Start Game");
	fw64_renderer_draw_text(menu->engine->renderer, menu->font, x, y, text);

	if(menu->menu_choice == MAIN_MENU_CONTROLS) {
		fw64_renderer_set_fill_color(menu->engine->renderer, 192, 192, 0, 255);
	} else {
		fw64_renderer_set_fill_color(menu->engine->renderer, 224, 224, 224, 255);
	}

	sprintf(text, "Controls");
	fw64_renderer_draw_text(menu->engine->renderer, menu->font, x, y + y_advance, text);

	if(menu->menu_choice == MAIN_MENU_PRACTICE) {
		fw64_renderer_set_fill_color(menu->engine->renderer, 192, 192, 0, 255);
	} else {
		fw64_renderer_set_fill_color(menu->engine->renderer, 224, 224, 224, 255);
	}

	sprintf(text, "Practice Range");
	fw64_renderer_draw_text(menu->engine->renderer, menu->font, x, y + (y_advance * 2), text);

}

void controls_menu_draw(Menu* menu) {
	IVec2 screen_size;
	fw64_renderer_get_screen_size(menu->engine->renderer, &screen_size);
	int x;
	int y = 20;
	int y_advance = 24;
	char text[20] = {0};

	sprintf(text, "Control Scheme:");
	IVec2 dimensions = fw64_font_measure_text(menu->font, text);
	x = (screen_size.x / 2) - (dimensions.x / 2);
	fw64_renderer_draw_text(menu->engine->renderer, menu->font, x, y, text);

	switch(menu->control_scheme) {
		case INPUT_MAP_LAYOUT_PERFECTEYE:
			sprintf(text, "Perfect Eye");
			break;
		case INPUT_MAP_LAYOUT_MODERN_TWINSTICK:
			sprintf(text, "Modern");
			break;
		case INPUT_MAP_LAYOUT_MODERN_TWINSTICK_SWAPPED:
			sprintf(text, "Modern Southpaw");
			break;
		default:
			break;
	}

	dimensions = fw64_font_measure_text(menu->font, text);
	x = (screen_size.x / 2) - (dimensions.x / 2);
	fw64_renderer_draw_text(menu->engine->renderer, menu->font, x, y + y_advance, text);
}