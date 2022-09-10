#pragma once

/** \file behaviors.h */

#include "framework64/vec3.h"

#define DEFAULT_MAX_LIN_ACCEL 5.0f
#define DEFAULT_MAX_ANG_ACCEL 1.0f

typedef enum {
	// EACH VALUE IS A BIT IN A BIT SET
	SB_NONE				 = 0,
	SB_SEEK				 = (1 << 0),  // move towards a position
	SB_FLEE				 = (1 << 1),  // move away from a position
	SB_ARRIVE			 = (1 << 2),  // seek, slow to stop at destination
	SB_PURSUE			 = (1 << 3),  // seek moving object
	SB_EVADE			 = (1 << 4),  // flee moving object
	SB_WANDER			 = (1 << 5),  // seek, continuously updating/randomizing seek target
	SB_FOLLOW_PATH		 = (1 << 6),  // seek waypoints in some order
	SB_AVOID_COLLISION	 = (1 << 7),  // predict future location of moving actor and flee to avoid
	SB_AVOID_OBSTACLE	 = (1 << 8),  // flee nearest fixed obstacle in path
	SB_SEPARATE			 = (1 << 9),  // prevent getting too close to nearby peers
	SB_COHESION			 = (1 << 10), // prevent getting too far from nearby peers
	SB_ALIGN			 = (1 << 11), // turn to face same average direction as peers
	SB_FOLLOW_LEADER	 = (1 << 12), // select a "leader" peer and follow their course
	SB_FLOCK			 = (1 << 13), // move with group

	// IF YOU ADD MORE BEHAVIORS, UPDATE THESE VALUES
	SB_TOTAL_BEHAVIORS   = 14,	
	SB_ALL				 = 0x3FFF // all bits in current use
} SteeringBehavior;

typedef struct {

	Vec3 position;
	Vec3 targetPosition;
	Vec3* linearVel;
	float deltaTime;
	Vec3 linearAccel;
	float angularAccel;
	float maxLinearAccel;  
	float maxAngularAccel;
} SteeringBehaviorData;

void steering_behavior_data_init(Vec3 position, Vec3 targetPosition, Vec3* linearVelocity, float deltaTime, SteeringBehaviorData* data);
void steering_seek(float strength, SteeringBehaviorData* data);
void steering_flee(float strength, SteeringBehaviorData* data);
void steering_arrive(float slowRadius, float stopRadius, float strength, SteeringBehaviorData* data); // slowing not implemented
void steering_pursue(Vec3* targetVelocity, float strength, SteeringBehaviorData* data);
void steering_evade(Vec3* targetVelocity, float strength, SteeringBehaviorData* data);
void steering_wander(Vec3* position, float strength, Vec3* out); // stubbed
void steering_follow_path(Vec3* position, float strength, Vec3* out); // stubbed
void steering_avoid_collision(Vec3* position, float strength, Vec3* out); // stubbed
void steering_align(Vec3* position, float strength, Vec3* out); // stubbed
void steering_flock(Vec3* position, float strength, Vec3* out); // stubbed