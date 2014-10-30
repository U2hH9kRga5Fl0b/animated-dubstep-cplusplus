/*
 * generate_seed.cpp
 *
 *  Created on: Oct 29, 2014
 *      Author: thallock
 */

#include "main/main.h"

#include "seed/seed.h"

void generate_seed()
{
	Solution* sol = get_nearest_seed(city);
	std::cout << *sol << std::endl;
	sol->human_readable(std::cout);
}
