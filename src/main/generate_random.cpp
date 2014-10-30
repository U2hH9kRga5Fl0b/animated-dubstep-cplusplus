/*
 * generate_random.cpp
 *
 *  Created on: Oct 29, 2014
 *      Author: thallock
 */

#include "main/main.h"

#include "seed/seed.h"

void generate_random()
{
	Solution* sol = get_random_solution(city);
	log() << *sol << std::endl;
	sol->human_readable(log());
}
