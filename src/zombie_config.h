#pragma once

#include <framework64/mesh.h>

typedef struct {
    fw64Image* face_image;
    int shirt_palette;
    int pants_palette;
} ZombieConfig;

void zombie_config_init(ZombieConfig* config, fw64Image* face_image, int shirt_palette, int pants_palette);
void zombie_config_apply(ZombieConfig* config, fw64Mesh* mesh);