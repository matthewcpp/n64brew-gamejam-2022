#include "projectile_controller.h"

#include "assets/layers.h"

#include "zombie.h"

#include <string.h>

void projectile_controller_init(ProjectileController* controller, fw64Level* level) {
    controller->level = level;
}

// note: currently this is only configured to shoot zombies...maybe in the future we would want to put bullet holes, etc
void projectile_controller_fire_ray(ProjectileController* controller, Vec3* origin, Vec3* direction, WeaponType weapon_type) {
    uint32_t dynamic_node_count = fw64_level_get_dynamic_node_count(controller->level);
    for (uint32_t i = 0; i < dynamic_node_count; i++) {
        fw64Node* dynamic_node = fw64_level_get_dynamic_node(controller->level, i);

        if (dynamic_node->layer_mask & ZOMBIE_LAYER) {
            Zombie* zombie = (Zombie*)dynamic_node->data;
            if (zombie->health > 0 && zombie->state != ZOMBIE_FLYING_BACK) {
                Vec3 out_hitpoint;
                float out_distance;
                int bounding_hit = fw64_collision_test_ray_box( origin,
                                                                direction,
                                                                &zombie->node.collider->bounding,
                                                                &out_hitpoint,
                                                                &out_distance);
                if(bounding_hit) {                    
                    zombie_hit(zombie, weapon_type);
                    break;
                } 
            }          
        }
    }
}
