#include "menu.h"
#include "assets/assets.h"
#include "assets/sound_bank_sounds.h"
#include "audio_controller.h"
#include "framework64/n64/controller_button.h"
#include "framework64/util/renderer_util.h"

#include <stdio.h>

static void menu_init_audio(Menu* menu);

static void jamlogo_init(Menu* menu);
static void jamlogo_uninit(Menu* menu);
static void teamlogo_init(Menu* menu);
static void teamlogo_uninit(Menu* menu);
static void scarylogo_init(Menu* menu);
static void scarylogo_uninit(Menu* menu);

static void advance_one_intro(Menu* menu);
static void skip_all_intros(Menu* menu);

static void main_menu_init(Menu* menu);
static void main_menu_draw(Menu* menu);
static void main_menu_uninit(Menu* menu);

static void controls_menu_init(Menu* menu);
static void controls_menu_draw(Menu* menu);
static void controls_menu_uninit(Menu* menu);

static void process_audio(Menu* menu);
static void process_timer(Menu* menu);
static void process_input(Menu* menu);

void menu_init(Menu* menu, fw64Engine* engine, fw64Allocator* allocator) {
	menu->engine = engine;
	menu->allocator = allocator;
	menu->bg = NULL;
	menu->font = fw64_font_load(engine->assets, FW64_ASSET_font_menu, allocator);
	menu_init_audio(menu);
	jamlogo_init(menu);
}

void menu_update(Menu* menu) {

	process_audio(menu);
	audio_controller_update(&menu->audio_controller);
	process_timer(menu);
	process_input(menu);

}
void menu_draw(Menu* menu) {
	if(menu->current_menu <= MENU_SCREEN_INTRO_TEAMLOGO && menu->timer > 4.0f) {
		uint8_t color = (int)(255.0 * ((6.0f - menu->timer) * 0.5f));
		fw64_renderer_set_fill_color(menu->engine->renderer, color, color, color, 255);
	} else if(menu->current_menu <= MENU_SCREEN_INTRO_SCARYLOGO && menu->timer < 2.0f) {
		uint8_t color = (int)(255.0 * menu->timer * 0.5f);
		fw64_renderer_set_fill_color(menu->engine->renderer, color, color, color, 255);
	}
	if(menu->bg) {
		fw64_renderer_draw_sprite(menu->engine->renderer, menu->bg, 0, 0);
	}

	if(menu->current_menu == MENU_SCREEN_MAIN) {
		main_menu_draw(menu);
	} else if (menu->current_menu == MENU_SCREEN_CONTROLS) {
		controls_menu_draw(menu);
	}

}
void menu_uninit(Menu* menu) {

	switch(menu->current_menu) {
		case MENU_SCREEN_INTRO_JAMLOGO:
			jamlogo_uninit(menu);
			break;
		case MENU_SCREEN_INTRO_TEAMLOGO:
			teamlogo_uninit(menu);
			break;
		case MENU_SCREEN_INTRO_SCARYLOGO:
			scarylogo_uninit(menu);
			break;
		case MENU_SCREEN_MAIN:
			main_menu_uninit(menu);
			break;
		case MENU_SCREEN_CONTROLS:
			controls_menu_uninit(menu);
			break;			
		default:
			break;
	}

	if (menu->sound)
        fw64_sound_bank_delete(menu->engine->assets, menu->sound, menu->allocator);

	fw64_font_delete(menu->engine->assets, menu->font, menu->allocator);
}

void jamlogo_init(Menu* menu) {
	menu->current_menu = MENU_SCREEN_INTRO_JAMLOGO;
	menu->timer = 6.0f; // 3 seconds
	fw64Image* bg_image = fw64_image_load_with_options(menu->engine->assets, FW64_ASSET_image_jam_logo, FW64_IMAGE_FLAG_NONE, menu->allocator);
	menu->bg = fw64_texture_create_from_image(bg_image, menu->allocator);
}
void jamlogo_uninit(Menu* menu) {
	if(menu->bg == NULL)
		return;
	fw64_image_delete(menu->engine->assets, fw64_texture_get_image(menu->bg), menu->allocator);
    fw64_texture_delete(menu->bg, menu->allocator);
}
void teamlogo_init(Menu* menu) {
	menu->current_menu = MENU_SCREEN_INTRO_TEAMLOGO;
	menu->timer = 6.0f; // 3 seconds
	fw64Image* bg_image = fw64_image_load_with_options(menu->engine->assets, FW64_ASSET_image_team_logo, FW64_IMAGE_FLAG_NONE, menu->allocator);
	menu->bg = fw64_texture_create_from_image(bg_image, menu->allocator);
}
void teamlogo_uninit(Menu* menu) {
	if(menu->bg == NULL)
		return;
	fw64_image_delete(menu->engine->assets, fw64_texture_get_image(menu->bg), menu->allocator);
    fw64_texture_delete(menu->bg, menu->allocator);
}
void scarylogo_init(Menu* menu) {
	menu->current_menu = MENU_SCREEN_INTRO_SCARYLOGO;
	menu->timer = 6.0f; // 3 seconds
	fw64Image* bg_image = fw64_image_load_with_options(menu->engine->assets, FW64_ASSET_image_scary_logo, FW64_IMAGE_FLAG_NONE, menu->allocator);
	menu->bg = fw64_texture_create_from_image(bg_image, menu->allocator);

	if(menu->sound)
		audio_controller_play(&menu->audio_controller, AUDIO_CONTROLLER_ENVIRONMENT, sound_bank_sounds_scary_logo);
}
void scarylogo_uninit(Menu* menu) {
	if(menu->bg == NULL)
		return;
	fw64_image_delete(menu->engine->assets, fw64_texture_get_image(menu->bg), menu->allocator);
    fw64_texture_delete(menu->bg, menu->allocator);
}
void main_menu_init(Menu* menu) {
	menu->current_menu = MENU_SCREEN_MAIN;
	fw64Image* bg_image = fw64_image_load_with_options(menu->engine->assets, FW64_ASSET_image_menu_main, FW64_IMAGE_FLAG_NONE, menu->allocator);
	menu->bg = fw64_texture_create_from_image(bg_image, menu->allocator);
	menu->menu_choice = 0;
	if(menu->sound)
		audio_controller_play(&menu->audio_controller, AUDIO_CONTROLLER_ENVIRONMENT, sound_bank_sounds_Menu_Song2);
}
void main_menu_uninit(Menu* menu) {
	if(menu->bg == NULL)
		return;
	fw64_image_delete(menu->engine->assets, fw64_texture_get_image(menu->bg), menu->allocator);
    fw64_texture_delete(menu->bg, menu->allocator);
}

void controls_menu_init(Menu* menu) {
	menu->current_menu = MENU_SCREEN_CONTROLS;
	fw64Image* bg_image = fw64_image_load_with_options(menu->engine->assets, FW64_ASSET_image_menu_controls, FW64_IMAGE_FLAG_NONE, menu->allocator);
	menu->bg = fw64_texture_create_from_image(bg_image, menu->allocator);
}
void controls_menu_uninit(Menu* menu) {
	if(menu->bg == NULL)
		return;
	fw64_image_delete(menu->engine->assets, fw64_texture_get_image(menu->bg), menu->allocator);
    fw64_texture_delete(menu->bg, menu->allocator);
}

void process_timer(Menu* menu) {
	if(menu->current_menu > MENU_SCREEN_INTRO_SCARYLOGO)
		return;

	menu->timer -= menu->engine->time->time_delta;

	if(menu->timer < EPSILON) {
		advance_one_intro(menu);
	}	
}

void process_input(Menu* menu) {
	switch(menu->current_menu) {
		case MENU_SCREEN_INTRO_JAMLOGO:		/* fall through */
		case MENU_SCREEN_INTRO_TEAMLOGO:	/* fall through */
		case MENU_SCREEN_INTRO_SCARYLOGO:
			int skip_one  = fw64_input_controller_button_pressed(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_A);
			    skip_one |= fw64_input_controller_button_pressed(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_B);
			    skip_one |= fw64_input_controller_button_pressed(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_Z);
			if(skip_one) {
				advance_one_intro(menu);
			} else {
				int skip_all = fw64_input_controller_button_pressed(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START);
				if(skip_all) {
					skip_all_intros(menu);
				}
			}
			break;
		case MENU_SCREEN_MAIN:
				int go  = fw64_input_controller_button_pressed(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_A);
					go |= fw64_input_controller_button_pressed(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START);

				if(go) {
					break;
				}
				Vec2 stick = {0.0f, 0.0f};
				fw64_input_controller_stick(menu->engine->input, 0, &stick);
				int move_cursor_up  = fw64_input_controller_button_pressed(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_UP);
					move_cursor_up |= fw64_input_controller_button_pressed(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_UP);
					move_cursor_up |= !!(stick.y > 0.5f);
				int move_cursor_down  = fw64_input_controller_button_pressed(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_DOWN);
					move_cursor_down |= fw64_input_controller_button_pressed(menu->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_DOWN);
					move_cursor_down |= !!(stick.y < -0.5f);

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
		default:
			break;
	}
}

void advance_one_intro(Menu* menu) {
	switch(menu->current_menu) {
		case MENU_SCREEN_INTRO_JAMLOGO:
			jamlogo_uninit(menu);
			teamlogo_init(menu);
			break;
		case MENU_SCREEN_INTRO_TEAMLOGO:
			teamlogo_uninit(menu);
			scarylogo_init(menu);
			break;
		case MENU_SCREEN_INTRO_SCARYLOGO:
			scarylogo_uninit(menu);
			main_menu_init(menu);
			break;			
		default:
			break;
	}
}

void skip_all_intros(Menu* menu) {
	switch(menu->current_menu) {
		case MENU_SCREEN_INTRO_JAMLOGO:
			jamlogo_uninit(menu);
			main_menu_init(menu);
			break;
		case MENU_SCREEN_INTRO_TEAMLOGO:
			teamlogo_uninit(menu);
			main_menu_init(menu);
			break;
		case MENU_SCREEN_INTRO_SCARYLOGO:
			scarylogo_uninit(menu);
			main_menu_init(menu);
			break;			
		default:
			break;
	}
}

void menu_init_audio(Menu* menu) {
    menu->sound = fw64_sound_bank_load(menu->engine->assets, FW64_ASSET_soundbank_sounds, menu->allocator);
    fw64_audio_set_sound_bank(menu->engine->audio, menu->sound);
    audio_controller_init(&menu->audio_controller, menu->engine->audio);

}

void process_audio(Menu* menu) {
	// really, just loop menu music if applicable
	if(menu->current_menu < MENU_SCREEN_MAIN)
		return;

	if(menu->sound) {
		if(!audio_controller_channel_is_playing(&menu->audio_controller, AUDIO_CONTROLLER_ENVIRONMENT))
			audio_controller_play(&menu->audio_controller, AUDIO_CONTROLLER_ENVIRONMENT, sound_bank_sounds_Menu_Song2);
	}
		
}

void main_menu_draw(Menu* menu) {
	int x = 20;
	int y = 130;
	int y_advance = 24;
	char text[20] = {0};
	sprintf(text, "Start Game");
	fw64_renderer_draw_text(menu->engine->renderer, menu->font, x, y, text);

	sprintf(text, "Controls");
	fw64_renderer_draw_text(menu->engine->renderer, menu->font, x, y + y_advance, text);

	sprintf(text, "Practice Range");
	fw64_renderer_draw_text(menu->engine->renderer, menu->font, x, y + (y_advance * 2), text);

}

void controls_menu_draw(Menu* menu) {

}