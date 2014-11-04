/*
 * nearest.cpp
 *
 *  Created on: Oct 25, 2014
 *      Author: thallock
 */

#include "seed/seed.h"

#include "main/global.h"
#include "opts/depth_first_search.h"

Solution* get_nearest_seed_dfs(const City* c)
{
	Solution* sol = new Solution { c, 3 * c->num_actions };

	bool *canservice = new bool[c->num_trucks];
	for (int d = 0; d < c->num_trucks; d++)
		canservice[d] = true;

	bool canserviceany;
	do
	{
		canserviceany = false;
		for (int d = 0; d < c->num_trucks; d++)
		{
			int nstops = sol->get_number_of_stops(d);
			insertion ins{d, nstops};

			if (!canservice[d])
			{
				continue;
			}

			bool found;
			if (nstops == 0)
			{
				found = search_for_path(sol, -1, 5, ins, begins{});
			}
			else
			{
				found = search_for_path(sol, -1, 5, ins, has_value{});
			}
			if (found)
			{
				canserviceany = true;
				ins.apply(sol);
			}
		}
	} while (canserviceany);

	delete[] canservice;

	for (int d = 0; d < c->num_trucks; d++)
	{
		insertion ins{d, sol->get_number_of_stops(d)};
		if (search_for_path(sol, -1, 5, ins, ends{}))
		{
			ins.apply(sol);
		}
	}

	is_feasible(sol);

	return sol;
}
