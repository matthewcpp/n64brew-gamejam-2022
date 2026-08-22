#include "compass.h"
#include "assets/assets.h"
#include "framework64/math.h"

void compass_init(Compass* compass, fw64Engine* engine, fw64Allocator* allocator, fw64Transform* player_pos) {
	compass->engine = engine;
	compass->allocator = allocator;
	compass->player_pos = player_pos;

	fw64Image* bg_image = fw64_assets_load_image(engine->assets, FW64_ASSET_image_compass_bg, compass->allocator);
	compass->compass_bg = fw64_texture_create_from_image(bg_image, compass->allocator);
	fw64Image* home_image = fw64_assets_load_image(engine->assets, FW64_ASSET_image_home, compass->allocator);
    compass->home_icon = fw64_texture_create_from_image(home_image, compass->allocator);
	fw64Image* north_image = fw64_assets_load_image(engine->assets, FW64_ASSET_image_north, compass->allocator);
	compass->north_icon = fw64_texture_create_from_image(north_image, compass->allocator);
	compass->turn_compass = 1;

	fw64_spritebatch_create(1, allocator);
}

void compass_uninit(Compass* compass) {
	fw64_image_delete(compass->engine->assets, fw64_texture_get_image(compass->compass_bg), compass->allocator);
    fw64_texture_delete(compass->compass_bg, compass->allocator);
	fw64_image_delete(compass->engine->assets, fw64_texture_get_image(compass->home_icon), compass->allocator);
    fw64_texture_delete(compass->home_icon, compass->allocator);
	fw64_image_delete(compass->engine->assets, fw64_texture_get_image(compass->north_icon), compass->allocator);
    fw64_texture_delete(compass->north_icon, compass->allocator);

	fw64_spritebatch_delete(compass->spritebatch);
}

void compass_draw(Compass* compass, fw64RenderPass* renderpass) {
	static Vec3 home_pos = { 0.0f, 0.0f, 0.0f };
	static Vec3 north = { 0.0f, 0.0f, -1.0f };
	static float radius = 14.0f;
	static Vec2 compass_pos = { 36.0f, 204.0f }; // 36 from the edges of the screen
	                                             // offset by -8,-8 to account for drawing the 16x16 images from their top left corner.
	Vec3 temp_north;
	vec3_copy(&north, &temp_north);	
	
	Vec3 home_dir;
	Vec3 ref_zero = { 0.0f, 0.0f, 0.0f };
	vec3_copy(&compass->player_pos->position, &home_dir);
	home_dir.y = 0;
	vec3_negate(&home_dir);
	vec3_normalize(&home_dir);
	vec3_scale(&home_dir, fw64_minf(1.0f, vec3_distance(&compass->player_pos->position, &ref_zero)/100.0f ) * radius, &home_dir);
	if(compass->turn_compass) {
		Quat compass_rotate;
		quat_set(&compass_rotate, 0.0f, -compass->player_pos->rotation.y, 0.0f, compass->player_pos->rotation.w);
		quat_transform_vec3(&compass_rotate, &temp_north, &temp_north);
		quat_transform_vec3(&compass_rotate, &home_dir, &home_dir);
	}

	fw64_spritebatch_begin(compass->spritebatch);
	fw64_spritebatch_set_color(compass->spritebatch, 255, 255, 255, 200);
	fw64_spritebatch_draw_sprite(compass->spritebatch, compass->compass_bg, compass_pos.x - 16.0f, compass_pos.y - 16.0f);
	fw64_spritebatch_set_color(compass->spritebatch, 64, 128, 64, 200);
	fw64_spritebatch_draw_sprite(compass->spritebatch, compass->north_icon, compass_pos.x - 8.0f + (temp_north.x * radius), compass_pos.y - 8.0f + (temp_north.z * radius));
	fw64_spritebatch_set_color(compass->spritebatch, 64, 64, 128, 200);
	fw64_spritebatch_draw_sprite(compass->spritebatch, compass->home_icon, compass_pos.x - 8.0f + home_dir.x, compass_pos.y - 8.0f + home_dir.z);
	fw64_spritebatch_end(compass->spritebatch);

	fw64_renderpass_draw_sprite_batch(renderpass, compass->spritebatch);
}