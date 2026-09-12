#pragma once

#include <framework64/engine.h>

#define ZOMBIE_FACE_PRIM_INDEX 12
#define ZOMBIE_SHIRT_PRIM_INDEX 0
#define ZOMBIE_PANTS_PRIM_INDEX 5

#define ZOMBIE_FACE_IMAGE_COUNT 4

typedef struct {
    fw64Engine* engine;
    fw64Allocator* allocator;
    fw64Image* face_images[ZOMBIE_FACE_IMAGE_COUNT];
} ZombieAppearance;

void zombie_appearance_init(ZombieAppearance* appearance, fw64Engine* engine, fw64SkinnedMeshInstance* zombie_instance, fw64Allocator* allocator);
void zombie_appearance_uninit(ZombieAppearance* appearance);

void zombie_appearance_set_face_image(ZombieAppearance* appearance, fw64MeshInstance* mesh_instance, int face_image_index);
void zombie_appearance_set_shirt_palette(ZombieAppearance* appearance, fw64MeshInstance* mesh_instance, int shirt_palette);
void zombie_appearance_set_pants_palette(ZombieAppearance* appearance, fw64MeshInstance* mesh_instance, int pants_palette);