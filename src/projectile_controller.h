#pragma once

#include "framework64/level.h"
#include "weapon.h"
#include "ray.h"

typedef struct {
    fw64Level* level;
} ProjectileController;

void projectile_controller_init(ProjectileController* controller, fw64Level* level);
void projectile_controller_fire_ray(ProjectileController* controller, Vec3* origin, Vec3* direction, WeaponType weapon_type);
void projectile_controller_fire_arc(ProjectileController* controller, Vec3* origin, Vec3* direction, float distance, float angle_degrees, WeaponType weapon_type);