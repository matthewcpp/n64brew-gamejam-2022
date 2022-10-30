#pragma once 

#include "framework64/scene.h"

#define MESH_COLLECTION_CAPACITY 16

typedef struct {
    fw64AssetDatabase* assets;
    fw64Scene* source_scene;
    fw64Node* mesh_nodes[MESH_COLLECTION_CAPACITY];
    uint32_t replacement_layer_mask;
} MeshCollection;

void mesh_collection_init(MeshCollection* collection, fw64AssetDatabase* assets, int source_scene_index, uint32_t replacement_layer_mask, fw64Allocator* allocator);
void mesh_collection_uninit(MeshCollection* collection);

void mesh_collection_set_scene_meshes(MeshCollection* collection, fw64Scene* scene);