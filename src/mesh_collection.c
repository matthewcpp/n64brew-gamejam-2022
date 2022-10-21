#include "mesh_collection.h"

#include <string.h>

void mesh_collection_init(MeshCollection* collection, fw64AssetDatabase* assets, int source_scene_index, fw64Allocator* allocator) {
    memset(collection, 0 , sizeof(MeshCollection));
    collection->assets = assets;
    collection->source_scene = fw64_scene_load(assets, source_scene_index, allocator);

    uint32_t node_count = fw64_scene_get_node_count(collection->source_scene);
    for (uint32_t i = 0; i < node_count; i++) {
        fw64Node* node = fw64_scene_get_node(collection->source_scene, i);

        if (!node->mesh)
            continue;
        
        uint32_t source_index = (uint32_t)node->data;

        if (source_index > 0 && source_index < MESH_COLLECTION_CAPACITY) {
            collection->meshes[source_index] = node->mesh;
        }
    }
}

void mesh_collection_uninit(MeshCollection* collection) {
    fw64_scene_delete(collection->assets, collection->source_scene, fw64_scene_get_allocator(collection->source_scene));
}

void mesh_collection_set_scene_meshes(MeshCollection* collection, fw64Scene* scene) {
    uint32_t node_count = fw64_scene_get_node_count(scene);

    for (uint32_t i = 0; i < node_count; i++) {
        fw64Node* node = fw64_scene_get_node(scene, i);

        uint32_t data_index = (uint32_t)node->data;

        if (data_index == 0 || data_index >= MESH_COLLECTION_CAPACITY)
            continue;

        fw64Mesh* mesh = collection->meshes[data_index];
        fw64_node_set_mesh(node, mesh);

        if (!node->collider)
            return;

        Box mesh_bounding;
        fw64_mesh_get_bounding_box(mesh, &mesh_bounding);
        fw64_collider_set_type_box(node->collider, &mesh_bounding);
    }

    fw64_scene_update_bounding(scene);
}