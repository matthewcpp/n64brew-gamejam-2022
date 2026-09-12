#include "zombie_appearance.h"


void zombie_appearance_set_face_image(fw64MeshInstance* mesh_instance, fw64Image* face_image) {
    fw64MaterialCollection* material_collection = fw64_mesh_instance_get_material_collection(mesh_instance);
    fw64Material* face_material = fw64_material_collection_get_material(material_collection, ZOMBIE_FACE_PRIM_INDEX);
    fw64Texture* face_texture = fw64_material_get_texture(face_material);
    fw64_texture_set_image(face_texture, face_image);
}

#define SHIRT_PRIM_INDICIES_COUNT 6
static const int shirt_prim_indicies[SHIRT_PRIM_INDICIES_COUNT] = {
    0, 3, 15, 16, 17, 18
};

#define PANTS_PRIM_INDICIES_COUNT 7
static const int pants_prim_indices[PANTS_PRIM_INDICIES_COUNT] = {
    5, 6, 7, 8, 9, 10, 11
};

static void zombie_appearance_set_prim_material_palettes(fw64MeshInstance* mesh_instance, const int* indices, int count, int palette_index) {
    fw64MaterialCollection* material_collection = fw64_mesh_instance_get_material_collection(mesh_instance);

    for (int i = 0; i < count; i++) {
        fw64Material* material = fw64_material_collection_get_material(material_collection, indices[i]);
        fw64_texture_set_palette_index(fw64_material_get_texture(material), palette_index);
    }
}

void zombie_appearance_set_shirt_palette(fw64MeshInstance* mesh_instance, int shirt_palette) {
    zombie_appearance_set_prim_material_palettes(mesh_instance, &shirt_prim_indicies[0], SHIRT_PRIM_INDICIES_COUNT, shirt_palette);
}

void zombie_appearance_set_pants_palette(fw64MeshInstance* mesh_instance, int pants_palette) {
    zombie_appearance_set_prim_material_palettes(mesh_instance, &pants_prim_indices[0], PANTS_PRIM_INDICIES_COUNT, pants_palette);
}
