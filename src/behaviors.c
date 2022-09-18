#include "behaviors.h"
#include "assets/layers.h"
#include "framework64/random.h"
#include "framework64/math.h"

static void steering_behavior_data_zero(SteeringBehaviorData* data) {
	vec3_zero(&data->linearAccel);
	data->angularAccel = 0.0f;
}

void steering_behavior_data_init(fw64Level* level,
								 fw64Collider* collider,
								 Vec3 position,
								 Vec3 targetPosition,
								 Vec3* linearVelocity,
								 float deltaTime,
								 SteeringBehaviorData* data) {
	steering_behavior_data_zero(data);
	data->level = level;
	data->maxLinearAccel = DEFAULT_MAX_LIN_ACCEL;
	data->maxAngularAccel = DEFAULT_MAX_ANG_ACCEL;
	data->deltaTime = deltaTime;
	data->collider = collider;
	data->position = position;
	data->targetPosition = targetPosition;
	data->linearVel = linearVelocity;
}

// Max acceleration directly towards a given point
void steering_seek(float strength, SteeringBehaviorData* data) {
	data->angularAccel = 0.0f;
	Vec3 seekAccel;
	vec3_subtract(&seekAccel, &data->targetPosition, &data->position);
	vec3_normalize(&seekAccel);
	vec3_scale(&seekAccel, &seekAccel, data->maxLinearAccel);
	vec3_add_and_scale(&data->linearAccel, &data->linearAccel, &seekAccel, strength);
}

// Max acceleration directly away from a given point
void steering_flee(float strength, SteeringBehaviorData* data) {
	data->angularAccel = 0.0f;
	Vec3 fleeAccel;
	vec3_subtract(&fleeAccel, &data->position, &data->targetPosition);
	vec3_normalize(&fleeAccel);
	vec3_scale(&fleeAccel, &fleeAccel, data->maxLinearAccel);
	vec3_add_and_scale(&data->linearAccel, &data->linearAccel, &fleeAccel, strength);
}

// seek towards a given point, slow to a stop when getting close
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

// seek towards the target's estimated future position
void steering_pursue(Vec3* targetVelocity, float strength, SteeringBehaviorData* data){
	vec3_add(&data->targetPosition, &data->targetPosition, targetVelocity);
	data->targetPosition.y = data->position.y;
	steering_seek(strength, data);
}

// flee from the target's estimated future position
void steering_evade(Vec3* targetVelocity, float strength, SteeringBehaviorData* data){
	vec3_add(&data->targetPosition, &data->targetPosition, targetVelocity);
	data->targetPosition.y = data->position.y;
	steering_flee(strength, data);
}

void steering_wander(float strength, SteeringBehaviorData* data) {
	steering_arrive(2.0f, 1.0f, strength, data); // TODO: add some mindless slow turning / zig zagging too
} 
void steering_follow_path(Vec3* position, float strength, Vec3* out){} // stubbed
void steering_avoid_collision(float strength, SteeringBehaviorData* data) {
	float avoidDist = 5.0f;
	float targetDist = vec3_distance(&data->position, &data->targetPosition);
	float lookAhead = 30.0f;
	Vec3 pos;
	vec3_copy(&pos, &data->position);
	pos.y -= 1.0f;
	Vec3 vel;
	vec3_copy(&vel, data->linearVel);
	//vec3_normalize(&vel);

	fw64IntersectMovingBoxQuery query;
	uint32_t mask = (uint32_t)(FW64_layer_obstacles | FW64_layer_wall);
	if(fw64_level_moving_box_intersection(data->level, &data->collider->bounding, &vel, mask, &query)) {
        if(query.results[0].tfirst > lookAhead)
			return;
		if(vec3_distance(&data->collider->transform->position, &query.results[0].node->collider->transform->position) > targetDist)
			return;

		Vec3 collidePosition, obstaclePos, avoidDir;
		vec3_normalize(&vel);
		Vec3 hit_pos;
		vec3_add_and_scale(&hit_pos, &pos, &vel, query.results[0].tfirst);
		vec3_copy(&obstaclePos, &query.results[0].node->collider->transform->position);
		vec3_add_and_scale(&collidePosition, &pos, &vel, query.results[0].tfirst);
		vec3_subtract(&avoidDir, &collidePosition, &obstaclePos);
		vec3_normalize(&avoidDir);
		
		vec3_add_and_scale(&data->linearAccel, &data->linearAccel, &avoidDir, data->maxLinearAccel * strength);
    }
}
void steering_align(Vec3* position, float strength, Vec3* out){} // stubbed
void steering_flock(Vec3* position, float strength, Vec3* out){} // stubbed
