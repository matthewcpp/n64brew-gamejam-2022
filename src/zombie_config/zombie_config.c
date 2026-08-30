#include "zombie_config.h"

#include "assets/zombie_image_texture_defs.h"

void zombie_config_init(ZombieConfig* config, fw64Image* face_image, int shirt_palette, int pants_palette) {
    config->face_image = face_image;
    config->shirt_palette = shirt_palette;
    config->pants_palette = pants_palette;
}

void zombie_config_apply(ZombieConfig* config, fw64Mesh* mesh) {
    fw64Texture* texture = fw64_mesh_get_texture(mesh, zombie_image_Z_Face01_texture_0);
    fw64_texture_set_image(texture, config->face_image);

    texture = fw64_mesh_get_texture(mesh, zombie_image_Z_TopFront_texture_0);
    fw64_texture_set_palette_index(texture, config->shirt_palette);

    texture = fw64_mesh_get_texture(mesh, zombie_image_Z_TopBack_texture_0);
    fw64_texture_set_palette_index(texture, config->shirt_palette);

    texture = fw64_mesh_get_texture(mesh, zombie_image_Z_Top_ArmLong_texture_0);
    fw64_texture_set_palette_index(texture, config->shirt_palette);

    texture = fw64_mesh_get_texture(mesh, zombie_image_Z_Top_ArmLong_texture_1);
    fw64_texture_set_palette_index(texture, config->shirt_palette);

    texture = fw64_mesh_get_texture(mesh, zombie_image_Z_JeanTop_texture_0);
    fw64_texture_set_palette_index(texture, config->pants_palette);

    texture = fw64_mesh_get_texture(mesh, zombie_image_Z_JeanLegR_texture_0);
    fw64_texture_set_palette_index(texture, config->pants_palette);

    texture = fw64_mesh_get_texture(mesh, zombie_image_Z_JeanLegL_texture_0);
    fw64_texture_set_palette_index(texture, config->pants_palette);
}
