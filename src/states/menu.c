#include "menu.h"
#include "audio_controller.h"
#include "framework64/controller_mapping/n64.h"
#include <framework64/util/renderpass_util.h>

#include "mapped_input.h"
#include "levels/levels.h"

#include "assets/assets.h"
#include "assets/music_bank_music.h"

#include <stdio.h>

#define LEVEL_MEMORY_POOL_SIZE (450 * 1024)
#define IMAGE_ALLOCATOR_BUFFER_SIZE (175 * 1024)

static void main_menu_draw(Menu* menu);
static void controls_menu_draw(Menu* menu);
static void process_input(Menu* menu);
static void set_menu_screen(Menu* menu, MenuScreen screen);

void game_state_menu_init(Menu* menu, fw64Engine* engine, GameData* game_data) {
	menu->engine = engine;
	menu->game_data = game_data;
	menu->menu_choice = 0;

	fw64Display* display = fw64_displays_get_primary(engine->displays);
	fw64Allocator* allocator = fw64_bump_allocator_init(&menu->bump_allocator, LEVEL_MEMORY_POOL_SIZE);

	menu->renderpass = fw64_renderpass_create(display, allocator);
	fw64_renderpass_util_ortho2d(menu->renderpass);

	menu->spritebatch = fw64_spritebatch_create(2, allocator);

	char* image_buffer = fw64_allocator_memalign(allocator, 8, IMAGE_ALLOCATOR_BUFFER_SIZE);
	fw64_bump_allocator_init_from_buffer(&menu->image_allocator, image_buffer, IMAGE_ALLOCATOR_BUFFER_SIZE);

	menu->control_scheme = INPUT_MAP_LAYOUT_MODERN_TWINSTICK;
	mapped_input_set_map_layout(&menu->game_data->player_data.input_map, menu->control_scheme);

	menu->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_menu, &menu->bump_allocator.interface);
	fw64_audio_play_music(engine->audio, music_bank_music_menu);

	menu->bg = NULL;
	set_menu_screen(menu, MENU_SCREEN_MAIN);
}

void game_state_menu_update(Menu* menu) {
	process_input(menu);

	if (fw64_audio_get_music_status(menu->engine->audio) != FW64_AUDIO_PLAYING)
		fw64_audio_play_music(menu->engine->audio, music_bank_music_menu);

}
void game_state_menu_draw(Menu* menu) {
	fw64_spritebatch_begin(menu->spritebatch);
	fw64_spritebatch_set_color(menu->spritebatch, 224, 224, 224, 255);
	fw64_spritebatch_set_active_layer(menu->spritebatch, 0);
	fw64_spritebatch_draw_sprite(menu->spritebatch, menu->bg, 0, 0);
	
	fw64_spritebatch_set_active_layer(menu->spritebatch, 1);
	if(menu->current_menu == MENU_SCREEN_MAIN) {
		main_menu_draw(menu);
	} else if (menu->current_menu == MENU_SCREEN_CONTROLS) {
		controls_menu_draw(menu);
	}
	fw64_spritebatch_end(menu->spritebatch);

	fw64_renderpass_begin(menu->renderpass);
	fw64_renderpass_draw_sprite_batch(menu->renderpass, menu->spritebatch);
	fw64_renderpass_end(menu->renderpass);

	fw64_renderer_submit_renderpass(menu->engine->renderer, menu->renderpass);
}
void game_state_menu_uninit(Menu* menu) {
	fw64_audio_stop_music(menu->engine->audio);
	set_menu_screen(menu, MENU_SCREEN_NONE);
	fw64_font_delete(menu->engine->assets, menu->font, &menu->bump_allocator.interface);

	fw64_renderpass_delete(menu->renderpass);
	fw64_spritebatch_delete(menu->spritebatch);
	
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

		case MENU_SCREEN_NONE:
			break;
	}

	if (asset_id != FW64_INVALID_ASSET_ID) {
		fw64Image* bg_image = fw64_assets_load_image(menu->engine->assets, asset_id, &menu->image_allocator.interface);
		menu->bg = fw64_texture_create_from_image(bg_image, &menu->image_allocator.interface);
	}
}

static void start_playing(Menu* menu, Level level) {
	menu->game_data->transition_to_level = level;
	menu->game_data->transition_to_state = GAME_STATE_PLAYING;

	PlayerData* player_data = &menu->game_data->player_data;
	WeaponInfo* handgun_info = weapon_get_info(WEAPON_TYPE_1911);

	player_data_init(player_data);
	player_data->equipped_weapon = WEAPON_TYPE_1911;
	player_data->ammo[WEAPON_TYPE_1911].current_mag_count = handgun_info->mag_size;
	player_data->ammo[WEAPON_TYPE_1911].additional_rounds_count = handgun_info->max_additional_rounds;
}

void process_input(Menu* menu) {
	switch(menu->current_menu) {
		case MENU_SCREEN_MAIN: {
				int go  = fw64_input_controller_button_released(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_A);
					go |= fw64_input_controller_button_released(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START);

				if(go) {
					switch(menu->menu_choice) {
						case MAIN_MENU_START:
							start_playing(menu, LEVEL_CHURCH_HILL);
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
				int move_cursor_up  = fw64_input_controller_button_released(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_UP);
					move_cursor_up |= fw64_input_controller_button_released(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_UP);
					if(prev_stick.y <= 0.5f)
						move_cursor_up |= !!(stick.y > 0.5f);
				int move_cursor_down  = fw64_input_controller_button_released(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_DOWN);
					move_cursor_down |= fw64_input_controller_button_released(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_DOWN);
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
			 	int go  = fw64_input_controller_button_released(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_B);
					go |= fw64_input_controller_button_released(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START);
				if(go) {
					mapped_input_set_map_layout(&menu->game_data->player_data.input_map, menu->control_scheme);
					set_menu_screen(menu, MENU_SCREEN_MAIN);
				}
				static Vec2 prev_stick = {0.0f, 0.0f};
				Vec2 stick = {0.0f, 0.0f};
				fw64_input_controller_stick(menu->engine->input, 0, &stick);
				int move_cursor_left  = fw64_input_controller_button_released(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT);
					move_cursor_left |= fw64_input_controller_button_released(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_LEFT);
					if(prev_stick.x >= -0.5f)
						move_cursor_left |= !!(stick.x < -0.5f);
				int move_cursor_right  = fw64_input_controller_button_released(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT);
					move_cursor_right |= fw64_input_controller_button_released(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_RIGHT);
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
		fw64_spritebatch_set_color(menu->spritebatch, 192, 192, 0, 255);
	} else {
		fw64_spritebatch_set_color(menu->spritebatch, 224, 224, 224, 255);
	}
		
	sprintf(text, "Start Game");
	fw64_spritebatch_draw_string(menu->spritebatch, menu->font, text, x, y);

	if(menu->menu_choice == MAIN_MENU_CONTROLS) {
		fw64_spritebatch_set_color(menu->spritebatch, 192, 192, 0, 255);
	} else {
		fw64_spritebatch_set_color(menu->spritebatch, 224, 224, 224, 255);
	}

	sprintf(text, "Controls");
	fw64_spritebatch_draw_string(menu->spritebatch, menu->font, text, x, y + y_advance);

	if(menu->menu_choice == MAIN_MENU_PRACTICE) {
		fw64_spritebatch_set_color(menu->spritebatch, 192, 192, 0, 255);
	} else {
		fw64_spritebatch_set_color(menu->spritebatch, 224, 224, 224, 255);
	}

	sprintf(text, "Practice Range");
	fw64_spritebatch_draw_string(menu->spritebatch, menu->font, text, x, y + (y_advance * 2));

}

void controls_menu_draw(Menu* menu) {
	IVec2 screen_size = fw64_display_get_size(fw64_displays_get_primary(menu->engine->displays));
	int x;
	int y = 20;
	int y_advance = 24;
	char text[20] = {0};

	sprintf(text, "Control Scheme:");
	IVec2 dimensions = fw64_font_measure_text(menu->font, text);
	x = (screen_size.x / 2) - (dimensions.x / 2);
	fw64_spritebatch_draw_string(menu->spritebatch, menu->font, text, x, y);

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
	fw64_spritebatch_draw_string(menu->spritebatch, menu->font, text, x, y + y_advance);
}