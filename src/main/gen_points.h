/*
 * gen_points.h
 *
 *  Created on: Oct 31, 2014
 *      Author: thallock
 */

#ifndef GEN_POINTS_H_
#define GEN_POINTS_H_

#include "model/coord.h"

typedef enum
{
	UNIFORM_CITY_TYPE,
	CIRCLE_CITY_TYPE,
	CLUSTER_CITY_TYPE,
	RADIAL_CITY_TYPE,
	SPIRAL_CITY_TYPE,
} points_pattern;


void gen_points(Coord* landfills, int num_fills,
		Coord* yards,     int num_yards,
		Coord* requests,  int num_requests,
		points_pattern pattern);





#endif /* GEN_POINTS_H_ */
