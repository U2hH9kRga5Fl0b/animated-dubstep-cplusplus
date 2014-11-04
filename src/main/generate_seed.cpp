/*
 * generate_seed.cpp
 *
 *  Created on: Oct 29, 2014
 *      Author: thallock
 */

#include "main/global.h"

#include "seed/seed.h"

void generate_seed()
{
	Solution* sol = get_nearest_seed_dfs(city);
	log() << *sol << std::endl;
	sol->human_readable(log());
}
