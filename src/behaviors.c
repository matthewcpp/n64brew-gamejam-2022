#include "behaviors.h"

void steering_seek(Vec3* position, Vec3* target, float strength, Vec3* out) {
	vec3_subtract(out, target, position);
	vec3_normalize(out);
	vec3_scale(out, out, strength);
}

void steering_flee(Vec3* position, Vec3* target, float strength, Vec3* out) {
	vec3_subtract(out, position, target);
	vec3_normalize(out);
	vec3_scale(out, out, strength);
}

void steering_arrive(Vec3* position, float strength, Vec3* out){} // stubbed
void steering_pursue(Vec3* position, float strength, Vec3* out){} // stubbed
void steering_evade(Vec3* position, float strength, Vec3* out){} // stubbed
void steering_wander(Vec3* position, float strength, Vec3* out){} // stubbed
void steering_follow_path(Vec3* position, float strength, Vec3* out){} // stubbed
void steering_avoid_collision(Vec3* position, float strength, Vec3* out){} // stubbed
void steering_align(Vec3* position, float strength, Vec3* out){} // stubbed
void steering_flock(Vec3* position, float strength, Vec3* out){} // stubbed
