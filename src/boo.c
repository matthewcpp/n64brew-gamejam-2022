#include "boo.h"

#include "scene_spooky_level.h"

void boo_init(Boo* boo, fw64Engine* engine, Player* player, fw64Scene* scene) {
    boo->engine = engine;
    boo->player = player;
    boo->node = fw64_scene_get_node(scene, FW64_scene_spooky_level_node_boo);
    boo->mesh = boo->node->mesh;
    boo->state = BOO_WAITING;
    fw64_node_set_mesh(boo->node, NULL);
}

static void boo_update_waiting(Boo* boo) {
    float distance_squared = vec3_distance_squared(&boo->player->camera.camera.transform.position, &boo->node->transform.position);
    if (distance_squared <= 1500.0f) {
        fw64_audio_play_sound(boo->engine->audio, 0);
        boo->state = BOO_APPEARING;
        fw64_node_set_mesh(boo->node, boo->mesh);
    }
}

void boo_update(Boo* boo) {
    switch(boo->state) {
        case BOO_WAITING:
            boo_update_waiting(boo);
            break;
    }
}