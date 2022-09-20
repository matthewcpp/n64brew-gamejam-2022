#include "projectile_controller.h"

#include "assets/layers.h"

#include "zombie.h"

#include <math.h>
#include <string.h>

void projectile_controller_init(ProjectileController* controller, fw64Level* level) {
    controller->level = level;
}

// note: currently this is only configured to shoot zombies...maybe in the future we would want to put bullet holes, etc
void projectile_controller_fire_ray(ProjectileController* controller, Vec3* origin, Vec3* direction, WeaponType weapon_type) {
    uint32_t dynamic_node_count = fw64_level_get_dynamic_node_count(controller->level);
    float closest_dist = FLT_MAX;
    Zombie* closest_nerd = NULL;
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
                if(bounding_hit && (out_distance < closest_dist)) {                    
                    closest_dist = out_distance;
                    closest_nerd = zombie;
                } 
            }          
        }
    }
    if(closest_nerd != NULL) {
        zombie_hit(closest_nerd, weapon_type);
    }
}

void projectile_controller_fire_arc(ProjectileController* controller, Vec3* origin, Vec3* direction, float distance, float angle_degrees, WeaponType weapon_type) {
    float distance_squared = distance * distance;
    float angle_radians = angle_degrees * (M_PI / 180.0f);
    int hit_count = 0; // temp?
    
    uint32_t dynamic_node_count = fw64_level_get_dynamic_node_count(controller->level);

    for (uint32_t i = 0; i < dynamic_node_count; i++) {
        fw64Node* dynamic_node = fw64_level_get_dynamic_node(controller->level, i);

        if (!(dynamic_node->layer_mask & ZOMBIE_LAYER)) 
            continue;

        Zombie* zombie = (Zombie*)dynamic_node->data;

        if (zombie->health <= 0 || zombie->state == ZOMBIE_FLYING_BACK) {
            continue;
        }

        Vec3 zombie_center, zombie_dir;
        box_center(&dynamic_node->collider->bounding, &zombie_center);

        if (vec3_distance_squared(&zombie_center, origin) > distance_squared)
            continue;

        vec3_subtract(&zombie_dir,  &zombie_center, origin);
        vec3_normalize(&zombie_dir);

        float d = vec3_dot(direction, &zombie_dir);
        
        if (d < 0.0f)
            continue;
        
        float theta = fw64_acosf(d);
        if (theta <= angle_radians) {
            zombie_hit(zombie, weapon_type);// todo: zombie hit should probably also take distance into account!

            if (++hit_count == 3)
                break;
        }
    }
}
