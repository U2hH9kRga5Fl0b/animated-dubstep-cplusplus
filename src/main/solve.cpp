/*
 * solve.cpp
 *
 *  Created on: Oct 29, 2014
 *      Author: thallock
 */

#include "main/main.h"

#include "seed/seed.h"
#include "opts/codon.h"

void solve()
{
	Solution* sol = get_nearest_seed(city);
	exchange_subpath_search(sol, 100);
}
