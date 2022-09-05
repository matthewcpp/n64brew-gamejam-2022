#pragma once

/** \file ray.h */

#include "framework64/vec3.h"

typedef struct {
    Vec3* position;
    Vec3 direction; // should be normalized
	float scale; 
	unsigned char infinite; // boolean
} Ray;