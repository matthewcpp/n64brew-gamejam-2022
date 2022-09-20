#pragma once

#include "level.h"
#include "weapon.h"
#include "ray.h"

typedef struct {
    fw64Level* level;
} ProjectileController;

void projectile_controller_init(ProjectileController* controller, fw64Level* level);
void projectile_controller_fire_ray(ProjectileController* controller, Vec3* origin, Vec3* direction, WeaponType weapon_type);