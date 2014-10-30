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
	std::cout << *sol << std::endl;
	sol->human_readable(std::cout);
}
