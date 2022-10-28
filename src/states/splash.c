#include "states/splash.h"
#include "framework64/texture.h"

#include "assets/assets.h"

#define LEVEL_MEMORY_POOL_SIZE (400 * 1024)
#define SPLASH_STATE_DURATION 3.0f

static void transition_to_state(Splash* splash, SplashState state);
static void transition_to_next(Splash* splash);

void game_state_splash_init(Splash* splash, fw64Engine* engine, GameData* game_data) {
    splash->engine = engine;
    splash->game_data = game_data;
    fw64_camera_init(&splash->camera);

    fw64_bump_allocator_init(&splash->bump_allocator, LEVEL_MEMORY_POOL_SIZE);

    splash->image_tex = NULL;

    splash->current_state = SPLASH_STATE_NONE;
    transition_to_state(splash, SPLASH_STATE_JAMLOGO);
}

void transition_to_state(Splash* splash, SplashState state) {
    if (splash->image_tex  != NULL) {
        fw64_image_delete(splash->engine->assets, fw64_texture_get_image(splash->image_tex), &splash->bump_allocator.interface);
        fw64_texture_delete(splash->image_tex, &splash->bump_allocator.interface);
        fw64_bump_allocator_reset(&splash->bump_allocator);
    }

    splash->current_state = state;
    splash->current_state_time = 0.0f;
    
    fw64AssetId image_id = FW64_INVALID_ASSET_ID;

    switch (splash->current_state) {
        case SPLASH_STATE_JAMLOGO:
            image_id = FW64_ASSET_image_jam_logo;
            break;

        case SPLASH_STATE_TEAMLOGO:
            image_id = FW64_ASSET_image_team_logo;
            break;

        case SPLASH_STATE_SCARYLOGO:
            image_id = FW64_ASSET_image_scary_logo;
            break;
        case SPLASH_STATE_NONE:
            break;
    }

    if (image_id == FW64_INVALID_ASSET_ID) {
        splash->image_tex = NULL;
    }
    else {
        fw64Image* image = fw64_image_load(splash->engine->assets, image_id, &splash->bump_allocator.interface);
        splash->image_tex = fw64_texture_create_from_image(image, &splash->bump_allocator.interface);
    }
}

static void transition_to_next(Splash* splash) {
    if (splash->current_state < SPLASH_STATE_SCARYLOGO) {
        transition_to_state(splash, splash->current_state + 1);
    }
    else {
        splash->game_data->transition_to_state = GAME_STATE_LEVEL_SELECT;
    }
}

void game_state_splash_uninit(Splash* splash) {
    transition_to_state(splash, SPLASH_STATE_NONE);
    fw64_bump_allocator_uninit(&splash->bump_allocator);
}

void game_state_splash_update(Splash* splash) {
    int skip_one  = fw64_input_controller_button_pressed(splash->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_A);
    skip_one |= fw64_input_controller_button_pressed(splash->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_B);
    skip_one |= fw64_input_controller_button_pressed(splash->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_Z);

    int skip_all = fw64_input_controller_button_pressed(splash->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START);

    if (skip_all) {
        splash->game_data->transition_to_state = GAME_STATE_LEVEL_SELECT;
        return;
    }

    splash->current_state_time += splash->engine->time->time_delta;

    if (skip_one || splash->current_state_time >= SPLASH_STATE_DURATION) {
        transition_to_next(splash);
        return;
    }
}

void game_state_splash_draw(Splash* splash) {
    fw64Renderer* renderer = splash->engine->renderer;

    fw64_renderer_begin(renderer, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_draw_sprite(renderer, splash->image_tex, 0, 0);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}