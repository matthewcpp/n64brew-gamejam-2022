#pragma once

#include <framework64/mesh_instance.h>

#define ZOMBIE_FACE_PRIM_INDEX 12
#define ZOMBIE_SHIRT_PRIM_INDEX 0
#define ZOMBIE_PANTS_PRIM_INDEX 5

typedef struct {
    

} ZombieConfig;

void zombie_config_set_face_image(fw64MeshInstance* mesh_instance, fw64Image* face_image);
void zombie_config_set_shirt_palette(fw64MeshInstance* mesh_instance, int shirt_palette);
void zombie_config_set_pants_palette(fw64MeshInstance* mesh_instance, int pants_palette);