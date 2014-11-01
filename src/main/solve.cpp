/*
 * solve.cpp
 *
 *  Created on: Oct 29, 2014
 *      Author: thallock
 */

#include "main/global.h"

#include "seed/seed.h"
#include "opts/codon.h"

void solve()
{
	Solution* sol = get_nearest_seed(city);
//	log() << *sol << std::endl;
	viewer.show("seed", sol);
	exchange_subpath_search(sol, 100);
}
