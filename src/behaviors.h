#pragma once

/** \file behaviors.h */

#include "framework64/vec3.h"

typedef enum {
	// EACH VALUE IS A BIT IN A BIT SET

	SB_SEEK				 = (1 << 0), // move towards a position
	SB_FLEE				 = (1 << 1), // move away from a position
	SB_ARRIVE			 = (1 << 2), // seek, slow to stop at destination
	SB_PURSUE			 = (1 << 3), // seek moving object
	SB_EVADE			 = (1 << 4), // flee moving object
	SB_WANDER			 = (1 << 5), // seek, continuously updating seek target
	SB_FOLLOW_PATH		 = (1 << 6), // seek waypoints in some order
	SB_AVOID_COLLISION	 = (1 << 7), // flee nearest object in path
	SB_ALIGN			 = (1 << 8), // turn to face same direction as peers
	SB_FLOCK			 = (1 << 9), // stay with group

	// IF YOU ADD MORE BEHAVIORS, UPDATE THESE VALUES
	// Power of 2 totals will cause conflict!
	SB_TOTAL_BEHAVIORS   = 10,
	SB_NONE				 = 0,
	SB_ALL				 = 0x3FF
} SteeringBehavior;

void steering_seek(Vec3* position, Vec3* target, float strength, Vec3* out);
void steering_flee(Vec3* position, Vec3* target, float strength, Vec3* out);

void steering_arrive(Vec3* position, float strength, Vec3* out); // stubbed
void steering_pursue(Vec3* position, float strength, Vec3* out); // stubbed
void steering_evade(Vec3* position, float strength, Vec3* out); // stubbed
void steering_wander(Vec3* position, float strength, Vec3* out); // stubbed
void steering_follow_path(Vec3* position, float strength, Vec3* out); // stubbed
void steering_avoid_collision(Vec3* position, float strength, Vec3* out); // stubbed
void steering_align(Vec3* position, float strength, Vec3* out); // stubbed
void steering_flock(Vec3* position, float strength, Vec3* out); // stubbed