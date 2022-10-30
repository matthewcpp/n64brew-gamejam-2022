#include "mesh_collection.h"

#include "assets/layers.h"

#include <string.h>

void mesh_collection_init(MeshCollection* collection, fw64AssetDatabase* assets, int source_scene_index, uint32_t replacement_layer_mask, fw64Allocator* allocator) {
    memset(collection, 0 , sizeof(MeshCollection));
    collection->assets = assets;
    collection->replacement_layer_mask = replacement_layer_mask;
    collection->source_scene = fw64_scene_load(assets, source_scene_index, allocator);

    uint32_t node_count = fw64_scene_get_node_count(collection->source_scene);
    for (uint32_t i = 0; i < node_count; i++) {
        fw64Node* node = fw64_scene_get_node(collection->source_scene, i);

        if (!node->mesh)
            continue;
        
        uint32_t source_index = (uint32_t)node->data;

        if (source_index > 0 && source_index < MESH_COLLECTION_CAPACITY) {
            collection->mesh_nodes[source_index] = node;
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

        if (!(node->layer_mask & collection->replacement_layer_mask)) {
            continue;
        }

        uint32_t data_index = (uint32_t)node->data;

        if (data_index == 0 || data_index >= MESH_COLLECTION_CAPACITY)
            continue;

        fw64Node* mesh_node = collection->mesh_nodes[data_index];
        fw64_node_set_mesh(node, mesh_node->mesh);

        if (!node->collider)
            continue;

        switch(mesh_node->collider->type) {
            case FW64_COLLIDER_BOX:
                fw64_collider_set_type_box(node->collider, &mesh_node->collider->source.box);
                break;

            case FW64_COLLIDER_MESH:
                fw64_collider_set_type_mesh(node->collider, mesh_node->collider->source.mesh);
                break;

            default:
                fw64_collider_set_type_none(node->collider);
        }
        
    }

    fw64_scene_update_bounding(scene);
}