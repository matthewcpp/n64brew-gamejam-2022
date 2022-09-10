#include "behaviors.h"

static void steering_behavior_data_zero(SteeringBehaviorData* data) {
	vec3_zero(&data->linearAccel);
	data->angularAccel = 0.0f;
}

void steering_behavior_data_init(Vec3 position, Vec3 targetPosition, Vec3* linearVelocity, float deltaTime, SteeringBehaviorData* data) {
	steering_behavior_data_zero(data);
	data->maxLinearAccel = DEFAULT_MAX_LIN_ACCEL;
	data->maxAngularAccel = DEFAULT_MAX_ANG_ACCEL;
	data->deltaTime = deltaTime;
	data->position = position;
	data->targetPosition = targetPosition;
	data->linearVel = linearVelocity;
}

void steering_seek(float strength, SteeringBehaviorData* data) {
	data->angularAccel = 0.0f;
	Vec3 seekAccel;
	vec3_subtract(&seekAccel, &data->targetPosition, &data->position);
	vec3_normalize(&seekAccel);
	vec3_scale(&seekAccel, &seekAccel, data->maxLinearAccel);
	vec3_add_and_scale(&data->linearAccel, &data->linearAccel, &seekAccel, strength);
}

void steering_flee(float strength, SteeringBehaviorData* data) {
	data->angularAccel = 0.0f;
	Vec3 fleeAccel;
	vec3_subtract(&fleeAccel, &data->position, &data->targetPosition);
	vec3_normalize(&fleeAccel);
	vec3_scale(&fleeAccel, &fleeAccel, data->maxLinearAccel);
	vec3_add_and_scale(&data->linearAccel, &data->linearAccel, &fleeAccel, strength);
}

void steering_arrive(float slowRadius, float stopRadius, float strength, SteeringBehaviorData* data) {
	data->angularAccel = 0.0f;
	
	float currentDistance = vec3_distance(&data->targetPosition, &data->position);
	if(currentDistance <= (stopRadius)) { 	// we have arrived
		steering_behavior_data_zero(data); 	//zero out our steering accelerations
		vec3_zero(data->linearVel); 		//zero out host velocity
		return;
	}
	if(currentDistance <= (slowRadius)) {
		steering_seek(strength, data); //TODO: implement slowing down
	}
	else {
		steering_seek(strength, data);
	}
}

void steering_pursue(Vec3* targetVelocity, float strength, SteeringBehaviorData* data){
	vec3_add(&data->targetPosition, &data->targetPosition, targetVelocity);
	data->targetPosition.y = data->position.y;
	steering_seek(strength, data);
}

void steering_evade(Vec3* targetVelocity, float strength, SteeringBehaviorData* data){
	vec3_add(&data->targetPosition, &data->targetPosition, targetVelocity);
	data->targetPosition.y = data->position.y;
	steering_flee(strength, data);
}

void steering_wander(Vec3* position, float strength, Vec3* out){} // stubbed
void steering_follow_path(Vec3* position, float strength, Vec3* out){} // stubbed
void steering_avoid_collision(Vec3* position, float strength, Vec3* out){} // stubbed
void steering_align(Vec3* position, float strength, Vec3* out){} // stubbed
void steering_flock(Vec3* position, float strength, Vec3* out){} // stubbed
